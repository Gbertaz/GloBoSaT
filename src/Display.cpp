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

#include "Display.h"

NexDisplay myNex(Serial2);
//ChartRenderer chartAltitude(myNex, 15, 15, 210, 120);


//==============================================================================================
//	NEXTION GRAPHICS EDITOR OBJECTS' NAMES
//==============================================================================================

//Object names are global so they are unique globally and not locally on the page.
//This is to avoid writing into a wrong object when changing pages quickly.
static const char *HOME_SPEED 				= "t0.txt";
static const char *HOME_ODOMETER 			= "t1.txt";
static const char *HOME_TRIP_TIME 			= "t2.txt";
static const char *HOME_ELEVATION 			= "t3.txt";
static const char *HOME_TEMPERATURE 		= "t4.txt";
static const char *HOME_BATTERY				= "B.val";		//Battery percentage
static const char *TIME_CLOCK 				= "t5.txt";
static const char *TIME_TRIP 				= "t6.txt";
static const char *TIME_MOVING 				= "t7.txt";
static const char *TIME_STOPPED 			= "t8.txt";
static const char *TIME_LAST_STOP 			= "t9.txt";
static const char *TIME_DAWN 				= "t10.txt";
static const char *TIME_DUSK 				= "t11.txt";
static const char *SPEED_CURRENT 			= "t12.txt";
static const char *SPEED_MAX 				= "t13.txt";
static const char *SPEED_AVG_MOVING 		= "t14.txt";
static const char *SPEED_AVG_TOTAL 			= "t15.txt";
static const char *ELEVATION_CURRENT 		= "t16.txt";
static const char *ELEVATION_MAX 			= "t17.txt";
static const char *ELEVATION_MIN 			= "t18.txt";
static const char *LOCATION_LATITUDE 		= "t19.txt";
static const char *LOCATION_LONGITUDE 		= "t20.txt";
static const char *LOCATION_ODOMETER 		= "t21.txt";
static const char *LOCATION_HOME_DISTANCE 	= "t22.txt";
static const char *LOCATION_HEADING 		= "t23.txt";
static const char *LOCATION_SATELLITES 		= "t24.txt";
static const char *COMPASS_SPEED 			= "t25.txt";
static const char *COMPASS_DISTANCE 		= "t26.txt";
static const char *COMPASS_DEGREES	 		= "p1.pic";
static const char *SETTINGS_GPS_RATE 		= "R.val";	//R: Rate => Gps update rate
static const char *SETTINGS_GPS_PLATFORM	= "D.val";	//D: Dynamic platform
static const char *SETTINGS_GPS_MODE		= "T.val";	//T: Tracking mode
static const char *SETTINGS_GPS_METERS		= "M.val";	//M: Meters => required traveled meters to trigger a log on sd card
static const char *MAP_PIN_X				= "xPin.val";	//X screen coordinate
static const char *MAP_PIN_Y				= "yPin.val";	//Y screen coordinate
static const char *MAP_TILE_ID				= "tileId.val";
static const char *HEALTH_HEART_BPM			= "t27.txt";
static const char *HEALTH_HEART_MAX			= "t28.txt";
static const char *HEALTH_HEART_MIN			= "t29.txt";


NextionDisplay::NextionDisplay() {
	myNex.currentPageId = 0;
	myNex.lastPageId = 99;
	_lastUpdateMillis = 0;
	_pageChanged = true;
}

void NextionDisplay::setup() {
	myNex.begin(115200);
	delay(100);
	_updateInterval = (1000 / DISPLAY_TARGET_FPS);

	// Set the Nextion display to page 0
	myNex.sendCommand("page 0");
}

