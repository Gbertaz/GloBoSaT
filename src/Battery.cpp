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

#include "Battery.h"

uint16_t const refVoltage = 3300;		//Board reference voltage [mV]
float const dividerRatio = 1.47;		//Voltage divider resitors ratio: (R1 + R2) / R2

Battery::Battery(){
}

void Battery::begin(uint16_t minVoltage, uint16_t maxVoltage, uint8_t analogPin){
	pinMode(analogPin, INPUT);
	_minVoltage = minVoltage;
	_maxVoltage = maxVoltage;
	_analogPin = analogPin;
	_voltage = 999;
	_percentage = 999;
	_lastUpdate = 0;
	cb_onStatusChange = NULL;
}

void Battery::update(){
	if(_lastUpdate != 0 && (millis() - _lastUpdate < 30000)) return;
	_lastUpdate = millis();
	uint16_t v = readVoltage();
	uint16_t p = percentage(v);

	if(v != _voltage || p != _percentage){
		_voltage = v;
		_percentage = p;
		if(cb_onStatusChange)(*cb_onStatusChange)(_voltage, _percentage);
	}
	
	#ifdef DEBUG_BATTERY
		Serial.print("BATTERY: ");
		Serial.print(v);
		Serial.print(" mV - ");
		Serial.print(p);
		Serial.print(" %");
		Serial.println("");
	#endif
}

uint16_t Battery::readVoltage() {
	return analogRead(_analogPin) * dividerRatio * refVoltage / 1024;
}

uint8_t Battery::percentage(uint16_t voltage){
	if (voltage <= _minVoltage) return 0;
	else if (voltage >= _maxVoltage) return 100;
	else {
		uint8_t result = 105 - (105 / (1 + pow(1.724 * (voltage - _minVoltage)/(_maxVoltage - _minVoltage), 5.5)));
		return result >= 100 ? 100 : result;
	}
}