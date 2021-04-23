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

#include "SdTrackReader.h"


TrackReader::TrackReader(){

}

bool TrackReader::init(char *filepath){

	cb_onQuietTime = NULL;

	if(_sdCard.begin(SdioConfig(FIFO_SDIO)) == false) {
		#ifdef DEBUG_TRACK_READER
			Serial.print("SD TRACK READER: SD card begin failed");
			Serial.println("");
		#endif

		return false;
	}

	if(_sdCard.exists(filepath) == false) {

		#ifdef DEBUG_TRACK_READER
			Serial.print("SD TRACK READER: file ");
			Serial.print(filepath);
			Serial.println(" doesn't exist.");
			Serial.println("");
		#endif

		return false;
	}

	if(_logFile.open(filepath, O_READ) == false) {

		#ifdef DEBUG_TRACK_READER
			Serial.print("SD TRACK READER: failed to open file ");
			Serial.print(filepath);
			Serial.println("");
		#endif

		return false;
	}

	_endOfFile = false;
	_isFileHeader = true;
	_lastFixMillis = 0;

	//Reads the first fix
	readLogLine(_currentFix);

	//Check if we reached the end of file
	if(isEndOfFile()) {
		#ifdef DEBUG_TRACK_READER
			Serial.print("SD TRACK READER: log file doesn't contain any fix ");
			Serial.print(filepath);
			Serial.println("");
		#endif

		return false;
	}

	//Reads the second fix
	readLogLine(_nextFix);

	//We need at least two fixes
	if(isEndOfFile()) {
		#ifdef DEBUG_TRACK_READER
			Serial.print("SD TRACK READER: log file contains only one fix ");
			Serial.print(filepath);
			Serial.println("");
		#endif
	}

	#ifdef DEBUG_TRACK_READER
		Serial.print("SD TRACK READER: starting reproducing track ");
		Serial.print(filepath);
		Serial.println("");
	#endif

	_fixInterval = calculateElapsedMillis(_nextFix, _currentFix);
	_currentState = dataFeeding;
	
	return true;
}


void TrackReader::readLogLine(NavData &fix){
	
	char textBuffer[255];
	char delimiter[] = "\n";

	//Reads one line
	int len = _logFile.fgets(textBuffer, sizeof(textBuffer), delimiter);

	if(len > 0){
		
		//Skips the file header
		if(_isFileHeader){
			_isFileHeader = false;
			readLogLine(fix);
			return;
		}

		parseLine(textBuffer, fix);
	}
	else if(len == 0){
		_endOfFile = true;
		_logFile.close();
	}
	else {
		_currentState = error;
	}
}


void TrackReader::parseLine(char *buffer, NavData &fix){
	char *p = buffer;
	char *str;
	uint8_t valueIndex = 0;

	while ((str = strtok_r(p, LOG_DATA_DELIMITER, &p)) != NULL){
		switch(valueIndex){
			case 0:
				fix.latitude = atof(str);
			break;
			case 1:
				fix.longitude = atof(str);
			break;
			case 2:
				fix.satellites = atoi(str);
			break;
			case 3:
				fix.speed = atof(str);
			break;
			case 4:
				fix.altitude = atof(str);
			break;
			case 5:
				fix.heading = atof(str);
			break;
			case 6:
				fix.temperature.current = atof(str);
				fix.temperature.currentValid = (fix.temperature.current != 85.0 && fix.temperature.current != -127.0);
			break;
			case 7:
				fix.date.year = atoi(str);
			break;
			case 8:
				fix.date.month = atoi(str);
			break;
			case 9:
				fix.date.day = atoi(str);
			break;
			case 10:
				fix.timeUTC.hours = atoi(str);
			break;
			case 11:
				fix.timeUTC.minutes = atoi(str);
			break;
			case 12:
				fix.timeUTC.seconds = atoi(str);
			break;
			case 13:
				fix.timeUTC.centiseconds = atoi(str);
			break;
		}

		valueIndex++;
	}
}


void TrackReader::waitNextFix(){
	if(millis() - _lastFixMillis < _fixInterval) return;
	_currentState = dataFeeding;
}

void TrackReader::feedDataFix(NavData &navData){

	_lastFixMillis = millis();

	//Updating NavData object with the last fix read from the log file
	navData = _currentFix;

	//Invoke the callback to notify that new data is available
	if(cb_onQuietTime)(*cb_onQuietTime)();

	//Swap fixes
	_currentFix = _nextFix;

	//Read the next one
	readLogLine(_nextFix);

	if(isEndOfFile()) return;

	//Calculate the next interval
	_fixInterval = calculateElapsedMillis(_nextFix, _currentFix);

	//And delay the correct amount of millis before providing the next fix
	_currentState = waitingNextFix;
}

void TrackReader::endOfTracking(){
	#ifdef DEBUG_TRACK_READER
		Serial.println("SD TRACK READER: END of Track");
	#endif

	_currentState = idling;
}

void TrackReader::errorState(){
	#ifdef DEBUG_TRACK_READER
		Serial.println("SD TRACK READER: an error occurred reading the log file");
	#endif

	_currentState = idling;
}

void TrackReader::update(NavData &navData){

	switch(_currentState){
		case idling:
		
		break;
		case dataFeeding:
			feedDataFix(navData);
		break;
		case waitingNextFix:
			waitNextFix();
		break;
		case endTrack:
			endOfTracking();
		break;
		case error:
			errorState();
		break;
	}
}

bool TrackReader::isEndOfFile(){
	if(_endOfFile) _currentState = endTrack;
	return _endOfFile;
}


int TrackReader::calculateElapsedMillis(NavData &lastFix, NavData &previousFix)
{
	/*struct std::tm lastFixTime  { .tm_sec = lastFix.timeUTC.seconds, .tm_min = lastFix.timeUTC.minutes, .tm_hour = lastFix.timeUTC.hours,
		.tm_mday = lastFix.date.day, .tm_mon = lastFix.date.month, .tm_year = lastFix.date.year };

	struct std::tm prevFixTime  { .tm_sec = previousFix.timeUTC.seconds, .tm_min = previousFix.timeUTC.minutes, .tm_hour = previousFix.timeUTC.hours,
		.tm_mday = previousFix.date.day, .tm_mon = previousFix.date.month, .tm_year = previousFix.date.year };

 	auto last = (std::chrono::system_clock::from_time_t(std::mktime(&lastFixTime)))
       + std::chrono::milliseconds(lastFix.timeUTC.centiseconds * 10);

	auto prev = (std::chrono::system_clock::from_time_t(std::mktime(&prevFixTime)))
		+ std::chrono::milliseconds(previousFix.timeUTC.centiseconds * 10);

	return (int)std::chrono::duration_cast<std::chrono::milliseconds>(last - prev).count();
	*/
	return 200;
}



