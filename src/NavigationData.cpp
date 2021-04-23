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

#include "NavigationData.h"

static const NeoGPS::Location_t Home(41.725556, -49.946944);
static const NeoGPS::Location_t Waypoint(-13.163027, -72.544760);

//Waypoints array
//NeoGPS::Location_t waypoints[];

NavData::NavData() {
}

void NavData::init(){
	satellites = 0;
	latitudeH = 0;
	longitudeH = 0;
	latitude = 0;
	longitude = 0;
	latitudeDMS.degrees = 0;
	latitudeDMS.minutes = 0;
	latitudeDMS.seconds = 0;
	longitudeDMS.degrees = 0;
	longitudeDMS.minutes = 0;
	longitudeDMS.seconds = 0;
	speed = 0;
	speedMax = 0;
	speedMovingAvg = 0;
	speedTotalAvg = 0;
	altitude = 0;
	altitudeMax = 0;
	altitudeMin = 0;
	heading = 0;
	trueBearing = 0;
	relativeBearing = 0;
	date.day = 0;
	date.month = 0;
	date.year = 0;
	_lastSunriseUpdate.day = 0;
	_lastSunriseUpdate.month = 0;
	_lastSunriseUpdate.year = 0;
	_lastSunsetUpdate.day = 0;
	_lastSunsetUpdate.month = 0;
	_lastSunsetUpdate.year = 0;
	timeUTC.hours = 0;
	timeUTC.minutes = 0;
	timeUTC.seconds = 0;
	timeUTC.centiseconds = 0;
	totalTime.hours = 0;
	totalTime.minutes = 0;
	totalTime.seconds = 0;
	movingTime.hours = 0;
	movingTime.minutes = 0;
	movingTime.seconds = 0;
	totalStoppedTime.hours = 0;
	totalStoppedTime.minutes = 0;
	totalStoppedTime.seconds = 0;
	lastStoppedTime.hours = 0;
	lastStoppedTime.minutes = 0;
	lastStoppedTime.seconds = 0;
	distanceTotalKm = 0;
	distanceFromHome = 0;
	distanceWaypoint = 0;
	temperature.current = 0;
	temperature.currentValid = false;
	temperature.max = 0;
	temperature.min = 0;
	heart.bpm = 0;
	heart.interval = 0;
	heart.sequenceNumber = 0;
	heart.bpmMax = 0;
	heart.bpmMin = 255;
	paceMinKm = 0;
	missedFixCount = 0;
	_elapsedMillis = 0;
	_movingTimeMillis = 0;
	_totalStoppedMillis = 0;
	_lastStoppedMillis = 0;
	batteryStatus.percentage = 0;
	batteryStatus.voltage = 0;
	_movingStatus = idling;
	_lastUpdateMillis = 0;
}

//==============================================================================================
//									PUBLIC
//==============================================================================================

