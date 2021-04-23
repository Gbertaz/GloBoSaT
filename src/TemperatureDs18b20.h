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

#ifndef TemperatureDs18b20
#define TemperatureDs18b20

#include "Config.h"
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>


class TemperatureSensor {

public:

	TemperatureSensor();

	//Accepted resolution values are: 9 - 10 - 11 - 12
	void begin(uint8_t resolution = 12);
	void update();
	void onTemperatureChange(void(*callback)(float temperature, bool valid)) {
		cb_onTemperatureChange = callback;
	}

private:

	enum sensorState {
		notFound = 0,
		requestReading,
		waitingReading,
		gettingReading
	};

	sensorState _currentState;
	unsigned long _lastTempTime;		//Time at last temperature sensor readout
	float _temperature;
	bool _validReadout;					//Is the last temperature readout valid?
	DeviceAddress _sensorAddress;

	void requestNewReading();
	void waitingForReading();
	void getReading();
	void readTemperature();
	void(*cb_onTemperatureChange)(float temperature, bool valid);
};

#endif