void NextionDisplay::update(const NavData &navData) {
	if(millis() - _lastUpdateMillis < _updateInterval) return;

	myNex.listen();

	bool newDataSent = false;

	#ifdef DEBUG_DISPLAY
		unsigned long t1 = millis();
	#endif

	//Updates only the current page
	switch (myNex.currentPageId) {
	case 0:
		newDataSent = refreshPage0(navData);
		break;
	case 1:
		newDataSent = refreshPage1(navData);
		break;
	case 2:
		newDataSent = refreshPage2(navData);
		break;
	case 3:
		newDataSent = refreshPage3(navData);
		break;
	case 4:
		newDataSent = refreshPage4(navData);
		break;
	case 5:
		newDataSent = refreshPage5(navData);
		break;
	case 6:
		newDataSent = refreshPage6(navData);
		break;
	case 7:
		newDataSent = refreshPage7(navData);
		break;
	case 8:
		newDataSent = refreshPage8(navData);
		break;
	}

	_pageChanged = false;
	
	#ifdef DEBUG_DISPLAY
		unsigned long updateTime = millis() - t1;
		Serial.print("Display update interval: ");
		Serial.print(millis() - _lastUpdateMillis - updateTime);
		Serial.print(" ms");
		Serial.println("");

	if (newDataSent) {	
			Serial.print("Display update time: ");
			Serial.print(updateTime);
			Serial.print(" ms");
			Serial.println("");
		}
	#endif

	_lastUpdateMillis = millis();
}

void NextionDisplay::onPageChange(){
	_pageChanged = true;

	#ifdef DEBUG_DISPLAY
		Serial.print("Display onPageChange: ");
		Serial.println(myNex.currentPageId);
	#endif
}

//=================================================================================================================================================
//=================================================================================================================================================
//	PAGE 0 --- HOME
//=================================================================================================================================================
//=================================================================================================================================================
bool NextionDisplay::refreshPage0(const NavData &navData) {

	bool newDataSent = false;

	//-----------------------------
	//Battery percentage
	if(_lastDisplayData.batteryStatus.percentage != navData.batteryStatus.percentage || _pageChanged){
		_lastDisplayData.batteryStatus = navData.batteryStatus;
		myNex.setNumber(HOME_BATTERY, (uint32_t)navData.batteryStatus.percentage);
		newDataSent = true;
	}

	//-----------------------------
	//Speed with tolerance
	bool send = false;
	if (_lastDisplayData.speed != navData.speed || _pageChanged || send) {
		_lastDisplayData.speed = navData.speed;
		char data[8];
		sprintf(data, "%.0f", _lastDisplayData.speed);      //IF THE GPS IS IN PEDESTRIAN MODE IT MIGHT BE USEFULL TO SHOW THE DECIMALS
		myNex.setText(HOME_SPEED, data);
		newDataSent = true;
	}

	//-----------------------------
	//Odometer
	if (_lastDisplayData.distanceTotalKm != navData.distanceTotalKm || _pageChanged || send) {
		_lastDisplayData.distanceTotalKm = navData.distanceTotalKm;
		char data[9];
		sprintf(data, "%.1f", (_lastDisplayData.distanceTotalKm < 1) ? _lastDisplayData.distanceTotalKm * 1000 : _lastDisplayData.distanceTotalKm);
		myNex.setText(HOME_ODOMETER, data);
		newDataSent = true;
	}

	//-----------------------------
	//Moving time
	if (_lastDisplayData.movingTime != navData.movingTime || _pageChanged) {
		_lastDisplayData.movingTime = navData.movingTime;
		char data[10];
		sprintf(data, "%02d:%02d:%02d", _lastDisplayData.movingTime.hours, _lastDisplayData.movingTime.minutes, _lastDisplayData.movingTime.seconds);
		myNex.setText(HOME_TRIP_TIME, data);
		newDataSent = true;
	}

	//-----------------------------
	//Altitude with tolerance
	if (_lastDisplayData.altitude != navData.altitude || _pageChanged) {
		_lastDisplayData.altitude = navData.altitude;
		char data[6];
		sprintf(data, "%.0f", _lastDisplayData.altitude);
		myNex.setText(HOME_ELEVATION, data);
		newDataSent = true;
	}
	
	#ifdef DEBUG_GPS_FIX_SKIPPED
		//Adding the number of missed Gps fixes for debug
		if (_lastDisplayData.missedFixCount != navData.missedFixCount || _pageChanged) {
			_lastDisplayData.missedFixCount = navData.missedFixCount;
			char data[10];
			sprintf(data, "%lu", _lastDisplayData.missedFixCount);
			myNex.setText(HOME_TEMPERATURE, data);
			newDataSent = true;
		}
	#else
		//-----------------------------
		//Temperature
		if (_lastDisplayData.temperature.current != navData.temperature.current || _pageChanged) {
			_lastDisplayData.temperature = navData.temperature;
			char data[10];

			if(_lastDisplayData.temperature.currentValid){
				sprintf(data, "%.1f\xB0 C", _lastDisplayData.temperature.current);
			}
			else {
				sprintf(data, "n.a");
			}

			myNex.setText(HOME_TEMPERATURE, data);
			newDataSent = true;
		}
	#endif

	return newDataSent;
}



