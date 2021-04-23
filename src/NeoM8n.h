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

#ifndef NEO_M8N_H
#define NEO_M8N_H

#include <Arduino.h>
#include <GPSport.h>
#include <NMEAGPS.h>


class GpsNeoM8n {

public:

	enum ubxUpdateRate {
		ubxRate1Hz = 1,
		ubxRate2Hz = 2,
		ubxRate5Hz = 5,
		ubxRate10Hz = 10
	};

	enum ubxPlatform {
		ubxPortable = 1,
		ubxStationary,
		ubxPedestrian,
		ubxAutomotive,
		ubxSea,
		ubxAirborne1g,
		ubxAirborne2g,
		ubxAirborne4g,
		ubxWrist
	};

	enum trackingMode {
		modeAuto = 1,
		modeTime,
		modeDistance
	};

	enum fixTargetDistance{
		meters10 = 10,
		meters20 = 20,
		meters50 = 50,
		meters100 = 100
	};

	enum commandRequest{
		waitingRequest = 1,
		rate,
		platform,
		mode,
		meters
	};

	GpsNeoM8n();
	void begin(unsigned long baud, ubxUpdateRate ubxRate, ubxPlatform platform, trackingMode trackingMode);
	void requestUpdateRate(ubxUpdateRate updateRate);
	void requestDynamicPlatform(ubxPlatform platform);
	void requestTrackingMode(trackingMode trackingMode);
	void requestFixTargetDistance(fixTargetDistance targetDistance);
	void update();
	uint8_t getUpdateRate() { return _updateRate; }
	uint8_t getDynamicPlatform() { return _dynamicPlatform; }
	uint8_t getTrackingMode() { return _trackingMode; }
	uint8_t getFixTargetDistance() { return _targetDistance; }

	void onQuietTime(void (*callback)()) {
		cb_onQuietTime = callback;
	}
	void onValidFix(void (*callback)(const gps_fix &fix)){
		cb_onValidFix = callback;
	}
	void onTrackData(void (*callback)(void)){
		cb_onTrackData = callback;
	}

private:
	gps_fix _fix;
	gps_fix _previousTargetFix;
	uint8_t _updateRateRequest;
	uint8_t _dynamicPlatformRequest;
	uint8_t _trackingModeRequest;
	uint8_t _targetDistanceRequest;
	uint8_t _updateRate;
	uint8_t _dynamicPlatform;
	uint8_t _trackingMode;
	uint8_t _targetDistance;
	uint8_t _commandRequest;

	void checkIncomingCommands();
	bool isFixTargetDistanceReached(gps_fix &fix);
	void setUpdateRate();
	void setDynamicPlatform();
	void setTrackingMode();
	void setFixTargetDistance();
	uint8_t calculateOptimalUpdateRate(float speed);
	void sendUBXCommand(const unsigned char *bytes, size_t numbytes);
	void (*cb_onQuietTime)(void);
	void (*cb_onValidFix)(const gps_fix &fix);
	void (*cb_onTrackData)(void);
};

#endif