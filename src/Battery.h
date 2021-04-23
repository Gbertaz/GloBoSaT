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

#ifndef BATTERY_H
#define BATTERY_H

#include <Arduino.h>

class Battery {

public:

	Battery();
	void begin(uint16_t minVoltage, uint16_t maxVoltage, uint8_t analogPin);
	void update();
	uint16_t readVoltage();
	uint8_t percentage(uint16_t voltage);
	void onStatusChange(void (*callback)(uint16_t voltage, uint16_t percentage)){
		cb_onStatusChange = callback;
	}

private:
	uint8_t _analogPin;
	uint16_t _minVoltage;
	uint16_t _maxVoltage;
	uint16_t _voltage;
	uint16_t _percentage;

	unsigned long _lastUpdate;
	void (*cb_onStatusChange)(uint16_t voltage, uint16_t percentage);
};

#endif