//=================================================================================================================================================
//=================================================================================================================================================
//	PAGE 1 --- TIME
//=================================================================================================================================================
//=================================================================================================================================================
bool NextionDisplay::refreshPage1(const NavData &navData) {

	bool newDataSent = false;

	//-----------------------------
	//Date and time
	if (_lastDisplayData.date != navData.date || _lastDisplayData.timeUTC != navData.timeUTC || _pageChanged)
	{
		_lastDisplayData.date = navData.date;
		_lastDisplayData.timeUTC = navData.timeUTC;

		char data[20];
		if(_lastDisplayData.date.month == 0) _lastDisplayData.date.month = 1;
		sprintf(data, "%02d %s %02d:%02d", _lastDisplayData.date.day, _months[_lastDisplayData.date.month - 1], _lastDisplayData.timeUTC.hours, _lastDisplayData.timeUTC.minutes);
		myNex.setText(TIME_CLOCK, data);
		newDataSent = true;
	}

	//-----------------------------
	//Total trip time
	if (_lastDisplayData.totalTime != navData.totalTime || _pageChanged) {
		_lastDisplayData.totalTime = navData.totalTime;
		char data[6];
		sprintf(data, "%02d:%02d", _lastDisplayData.totalTime.hours, _lastDisplayData.totalTime.minutes);
		myNex.setText(TIME_TRIP, data);
		newDataSent = true;
	}

	//-----------------------------
	//Moving time
	if (_lastDisplayData.movingTime != navData.movingTime || _pageChanged) {
		_lastDisplayData.movingTime = navData.movingTime;
		char data[6];
		sprintf(data, "%02d:%02d", _lastDisplayData.movingTime.hours, _lastDisplayData.movingTime.minutes);
		myNex.setText(TIME_MOVING, data);
		newDataSent = true;
	}

	//-----------------------------
	//Total stopped time
	if (_lastDisplayData.totalStoppedTime != navData.totalStoppedTime || _pageChanged) {
		_lastDisplayData.totalStoppedTime = navData.totalStoppedTime;
		char data[6];
		sprintf(data, "%02d:%02d", _lastDisplayData.totalStoppedTime.hours, _lastDisplayData.totalStoppedTime.minutes);
		myNex.setText(TIME_STOPPED, data);
		newDataSent = true;
	}

	//-----------------------------
	//Last stopped time
	if (_lastDisplayData.lastStoppedTime != navData.lastStoppedTime || _pageChanged) {
		_lastDisplayData.lastStoppedTime = navData.lastStoppedTime;
		char data[6];
		sprintf(data, "%02d:%02d", _lastDisplayData.lastStoppedTime.hours, _lastDisplayData.lastStoppedTime.minutes);
		myNex.setText(TIME_LAST_STOP, data);
		newDataSent = true;
	}

	//-----------------------------
	//Sunrise and Sunset
	if (_pageChanged) {
		myNex.setText(TIME_DAWN, navData.sunrise);
		myNex.setText(TIME_DUSK, navData.sunset);
		newDataSent = true;
	}

	return newDataSent;
}




