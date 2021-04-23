// MIT License
//
// Copyright(c) 2021 Giovanni Bertazzoni <nottheworstdev@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "src/Display.h"
#include "src/NeoM8n.h"
#include "src/SdCard/SdLogger.h"
#include "src/SdCard/SdTrackReader.h"
#include "src/Config.h"
#include "src/NavigationData.h"
#include "src/TemperatureDs18b20.h"
#include "src/Nextion/NextionDisplay.h"
#include "src/Map/Map.h"
#include "src/SdCard/SdMapLoader.h"
#include "src/Battery.h"
#include <USBHost_t36.h>
#include <TeensyThreads.h>

NavData navData;
GpsNeoM8n gpsM8n;
SdCardLogger sdLogger;
NextionDisplay nexDisplay;
TemperatureSensor tempSensor;
TrackReader trackReader;
GlobosatMap maps;
MapLoader mapLoader;
Battery battery;

USBHost myusb;
AntPlus ant1(myusb);

//====================================================================
//                        MACHINE STATE
//====================================================================

typedef enum states 
{ 
  IDLING,           //0
  SD_CARD_FAILED,   //1
  TRACKING,         //2
  FIX_WAIT,         //3
  FIX_LOST          //4
} state;

state _lastState;                    //Previous state
state _currentState;                 //Current state

  
//====================================================================
//                        SETUP
//====================================================================

void setup()
{
    //Init display and display loop on a parallel thread
    nexDisplay.setup();
    threads.addThread(displayLoop);
    delay(50);
  
  #ifdef SIMULATED
  
    bool succeeded = trackReader.init((char *)TRACK_FILE_PATH);
    if(succeeded == false) return;
    trackReader.onGpsQuietTime(handleGpsQuietTime);
    
  #else

    #ifdef DEBUG_INFO
      while (!Serial);
    #endif
    
    pinMode(LED_BUILTIN, OUTPUT);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    
    //Initial state
    if(LOG_ON_STARTUP){
      _currentState = FIX_WAIT;
      _lastState = IDLING;
    }
    else {
      _currentState = IDLING;
      _lastState = FIX_WAIT;
    }
  
    //Init GPS port, update rate, dynamic platform and tracking mode
    gpsM8n.begin(921600, GpsNeoM8n::ubxRate5Hz, GpsNeoM8n::ubxAutomotive, GpsNeoM8n::modeTime);
    gpsM8n.onQuietTime(handleGpsQuietTime);
    gpsM8n.onValidFix(hangleGpsValidFix);
    gpsM8n.onTrackData(handleGpsTrackData);
    
    navData.init();
  
    delay(50);
    
    tempSensor.begin();
    tempSensor.onTemperatureChange(handleTemperatureChange);
  
    delay(50);
    
    battery.begin(BATTERY_MIN_VOLTAGE, BATTERY_MAX_VOLTAGE, BATTERY_ANALOG_PING);
    battery.onStatusChange(handleBatteryStatusChange);

    delay(50);

    maps.onMarkerPositionChange(handleMapsPositionChange);
    mapLoader.load((char*)MAP_TILES_CALIBRATION, maps.tiles);

    delay(50);

    myusb.begin();
    ant1.begin();
    ant1.onStatusChange(handleStatusChange);
    ant1.onDeviceID(handleDeviceID);
    ant1.onHeartRateMonitor(handleHeartRateMonitor);
  
  #endif
}



//====================================================================
//                        LOOP
//====================================================================

void loop()
{
  #ifdef SIMULATED
  
    trackReader.update(navData);
    
  #else
  
    gpsM8n.update();
    myusb.Task();
    
  #endif
}


void displayLoop(){
  while(1){
    nexDisplay.update(navData);
    threads.yield();
  }
}

//====================================================================
//                        CALLBACKS
//====================================================================

  //GPS quiet time routine. 
  //It occurs every: 1000 / Gps update rate[Hz] [ms]
  //To avoid losing Gps data, any time consuming task 
  //must be put inside this routine.
void handleGpsQuietTime()
{ 
  #ifndef SIMULATED

    navData.gpsUpdateRate = gpsM8n.getUpdateRate();
    navData.gpsDynamicPlatform = gpsM8n.getDynamicPlatform();
    navData.gpsTrackingMode = gpsM8n.getTrackingMode();
    navData.gpsFixTargetDistance = gpsM8n.getFixTargetDistance();
  
    battery.update();
    tempSensor.update();
    sdLogger.update();
    maps.update(navData.latitude, navData.longitude);
    
    switch(_currentState){
      case IDLING:
        loop_IDLING();
      break;
      case SD_CARD_FAILED:
        loop_SD_CARD_FAILED();
      break;
      case TRACKING:
        loop_TRACKING();
      break;
      case FIX_WAIT:
        loop_FIX_WAIT();
      break;
      case FIX_LOST:
        loop_FIX_LOST();
      break;
    }
  
  #endif
}