void NavData::update(const gps_fix &fix){

	//Calculate the elapsed millisecond
	_elapsedMillis = millis() - _lastUpdateMillis;
	_lastUpdateMillis = millis();

	//Check if it is the very first fix
	if(_firstFix.location.lat() == 0 && _firstFix.location.lon() == 0){
		_firstFix = fix;
		_previousFix = fix;
	}
	else {
		_previousFix = _lastFix;
	}
	
	_lastFix = fix;

	//Calculates the number of Gps fixes skipped (if any) between the current fix and the previous one
	missedFixCount += calculateMissedFixes(_elapsedMillis, gpsUpdateRate);

	if (_lastFix.valid.satellites) satellites = _lastFix.satellites;
	
	if (_lastFix.valid.speed) {
		float rawSpeed = _lastFix.speed_kph();

		#ifdef GPS_SPEED_LOW_PASS_FILTER
			speed = lowPassFilter(GPS_SPEED_THRESHOLD, rawSpeed);
		#else
			speed = rawSpeed;
		#endif

		if (speed > speedMax) speedMax = speed;
		paceMinKm = (speed > 0) ? (60 / speed) : 0;
	}

	if (_lastFix.valid.altitude) {
		altitude = _lastFix.altitude();
		if (altitude > altitudeMax || altitudeMax == 0) altitudeMax = altitude;
		if (altitude < altitudeMin || altitudeMin == 0) altitudeMin = altitude;
	}

	if (_lastFix.valid.location) {

		//True bearing:
		//Clockwise degrees from North
		trueBearing = _lastFix.location.BearingToDegrees(Waypoint);

		// for the higher-precision integer degrees, scaled by 10,000,000 (10 significant digits)
		latitudeH = _lastFix.latitudeL();
		longitudeH = _lastFix.longitudeL();

		// lower-precision floating-point degrees (~7 significant digits)
		latitude = _lastFix.latitude();
		longitude = _lastFix.longitude();

		// high precision coordinates ind DMS format
		latitudeDMS.degrees = _lastFix.latitudeDMS.degrees;
		latitudeDMS.minutes = _lastFix.latitudeDMS.minutes;
		latitudeDMS.seconds = _lastFix.latitudeDMS.secondsF();
		longitudeDMS.degrees = _lastFix.longitudeDMS.degrees;
		longitudeDMS.minutes = _lastFix.longitudeDMS.minutes;
		longitudeDMS.seconds = _lastFix.longitudeDMS.secondsF();
		latitudeDMS.NS = _lastFix.latitudeDMS.NS();
		longitudeDMS.EW = _lastFix.longitudeDMS.EW();
	}

	if (_lastFix.valid.heading) {
		heading = _lastFix.heading();

		//Relative bearing:
		//Clockwise degrees between the user's current heading
		//and line of sight of the waypoint from the user
		relativeBearing = trueBearingToRelative(trueBearing, heading);
	}
	
	if(_lastFix.valid.date && _lastFix.valid.time){
		date.day = _lastFix.dateTime.date;
		date.month = _lastFix.dateTime.month;
		date.year = _lastFix.dateTime.full_year();

		timeUTC.hours = _lastFix.dateTime.hours;
		timeUTC.minutes = _lastFix.dateTime.minutes;
		timeUTC.seconds = _lastFix.dateTime.seconds;
		timeUTC.centiseconds = _lastFix.dateTime_cs;

		//Elapsed time since first valid GPS Fix
		NeoGPS::time_t totTime = (clock_t)_lastFix.dateTime - (clock_t)_firstFix.dateTime;

		//Total average speed [Km/h]
		speedTotalAvg = calculateAverageSpeed(distanceTotalKm, totTime);

		totalTime.hours = totTime.hours;
		totalTime.minutes = totTime.minutes;
		totalTime.seconds = totTime.seconds;
	}
	
	//Distance to home and waypoint
	distanceFromHome = _lastFix.location.DistanceKm(Home);
	distanceWaypoint = _lastFix.location.DistanceKm(Waypoint);

	//Sunrise and sunset
	if (updateSunrise() || updateSunset()) {
		if(_lastFix.valid.location && _lastFix.valid.date){
			//TODO
		}
	}

	switch(_movingStatus){
		case idling:
			loopIdling();
		break;
		case moving:
			loopMoving();
		break;
		case stopped:
			loopStopped();
		break;
	}

	#ifdef DEBUG_GPS_ELAPSED_MILLIS
		Serial.print("GPS: elapsed ");
		Serial.print(_elapsedMillis);
		Serial.print(" ms");
		Serial.println("");
	#endif

	/*
	#ifdef DEBUG_GPS_FIX_SKIPPED
		Serial.print("GPS: missed fix count ");
		Serial.print(missedFixCount);
		Serial.println("");
	#endif
	*/
}


//Location only
bool NavData::hasValid2DFix() {
	return (_lastFix.valid.location);
}

//Location and altitude
bool NavData::hasValid3DFix() {
	return (_lastFix.valid.location && _lastFix.valid.altitude);
}

//Location and time
bool NavData::hasValid4DFix() {
	return (_lastFix.valid.location && _lastFix.valid.date && _lastFix.valid.time);
}


//==============================================================================================
//									PRIVATE
//==============================================================================================

bool NavData::isMoving(){
	return speed >= GPS_SPEED_THRESHOLD;
}