//=================================================================================================================================================
//=================================================================================================================================================
//	PAGE 2 --- SPEED
//=================================================================================================================================================
//=================================================================================================================================================
bool NextionDisplay::refreshPage2(const NavData &navData) {

	bool newDataSent = false;

	//-----------------------------
	//Speed
	if (_lastDisplayData.speed != navData.speed || _pageChanged) {
		_lastDisplayData.speed = navData.speed;

		char data[8];
		sprintf(data, "%.1f", _lastDisplayData.speed);
		myNex.setText(SPEED_CURRENT, data);
		newDataSent = true;
	}

	//-----------------------------
	//Max speed
	if (_lastDisplayData.speedMax != navData.speedMax || _pageChanged) {
		_lastDisplayData.speedMax = navData.speedMax;

		char data[8];
		sprintf(data, "%.1f", _lastDisplayData.speedMax);
		myNex.setText(SPEED_MAX, data);
		newDataSent = true;
	}

	//-----------------------------
	//Moving average speed
	if (_lastDisplayData.speedMovingAvg != navData.speedMovingAvg || _pageChanged) {
		_lastDisplayData.speedMovingAvg = navData.speedMovingAvg;

		char data[8];
		sprintf(data, "%.1f", _lastDisplayData.speedMovingAvg);
		myNex.setText(SPEED_AVG_MOVING, data);
		newDataSent = true;
	}

	//-----------------------------
	//Totale average speed (moving and not moving time)
	if (_lastDisplayData.speedTotalAvg != navData.speedTotalAvg || _pageChanged) {
		_lastDisplayData.speedTotalAvg = navData.speedTotalAvg;

		char data[8];
		sprintf(data, "%.1f", _lastDisplayData.speedTotalAvg);
		myNex.setText(SPEED_AVG_TOTAL, data);
		newDataSent = true;
	}


	return newDataSent;
}




//=================================================================================================================================================
//=================================================================================================================================================
//	PAGE 3 --- ALTITUDE
//=================================================================================================================================================
//=================================================================================================================================================
bool NextionDisplay::refreshPage3(const NavData &navData) {

	bool newDataSent = false;


	//-----------------------------
	//Current altitude
	if (_lastDisplayData.altitude != navData.altitude || _pageChanged) {
		_lastDisplayData.altitude = navData.altitude;

		char data[6];
		sprintf(data, "%.0f", _lastDisplayData.altitude);
		myNex.setText(ELEVATION_CURRENT, data);

		//int value = map(_lastDisplayData.altitude, 0, 5000, 0, 200);

		//char dataChart[15];
		//sprintf(dataChart, "add 13,0,%d", value);
		//myNex.sendCommand(dataChart);

		//Update chart altitude
		/*ChartRenderer::point_g point;
		point.value = (int)_lastDisplayData.altitude;
		point.minutes = (int)_lastDisplayData.timeUTC.minutes;
		chartAltitude.update(point);*/

		newDataSent = true;
	}

	//-----------------------------
	//Altitude max
	if(_lastDisplayData.altitudeMax != navData.altitudeMax || _pageChanged){
		_lastDisplayData.altitudeMax = navData.altitudeMax;

		char data[6];
		sprintf(data, "%.0f", _lastDisplayData.altitudeMax);
		myNex.setText(ELEVATION_MAX, data);
		newDataSent = true;
	}

	//-----------------------------
	//Altitude min
	if(_lastDisplayData.altitudeMin != navData.altitudeMin || _pageChanged){
		_lastDisplayData.altitudeMin = navData.altitudeMin;

		char data[6];
		sprintf(data, "%.0f", _lastDisplayData.altitudeMin);
		myNex.setText(ELEVATION_MIN, data);
		newDataSent = true;
	}

	return newDataSent;
}




