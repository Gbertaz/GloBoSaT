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

#include "NeoM8n.h"

//This parses the GPS characters
static NMEAGPS gps;

//====================================================================
//                        GPS CONFIG
//====================================================================

// Check that the config files are set up properly
#if !defined( NMEAGPS_PARSE_RMC )
  #error You must uncomment NMEAGPS_PARSE_RMC in NMEAGPS_cfg.h!
#endif

#if !defined( GPS_FIX_TIME )
  #error You must uncomment GPS_FIX_TIME in GPSfix_cfg.h!
#endif

#if !defined( GPS_FIX_LOCATION )
  #error You must uncomment GPS_FIX_LOCATION in GPSfix_cfg.h!
#endif

#if !defined( GPS_FIX_SPEED )
  #error You must uncomment GPS_FIX_SPEED in GPSfix_cfg.h!
#endif

#if !defined( GPS_FIX_SATELLITES )
  #error You must uncomment GPS_FIX_SATELLITES in GPSfix_cfg.h!
#endif

#if !defined( GPS_FIX_HEADING )
  #error You must uncomment GPS_FIX_HEADING in GPSfix_cfg.h!
#endif

#ifdef NMEAGPS_INTERRUPT_PROCESSING
  #error You must *NOT* define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
#endif


//===================================
//Update rate configuration commands
//===================================
const unsigned char _ubxRate1Hz[] PROGMEM = 
{ 0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00 };

const unsigned char _ubxRate2Hz[] PROGMEM = 
{ 0x06,0x08,0x06,0x00,0xF4,0x01,0x01,0x00,0x01,0x00 };

const unsigned char _ubxRate5Hz[] PROGMEM = 
{ 0x06,0x08,0x06,0x00,200,0x00,0x01,0x00,0x01,0x00 };

const unsigned char _ubxRate10Hz[] PROGMEM = 
{ 0x06,0x08,0x06,0x00,100,0x00,0x01,0x00,0x01,0x00 };

const unsigned char _ubxRate16Hz[] PROGMEM = 
{ 0x06,0x08,0x06,0x00,50,0x00,0x01,0x00,0x01,0x00 };


//===================================
//Dynamic platform models
//===================================