void NavData::loopIdling(){
	isMoving() ? _movingStatus = moving : _movingStatus = stopped;
}

void NavData::loopMoving(){

	//Odometer calculated as the sum of the distance between each fix and the previous one
	distanceTotalKm += _lastFix.location.DistanceKm(_previousFix.location);

	if(_elapsedMillis > 0) {

		//Moving time
		_movingTimeMillis += _elapsedMillis;
		movingTime.hours = (int)((_movingTimeMillis / 3600000) % 24);
		movingTime.minutes = (int)((_movingTimeMillis / 60000) % 60);
		movingTime.seconds = (int)(_movingTimeMillis / 1000) % 60;

		//Moving Average speed [Km/h]
		speedMovingAvg = calculateAverageSpeed(distanceTotalKm, _movingTimeMillis / 1000);
	}

	//If not moving resets the last stopped time and changes state
	if(!isMoving()) {
		_lastStoppedMillis = 0;
		lastStoppedTime.hours = 0;
		lastStoppedTime.minutes = 0;
		lastStoppedTime.seconds = 0;
		_movingStatus = stopped;
	}
}

void NavData::loopStopped(){

	if(_elapsedMillis > 0){
		_totalStoppedMillis += _elapsedMillis;
		_lastStoppedMillis += _elapsedMillis;

		if(_totalStoppedMillis > 0)
		{
			totalStoppedTime.hours = (int)((_totalStoppedMillis / 3600000) % 24);
			totalStoppedTime.minutes = (int)((_totalStoppedMillis / 60000) % 60);
			totalStoppedTime.seconds = (int)(_totalStoppedMillis / 1000) % 60;
		}

		if(_lastStoppedMillis > 0){
			lastStoppedTime.hours = (int)((_lastStoppedMillis / 3600000) % 24);
			lastStoppedTime.minutes = (int)((_lastStoppedMillis / 60000) % 60);
			lastStoppedTime.seconds = (int)(_lastStoppedMillis / 1000) % 60;
		}
	}

	if(isMoving()) _movingStatus = moving;
}

float NavData::calculateAverageSpeed(float distanceKm, NeoGPS::time_t neoGpsTime) {
	float avgSpeed = 0;
	if (distanceKm <= 0) return avgSpeed;
	int seconds = convertNeoTimeToSeconds(neoGpsTime);
	if (seconds <= 0) return avgSpeed;
	avgSpeed = distanceKm * 3600 / seconds;
	return avgSpeed;
}

float NavData::calculateAverageSpeed(float distanceKm, unsigned long seconds) {
	float avgSpeed = 0;
	if (distanceKm <= 0) return avgSpeed;
	if (seconds <= 0) return avgSpeed;
	avgSpeed = distanceKm * 3600 / seconds;
	return avgSpeed;
}

int NavData::convertNeoTimeToSeconds(NeoGPS::time_t neoGpsTime) {
	int seconds = 0;
	if (neoGpsTime.hours > 0) seconds += neoGpsTime.hours * 3600;
	if (neoGpsTime.minutes > 0) seconds += neoGpsTime.minutes * 60;
	if (neoGpsTime.seconds > 0) seconds += neoGpsTime.seconds;
	return seconds;
}

//If the elapsed milliseconds don't match the expected elapsed milliseconds
//between each Gps fix (calculated as 1000 / Gps update rate) means we either have
//missed reading a fix from the UART interface or the Gps didn't return the fix
int NavData::calculateMissedFixes(unsigned long elapsedMillis, int gpsRate){
	if(elapsedMillis == 0) return 0;
	int nFixLost = (elapsedMillis / (1000 / gpsRate)) - 1;
	return (nFixLost > 0) ? nFixLost : 0;
}

float NavData::lowPassFilter(float threshold, float value) {
	if(value < threshold) value = 0;
	float avg = 0;
	avg = (avg * 3) / 4 + value;
	value = avg / 4;
	return value;
}

uint16_t NavData::trueBearingToRelative(float trueB, float currentHeading){
	float relative = trueB - currentHeading;
	return (relative > 0) ? (uint16_t)relative : (uint16_t)((360 - currentHeading) + trueB);
}