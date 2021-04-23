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

#include "TemperatureDs18b20.h"

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensorDs18b20(&oneWire);

TemperatureSensor::TemperatureSensor(){
	_lastTempTime = 0;
	_validReadout = false;
	_temperature = 999;
	_currentState = notFound;
	cb_onTemperatureChange = NULL;
}

void TemperatureSensor::begin(uint8_t resolution){
	_currentState = notFound;
	sensorDs18b20.begin();
	delay(50);
	if(sensorDs18b20.getDeviceCount() > 0){
		//Avoid blocking the CPU waiting for the sensor reading
		sensorDs18b20.setWaitForConversion(false);
		sensorDs18b20.getAddress(_sensorAddress, 0);
		sensorDs18b20.setResolution(resolution);
		_currentState = requestReading;
	}
	
	#ifdef DEBUG_DS18B20
		uint8_t count = sensorDs18b20.getDeviceCount();
		Serial.print("DS18B20: number of sensors found: ");
		Serial.print(count);
		Serial.println("");
		if(count > 0){
			Serial.print("DS18B20: parasite power is "); 
			if (sensorDs18b20.isParasitePowerMode()) Serial.print("ON");
			else Serial.print("OFF");
		}
		Serial.println("");
	#endif
}

void TemperatureSensor::update(){
	switch(_currentState){
		case notFound:
			
		break;
		case requestReading:
			requestNewReading();
		break;
		case waitingReading:
			waitingForReading();
		break;
		case gettingReading:
			getReading();
		break;
	}
}

void TemperatureSensor::requestNewReading(){
	if(_lastTempTime != 0 && (millis() - _lastTempTime < TEMPERATURE_FREQ)) return;
	sensorDs18b20.requestTemperatures();
	_currentState = waitingReading;

	#ifdef DEBUG_DS18B20
		Serial.print("DS18B20: requested new reading");
		Serial.println("");
	#endif
}

//The sensor conversion timeout is 750ms
//I am waiting 1 second to be sure
//I am only using a timeout because sensorDs18b20.isConversionComplete() keeps reading the bus
void TemperatureSensor::waitingForReading(){
	if(sensorDs18b20.isConversionComplete())
		_currentState = gettingReading;
}

void TemperatureSensor::getReading(){
	readTemperature();
	_currentState = requestReading;
	_lastTempTime = millis();

	#ifdef DEBUG_DS18B20
		Serial.print("DS18B20: ");
		Serial.print(_temperature);
		Serial.print(" °C");
		Serial.println("");
	#endif
}

void TemperatureSensor::readTemperature(){
	float t = sensorDs18b20.getTempC(_sensorAddress);
	_validReadout = (t != 85.0 && t != (-127.0));

	if(_temperature != t){
		_temperature = t;
		if(cb_onTemperatureChange)(*cb_onTemperatureChange)(_temperature, _validReadout);
	}
}