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

#ifndef NavigationData
#define NavigationData

#include <NMEAGPS.h>
#include "Config.h"

class NavData {

public:

	NavData();

	struct time_g {
		volatile uint8_t hours;
		volatile uint8_t minutes;
		volatile uint8_t seconds;
		volatile uint16_t centiseconds;

		bool operator==(const time_g &rhs) const
		{
			return hours == rhs.hours && minutes == rhs.minutes && seconds == rhs.seconds;
		}

		bool operator!=(const time_g &rhs) const
		{
			return !(*this == rhs);
		}
	};

	struct date_g {
		volatile uint8_t day;
		volatile uint8_t month;
		volatile uint16_t year;

		bool operator==(const date_g &rhs) const
		{
			return day == rhs.day && month == rhs.month && year == rhs.year;
		}

		bool operator!=(const date_g &rhs) const
		{
			return !(*this == rhs);
		}
	};

	struct coordinateDMS_g {
		volatile uint8_t degrees;
		volatile uint8_t minutes;
		volatile float	seconds;
		volatile char NS;
		volatile char EW;

		bool operator==(const coordinateDMS_g &rhs) const
		{
			return degrees == rhs.degrees && minutes == rhs.minutes && seconds == rhs.seconds;
		}

		bool operator!=(const coordinateDMS_g &rhs) const
		{
			return !(*this == rhs);
		}
	};

	struct temperature_g {
		volatile float current;
		volatile float max;
		volatile float min;
		volatile bool currentValid;
	};

	struct mapData {
		volatile int x;
		volatile int y;
		volatile uint16_t tileId;

		bool operator==(const mapData &rhs) const
		{
			return x == rhs.x && y == rhs.y && tileId == rhs.tileId;
		}

		bool operator!=(const mapData &rhs) const
		{
			return !(*this == rhs);
		}
	};

	struct battery {
		volatile uint8_t percentage;
		volatile uint16_t voltage;
	};

	struct heartData {
		volatile uint8_t bpm;
		volatile uint8_t bpmMax;
		volatile uint8_t bpmMin;
		volatile int interval;
		volatile int sequenceNumber;
	};

	enum movingState {
		idling = 0,
		moving,
		stopped
	};
	
	heartData heart;
	battery batteryStatus;
	mapData mapData;
	volatile uint8_t satellites;
	volatile int32_t latitudeH;					//High precision latitude
	volatile int32_t longitudeH;				//High precision longitude
	volatile float latitude;					//Low precision latitude used to calculate sunrise and sunset time
	volatile float longitude;					//Low precision longitude used to calculate sunrise and sunset time
	coordinateDMS_g latitudeDMS;
	coordinateDMS_g longitudeDMS;
	date_g date;
	time_g timeUTC;
	time_g totalTime;
	time_g movingTime;
	time_g totalStoppedTime;
	time_g lastStoppedTime;
	temperature_g temperature;
	volatile float speed;
	volatile float speedMax;
	volatile float speedMovingAvg;
	volatile float speedTotalAvg;
	volatile float altitude;
	volatile float altitudeMax;
	volatile float altitudeMin;
	volatile float heading;
	volatile float trueBearing;
	volatile uint16_t relativeBearing;
	volatile float distanceTotalKm;
	volatile float distanceFromHome;
	volatile float distanceWaypoint;
	volatile float paceMinKm;
	volatile unsigned long missedFixCount;
	char sunrise[6] = "__:__";
	char sunset[6] = "__:__";

	volatile uint8_t gpsUpdateRate;
	volatile uint8_t gpsDynamicPlatform;
	volatile uint8_t gpsTrackingMode;
	volatile uint8_t gpsFixTargetDistance;

	void init();
	void update(const gps_fix &fix);
	bool hasValid2DFix();
	bool hasValid3DFix();
	bool hasValid4DFix();
	void resetMissedFixCount() { missedFixCount = 0; }

	void setTemperature(float t, bool valid){
		temperature.current = t;
		temperature.currentValid = valid;
		if(valid){
			if(t > temperature.max) temperature.max = t;
			if(t < temperature.min) temperature.min = t;
		}
	}

	void setHeartData(int bpm, int interval, int sequenceNumber){
		heart.bpm = bpm;
		heart.interval = interval;
		heart.sequenceNumber = sequenceNumber;
		if(bpm > heart.bpmMax) heart.bpmMax = bpm;
		if(bpm < heart.bpmMin) heart.bpmMin = bpm;
	}

private:

	gps_fix _firstFix;
	gps_fix _lastFix;
	gps_fix _previousFix;

	movingState _movingStatus;

	unsigned long _lastUpdateMillis;
	unsigned long _elapsedMillis;
	unsigned long _movingTimeMillis;
	unsigned long _totalStoppedMillis;
	unsigned long _lastStoppedMillis;
	
	date_g _lastSunriseUpdate;
	date_g _lastSunsetUpdate;
	
	int calculateElapsedMillis(gps_fix &lastFix, gps_fix &previousFix);
	int calculateMissedFixes(unsigned long elapsedMillis, int gpsRate);
	float calculateAverageSpeed(float distanceKm, NeoGPS::time_t neoGpsTime);
	float calculateAverageSpeed(float distanceKm, unsigned long seconds);
	uint16_t trueBearingToRelative(float trueB, float currentHeading);
	int convertNeoTimeToSeconds(NeoGPS::time_t neoGpsTime);
	float lowPassFilter(float threshold, float value);
	bool isGpsFixValid(const gps_fix &fix);
	bool isMoving();
	void loopIdling();
	void loopMoving();
	void loopStopped();
	bool updateSunrise(){return /*_lastSunriseUpdate != date;*/ false;}
	bool updateSunset(){return /*_lastSunsetUpdate != date;*/ false;}
};

#endif
