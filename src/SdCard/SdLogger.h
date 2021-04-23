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

#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include "../NavigationData.h"
#include "../Config.h"
#include <Arduino.h>
#include <SdFat.h>

class SdCardLogger {

public:

	SdCardLogger();
	bool initLogFile(uint8_t day, uint8_t month, uint16_t year);
	void update();
	void log(const NavData &navData);

private:

	SdFs _sdCard;
	FsFile _logFile;
	uint8_t _fixCounter;
	unsigned long _lastFlushMillis;

	bool isTimeToFlush();
	void flushSdBuffer();
	void stopLog();
	void printFixLostMark();
	void printL(Print &outs, int32_t degE7);
};

#endif