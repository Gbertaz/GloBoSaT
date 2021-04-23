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

#ifndef SD_CARD_READER_H
#define SD_CARD_READER_H

#include <Arduino.h>
#include <SdFat.h>
#include <NMEAGPS.h>
#include "../Config.h"
#include "../NavigationData.h"

class TrackReader {

public:

	TrackReader();
	bool init(char *filepath);
	void update(NavData &navData);
	void onGpsQuietTime(void (*callback)(void)){
		cb_onQuietTime = callback;
	}

private:

	NavData _currentFix;
	NavData _nextFix;
	
	unsigned long _lastFixMillis;
	unsigned long _fixInterval;
	bool _isFileHeader;
	bool _endOfFile;

	enum state{
		idling,
		dataFeeding,
		waitingNextFix,
		endTrack,
		error
	};

	state _currentState;

	SdFs _sdCard;
	SdFile _logFile;
	void readLogLine(NavData &fix);
	void parseLine(char *buffer, NavData &fix);
	int calculateElapsedMillis(NavData &lastFix, NavData &previousFix);
	void waitNextFix();
	void feedDataFix(NavData &navData);
	void endOfTracking();
	void errorState();
	bool isEndOfFile();
	void (*cb_onQuietTime)(void);
};

#endif