//=================================================================================================================================================
//=================================================================================================================================================
//	PAGE 4 --- LOCATION
//=================================================================================================================================================
//=================================================================================================================================================
bool NextionDisplay::refreshPage4(const NavData &navData) {

	bool newDataSent = false;
	
	//-----------------------------
	//Latitude
	if(_lastDisplayData.latitudeDMS != navData.latitudeDMS || _pageChanged){
		_lastDisplayData.latitudeDMS = navData.latitudeDMS;
	  char data[20];
	  //Coordinates in DMS format
	  sprintf(data, "%c %02d\xB0 %02d' %.2f''", _lastDisplayData.latitudeDMS.NS, _lastDisplayData.latitudeDMS.degrees, _lastDisplayData.latitudeDMS.minutes, _lastDisplayData.latitudeDMS.seconds);
	  myNex.setText(LOCATION_LATITUDE, data);
	  newDataSent = true;
	}
	
	//-----------------------------
	//Longitude
	if (_lastDisplayData.longitudeDMS != navData.longitudeDMS || _pageChanged) {
		_lastDisplayData.longitudeDMS = navData.longitudeDMS;
		char data[20];
		//Coordinates in DMS format
		sprintf(data, "%c%03d\xB0 %02d' %.2f''", _lastDisplayData.longitudeDMS.EW, _lastDisplayData.longitudeDMS.degrees, _lastDisplayData.longitudeDMS.minutes, _lastDisplayData.longitudeDMS.seconds);
		myNex.setText(LOCATION_LONGITUDE, data);
		newDataSent = true;
	}

	//-----------------------------
	//Odometer
	if (_lastDisplayData.distanceTotalKm != navData.distanceTotalKm || _pageChanged) {
		_lastDisplayData.distanceTotalKm = navData.distanceTotalKm;
		char data[9];
		sprintf(data, "%.1f", (_lastDisplayData.distanceTotalKm < 1) ? _lastDisplayData.distanceTotalKm * 1000 : _lastDisplayData.distanceTotalKm);
		myNex.setText(LOCATION_ODOMETER, data);
		newDataSent = true;
	}

	//-----------------------------
	//Distance from home (straight line)
	if (_lastDisplayData.distanceFromHome != navData.distanceFromHome || _pageChanged) {
		_lastDisplayData.distanceFromHome = navData.distanceFromHome;
		char data[9];
		sprintf(data, "%.1f", _lastDisplayData.distanceFromHome);
		myNex.setText(LOCATION_HOME_DISTANCE, data);
		newDataSent = true;
	}

	//-----------------------------
	//Heading
	if((int)_lastDisplayData.heading != (int)navData.heading || _pageChanged){
		_lastDisplayData.heading = navData.heading;

		char data[5];
		sprintf(data, "%d\xB0", (int)_lastDisplayData.heading);
		myNex.setText(LOCATION_HEADING, data);
		newDataSent = true;
	}

	//-----------------------------
	//Number of satellites
	if (_lastDisplayData.satellites != navData.satellites || _pageChanged) {
		_lastDisplayData.satellites = navData.satellites;
		char data[3];
		sprintf(data, "%02d", _lastDisplayData.satellites);
		myNex.setText(LOCATION_SATELLITES, data);
		newDataSent = true;
	}

	return newDataSent;
}



//=================================================================================================================================================
//=================================================================================================================================================
//	PAGE 5 --- GPS SETTINGS
//=================================================================================================================================================
//=================================================================================================================================================
bool NextionDisplay::refreshPage5(const NavData &navData){

	bool newDataSent = false;

	//if (_lastDisplayData.gpsUpdateRate != navData.gpsUpdateRate || _pageChanged) {
		_lastDisplayData.gpsUpdateRate = navData.gpsUpdateRate;
		myNex.setNumber(SETTINGS_GPS_RATE, (uint32_t)navData.gpsUpdateRate);
		newDataSent = true;
	//}

	//if (_lastDisplayData.gpsDynamicPlatform != navData.gpsDynamicPlatform || _pageChanged) {
		_lastDisplayData.gpsDynamicPlatform = navData.gpsDynamicPlatform;
		myNex.setNumber(SETTINGS_GPS_PLATFORM, (uint32_t)navData.gpsDynamicPlatform);
		newDataSent = true;
	//}

	//if (_lastDisplayData.gpsTrackingMode != navData.gpsTrackingMode || _pageChanged) {
		_lastDisplayData.gpsTrackingMode = navData.gpsTrackingMode;
		myNex.setNumber(SETTINGS_GPS_MODE, (uint32_t)navData.gpsTrackingMode);
		newDataSent = true;
	//}

	//if (_lastDisplayData.gpsFixTargetDistance != navData.gpsFixTargetDistance || _pageChanged) {
		_lastDisplayData.gpsFixTargetDistance = navData.gpsFixTargetDistance;
		myNex.setNumber(SETTINGS_GPS_METERS, (uint32_t)navData.gpsFixTargetDistance);
		newDataSent = true;
	//}

	return newDataSent;
}