//Invoked when a valid 2D fix is returned by the Gps
void hangleGpsValidFix(const gps_fix &fix){
  navData.update(fix);
}

//Invoked when the Gps signal is lost. * NOT USED FOR NOW *
void handleGpsFixLost(){
  if (_currentState == TRACKING)
    _currentState = FIX_LOST;
}

//Invoked when a new Gps fix is available for logging
void handleGpsTrackData(){
   sdLogger.log(navData);
}

//Invoked when the battery status changes
void handleBatteryStatusChange(uint16_t voltage, uint16_t percentage){
  navData.batteryStatus.voltage = voltage;
  navData.batteryStatus.percentage = percentage;
}

//Invoked when the temperature changes
void handleTemperatureChange(float temperature, bool valid){
  navData.setTemperature(temperature, valid);
}

void handleMapsPositionChange(float x, float y, uint16_t tileId){
  navData.mapData.x = x;
  navData.mapData.y = y;
  navData.mapData.tileId = tileId;
}

//Invoked when we receive the heart bpm from the ant+ cardio belt
void handleHeartRateMonitor(int beatsPerMinute, int milliseconds, int sequenceNumber) {
  navData.setHeartData(beatsPerMinute, milliseconds, sequenceNumber);
  /*
  Serial.print("HRM: sequence:");
  Serial.print(sequenceNumber);
  Serial.print(", interval:");
  Serial.print(milliseconds);
  Serial.print("ms, bpm:");
  Serial.println(beatsPerMinute);
  */
}

void handleStatusChange(int channel, int status) {
  /*
  Serial.print("Channel ");
  Serial.print(channel);
  Serial.print(" status: ");
  switch (status) {
    case 0: Serial.println("STATUS UNASSIGNED CHANNEL"); break;
    case 1: Serial.println("STATUS ASSIGNED CHANNEL"); break;
    case 2: Serial.println("STATUS SEARCHING CHANNEL"); break;
    case 3: Serial.println("STATUS TRACKING_CHANNEL"); break;
    default: Serial.println("UNKNOWN STATUS STATE");
  }
  */
}

void handleDeviceID(int channel, int devId, int devType, int transType) {

  /*
  Serial.print("Device found on channel ");
  Serial.print(channel);
  Serial.print(": deviceId:");
  Serial.print(devId);
  Serial.print(", deviceType:");
  Serial.print(devType);
  Serial.print(", transType:");
  Serial.println(transType);
  */
  
}

//====================================================================
//                        MACHINE STATE LOOPS
//====================================================================

unsigned long _sdFailedMillis = 0;

void loop_IDLING(){ }

void loop_SD_CARD_FAILED(){
  if(millis() - _sdFailedMillis < SD_INIT_RETRY_INTERVAL) return;
  _currentState = FIX_WAIT;
}

void loop_TRACKING(){ }

void loop_FIX_WAIT(){

  //Remains in this state until the GPS has a valid 4D fix (location and time)
  if(navData.hasValid4DFix() == false) return;
  
  //Init the log file:
  //creates the directory on SD card if it doesn't exist
  //the name of the directory is the current date in the format "ddmmyyyy"
  //the log file name is in the format "data_xxxx.log" where xxxx is a progressive number from 0 to 10000
  bool initSucceeded = sdLogger.initLogFile(navData.date.day, navData.date.month, navData.date.year);
  
  if(initSucceeded){
    digitalWrite(LED_BUILTIN, LOW);
    _currentState = TRACKING;
  }
  else {
    _currentState = SD_CARD_FAILED;
    _sdFailedMillis = millis();
  }
}

void loop_FIX_LOST(){

  //Once the Gps has a valid 4D fix (location and time), goes back to TRACKING mode
  if(navData.hasValid4DFix()) 
    _currentState = TRACKING;
}



//====================================================================
//                        DISPLAY CALLBACKS
//====================================================================

//Invoked every time a new page is displayed
void onPageChange(uint8_t pageId){
  nexDisplay.onPageChange();
}

//Invoked when the user sets the target distance from the Display setting page
void onGpsTargetDistanceSet(uint8_t meters){
  gpsM8n.requestFixTargetDistance((GpsNeoM8n::fixTargetDistance)meters);
}

//Invoked when the user sets the Gps update rate from the Display setting page
void onGpsRateSet(uint8_t updateRate){
  gpsM8n.requestUpdateRate((GpsNeoM8n::ubxUpdateRate)updateRate);
}

//Callback invoked when the user sets the Gps dynamic platfom from the Display setting page
void onGpsPlatformSet(uint8_t platformId){
  gpsM8n.requestDynamicPlatform((GpsNeoM8n::ubxPlatform)platformId);
}

//Callback invoked when the user sets the Gps tracking mode from the Display setting page
void onGpsTrackingModeSet(uint8_t trackingModeId){
  gpsM8n.requestTrackingMode((GpsNeoM8n::trackingMode)trackingModeId);
}