const unsigned char _ubxPortable[] PROGMEM = 
{ 
	0x06,0x24,0x24,0x00,0xFF,0xFF,0x00,0x03,0x00,0x00,
	0x00,0x00,0x10,0x27,0x00,0x00,0x05,0x00,0xFA,0x00,
	0xFA,0x00,0x64,0x00,0x5E,0x01,0x00,0x3C,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const unsigned char _ubxStationary[] PROGMEM =
{
	0x06,0x24,0x24,0x00,0xFF,0xFF,0x02,0x03,0x00,0x00,
	0x00,0x00,0x10,0x27,0x00,0x00,0x05,0x00,0xFA,0x00,
	0xFA,0x00,0x64,0x00,0x5E,0x01,0x00,0x3C,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const unsigned char _ubxPedestrian[] PROGMEM =
{ 
	0x06,0x24,0x24,0x00,0xFF,0xFF,0x03,0x03,0x00,0x00,
	0x00,0x00,0x10,0x27,0x00,0x00,0x05,0x00,0xFA,0x00,
	0xFA,0x00,0x64,0x00,0x5E,0x01,0x00,0x3C,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 
};
const unsigned char _ubxAutomotive[] PROGMEM =
{ 
	0x06,0x24,0x24,0x00,0xFF,0xFF,0x04,0x03,0x00,0x00,
	0x00,0x00,0x10,0x27,0x00,0x00,0x05,0x00,0xFA,0x00,
	0xFA,0x00,0x64,0x00,0x5E,0x01,0x00,0x3C,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 
};
const unsigned char _ubxSea[] PROGMEM =
{
	0x06,0x24,0x24,0x00,0xFF,0xFF,0x05,0x03,0x00,0x00,
	0x00,0x00,0x10,0x27,0x00,0x00,0x05,0x00,0xFA,0x00,
	0xFA,0x00,0x64,0x00,0x5E,0x01,0x00,0x3C,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const unsigned char _ubxAirborne1g[] PROGMEM =
{
	0x06,0x24,0x24,0x00,0xFF,0xFF,0x06,0x03,0x00,0x00,
	0x00,0x00,0x10,0x27,0x00,0x00,0x05,0x00,0xFA,0x00,
	0xFA,0x00,0x64,0x00,0x5E,0x01,0x00,0x3C,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const unsigned char _ubxAirborne2g[] PROGMEM = 
{
	0x06,0x24,0x24,0x00,0xFF,0xFF,0x07,0x03,0x00,0x00,
	0x00,0x00,0x10,0x27,0x00,0x00,0x05,0x00,0xFA,0x00,
	0xFA,0x00,0x64,0x00,0x5E,0x01,0x00,0x3C,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const unsigned char _ubxAirborne4g[] PROGMEM =
{
	0x06,0x24,0x24,0x00,0xFF,0xFF,0x08,0x03,0x00,0x00,
	0x00,0x00,0x10,0x27,0x00,0x00,0x05,0x00,0xFA,0x00,
	0xFA,0x00,0x64,0x00,0x5E,0x01,0x00,0x3C,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const unsigned char _ubxWrist[] PROGMEM =
{ 
	0x06,0x24,0x24,0x00,0xFF,0xFF,0x09,0x03,0x00,0x00,
	0x00,0x00,0x10,0x27,0x00,0x00,0x05,0x00,0xFA,0x00,
	0xFA,0x00,0x64,0x00,0x5E,0x01,0x00,0x3C,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 
};

GpsNeoM8n::GpsNeoM8n() {
	
}

void GpsNeoM8n::begin(unsigned long baud, ubxUpdateRate updateRate, ubxPlatform platform, trackingMode trackingMode) {
	_updateRateRequest = (uint8_t)updateRate;
	_dynamicPlatformRequest = (uint8_t)platform;
	_trackingModeRequest = (uint8_t)trackingMode;
	_targetDistanceRequest = (uint8_t)meters10;
	_commandRequest = waitingRequest;
	cb_onQuietTime = NULL;
	cb_onTrackData = NULL;

	gpsPort.begin(baud);
	delay(50);
	setDynamicPlatform();
	delay(50);
	setUpdateRate();
	delay(50);
	setTrackingMode();
	delay(50);
	setFixTargetDistance();
	delay(50);
}


//==============================================================================================
//									PUBLIC
//==============================================================================================

void GpsNeoM8n::update() {
	while (gps.available(gpsPort)) {
		_fix = gps.read();
		checkIncomingCommands();
		if(cb_onQuietTime) (*cb_onQuietTime)();
		if(!_fix.valid.location && !_fix.valid.altitude) return;
		if(cb_onValidFix)(*cb_onValidFix)(_fix);

		bool newFixAvailable = false;

		if(_trackingMode == modeDistance){
			newFixAvailable = isFixTargetDistanceReached(_fix);

			if(_fix.valid.speed){
				uint8_t optimalRate = calculateOptimalUpdateRate(_fix.speed_kph());
				if(_updateRate != optimalRate) requestUpdateRate((ubxUpdateRate)optimalRate);
			}
		}
		else if(_trackingMode == modeTime){
			newFixAvailable = true;
		}
		else {
			newFixAvailable = true;
		}

		if(newFixAvailable){
			//Invoke callback
			if(cb_onTrackData) (*cb_onTrackData)();
		}
	}
}

void GpsNeoM8n::requestUpdateRate(ubxUpdateRate updateRate) {
	if(_commandRequest != waitingRequest) return;	//Make sure there isn't already another command request
	_commandRequest = rate;
	_updateRateRequest = (uint8_t)updateRate;
}

//Dynamic platform model Navigation Engine Settings
void GpsNeoM8n::requestDynamicPlatform(ubxPlatform dynamicPlatform) {
	if(_commandRequest != waitingRequest) return;
	_commandRequest = platform;
	_dynamicPlatformRequest = (uint8_t)dynamicPlatform;
}

void GpsNeoM8n::requestTrackingMode(trackingMode trackingMode){
	if(_commandRequest != waitingRequest) return;
	_commandRequest = mode;
	_trackingModeRequest = (uint8_t)trackingMode;
}

void GpsNeoM8n::requestFixTargetDistance(fixTargetDistance targetDistance){
	if(_commandRequest != waitingRequest) return;
	_commandRequest = meters;
	_targetDistanceRequest = (uint8_t)targetDistance;
}

//==============================================================================================
//									PRIVATE
//==============================================================================================

bool GpsNeoM8n::isFixTargetDistanceReached(gps_fix &fix){
	if(fix.valid.location == false) return false;

	//If this is the first fix returned by the Gps
	if(_previousTargetFix.location.lat() == 0 && _previousTargetFix.location.lon() == 0) {
		_previousTargetFix = fix;
		return true;
	}

	float meters = fix.location.DistanceKm(_previousTargetFix.location) * 1000;

	if(meters >= _targetDistance){
		_previousTargetFix = fix;
		return true;
	}

	return false;
}


//===============================
//When Tracking mode is set on Distance, the Gps update rate
//must be set to a value that, at current speed (plus a margin percent),
//allows the Gps receiver to detect, between two fixes, a movement 
//at least equal to the Target Distance (in meters).
//This function calculates the required Gps Update Rate [Hz] 
//based on current speed [Km/h] and Target Distance [m]
uint8_t GpsNeoM8n::calculateOptimalUpdateRate(float speed){
	if(speed == 0) return 1;
	float margin = 0.15;	//Margin percent on speed
	uint8_t rate = ceil(1 / (_targetDistance / (speed + (speed * margin)) * 3.6));
	if(rate == 0) rate = 1;
	if(rate == 3 || rate == 4) rate = 5;
	if(rate > 5) rate = 10;
	return rate;
}

void GpsNeoM8n::checkIncomingCommands(){
	if(_commandRequest == waitingRequest) return;
	switch(_commandRequest){
		case rate:
		setUpdateRate();
		break;
		case platform:
		setDynamicPlatform();
		break;
		case mode:
		setTrackingMode();
		break;
		case meters:
		setFixTargetDistance();
		break;
	}

	_commandRequest = waitingRequest;
}

void GpsNeoM8n::setUpdateRate(){
	switch (_updateRateRequest) {
	case ubxRate1Hz:
		sendUBXCommand(_ubxRate1Hz, sizeof(_ubxRate1Hz));
		_updateRate = ubxRate1Hz;
		break;
	case ubxRate2Hz:
		sendUBXCommand(_ubxRate2Hz, sizeof(_ubxRate2Hz));
		_updateRate = ubxRate2Hz;
		break;
	case ubxRate5Hz:
		sendUBXCommand(_ubxRate5Hz, sizeof(_ubxRate5Hz));
		_updateRate = ubxRate5Hz;
		break;
	case ubxRate10Hz:
		sendUBXCommand(_ubxRate10Hz, sizeof(_ubxRate10Hz));
		_updateRate = ubxRate10Hz;
		break;
	}

	#ifdef DEBUG_GPS_SETTINGS
		Serial.print("GPS SETTINGS: set rate ");
		Serial.print(_updateRate);
		Serial.print(" Hz");
		Serial.println("");
  	#endif
}

void GpsNeoM8n::setDynamicPlatform(){
	switch (_dynamicPlatformRequest) {
	case ubxPortable:
		sendUBXCommand(_ubxPortable, sizeof(_ubxPortable));
		_dynamicPlatform = ubxPortable;
		break;
	case ubxStationary:
		sendUBXCommand(_ubxStationary, sizeof(_ubxStationary));
		_dynamicPlatform = ubxStationary;
		break;
	case ubxPedestrian:
		sendUBXCommand(_ubxPedestrian, sizeof(_ubxPedestrian));
		_dynamicPlatform = ubxPedestrian;
		break;
	case ubxAutomotive:
		sendUBXCommand(_ubxAutomotive, sizeof(_ubxAutomotive));
		_dynamicPlatform = ubxAutomotive;
		break;
	case ubxSea:
		sendUBXCommand(_ubxSea, sizeof(_ubxSea));
		_dynamicPlatform = ubxSea;
		break;
	case ubxAirborne1g:
		sendUBXCommand(_ubxAirborne1g, sizeof(_ubxAirborne1g));
		_dynamicPlatform = ubxAirborne1g;
		break;
	case ubxAirborne2g:
		sendUBXCommand(_ubxAirborne2g, sizeof(_ubxAirborne2g));
		_dynamicPlatform = ubxAirborne2g;
		break;
	case ubxAirborne4g:
		sendUBXCommand(_ubxAirborne4g, sizeof(_ubxAirborne4g));
		_dynamicPlatform = ubxAirborne4g;
		break;
	case ubxWrist:
		sendUBXCommand(_ubxWrist, sizeof(_ubxWrist));
		_dynamicPlatform = ubxWrist;
		break;
	}

	#ifdef DEBUG_GPS_SETTINGS
		Serial.print("GPS SETTINGS: set dynamic platform ");
		Serial.print(_dynamicPlatform);
		Serial.println("");
  	#endif
}

void GpsNeoM8n::setTrackingMode(){
	switch(_trackingModeRequest){
		case modeAuto:
		_trackingMode = modeAuto;
		break;
		case modeTime:
		_trackingMode = modeTime;
		break;
		case modeDistance:
		_previousTargetFix.location.init();	//Reset _lat and _lon to zero
		_trackingMode = modeDistance;
		break;
	}

	#ifdef DEBUG_GPS_SETTINGS
		Serial.print("GPS SETTINGS: set tracking mode ");
		Serial.print(_trackingMode);
		Serial.println("");
  	#endif
}

void GpsNeoM8n::setFixTargetDistance(){
	_targetDistance = (uint8_t)_targetDistanceRequest;

	#ifdef DEBUG_GPS_SETTINGS
		Serial.print("GPS SETTINGS: set target distance ");
		Serial.print(_targetDistance);
		Serial.println("");
  	#endif
}

void GpsNeoM8n::sendUBXCommand(const unsigned char *bytes, size_t len)
{
	gpsPort.write( 0xB5 ); // SYNC1
	gpsPort.write( 0x62 ); // SYNC2

	uint8_t a = 0, b = 0;
	while (len-- > 0) {
		uint8_t c = pgm_read_byte( bytes++ );
		a += c;
		b += a;
		gpsPort.write( c );
	}

	gpsPort.write( a ); // CHECKSUM A
	gpsPort.write( b ); // CHECKSUM B
}