//=================================================================================================================================================
//=================================================================================================================================================
//	PAGE 6 --- COMPASS
//=================================================================================================================================================
//=================================================================================================================================================
bool NextionDisplay::refreshPage6(const NavData &navData){

	bool newDataSent = false;

	//-----------------------------
	//Bearing
	if (_lastDisplayData.relativeBearing != navData.relativeBearing || _pageChanged) {
		_lastDisplayData.relativeBearing = navData.relativeBearing;
		uint32_t val = (uint32_t)navData.relativeBearing;
		if(val == 0) val = 360;
		val += 13; 	//Add the offset ID of the images in the Nextion display IDE
		myNex.setNumber(COMPASS_DEGREES, val);
		newDataSent = true;
	}

	//-----------------------------
	//Speed
	if (_lastDisplayData.speed != navData.speed || _pageChanged) {
		_lastDisplayData.speed = navData.speed;
		char data[8];
		sprintf(data, "%.0f", _lastDisplayData.speed);
		myNex.setText(COMPASS_SPEED, data);
		newDataSent = true;
	}
	
	//-----------------------------
	//Distance from waypoint
	if (_lastDisplayData.distanceWaypoint != navData.distanceWaypoint || _pageChanged) {
		_lastDisplayData.distanceWaypoint = navData.distanceWaypoint;
		char data[9];
		sprintf(data, "%.1f", (_lastDisplayData.distanceWaypoint * 1000));
		myNex.setText(COMPASS_DISTANCE, data);
		newDataSent = true;
	}
	
	return newDataSent;
}


//=================================================================================================================================================
//=================================================================================================================================================
//	PAGE 7 --- MAP
//=================================================================================================================================================
//=================================================================================================================================================
bool NextionDisplay::refreshPage7(const NavData &navData){

	bool newDataSent = false;
	
	if (_lastDisplayData.mapData != navData.mapData || _pageChanged) {
		_lastDisplayData.mapData = navData.mapData;
		myNex.setNumber(MAP_PIN_X, (uint32_t)navData.mapData.x);
		myNex.setNumber(MAP_PIN_Y, (uint32_t)navData.mapData.y);
		myNex.setNumber(MAP_TILE_ID, (uint32_t)navData.mapData.tileId);
		newDataSent = true;
	}

	return newDataSent;
}


//=================================================================================================================================================
//=================================================================================================================================================
//	PAGE 8 --- HEALTH
//=================================================================================================================================================
//=================================================================================================================================================
bool NextionDisplay::refreshPage8(const NavData &navData){

	bool newDataSent = false;
	
	if (_lastDisplayData.heart.bpm != navData.heart.bpm || _pageChanged) {
		_lastDisplayData.heart.bpm = navData.heart.bpm;
		char data[4];
		sprintf(data, "%d", (_lastDisplayData.heart.bpm));
		myNex.setText(HEALTH_HEART_BPM, data);
		newDataSent = true;
	}

	if (_lastDisplayData.heart.bpmMax != navData.heart.bpmMax || _pageChanged) {
		_lastDisplayData.heart.bpmMax = navData.heart.bpmMax;
		char data[4];
		sprintf(data, "%d", (_lastDisplayData.heart.bpmMax));
		myNex.setText(HEALTH_HEART_MAX, data);
		newDataSent = true;
	}

	if (_lastDisplayData.heart.bpmMin != navData.heart.bpmMin || _pageChanged) {
		_lastDisplayData.heart.bpmMin = navData.heart.bpmMin;
		char data[4];
		sprintf(data, "%d", (_lastDisplayData.heart.bpmMin));
		myNex.setText(HEALTH_HEART_MIN, data);
		newDataSent = true;
	}

	return newDataSent;
}