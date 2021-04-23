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

#include "SdLogger.h"

char _directoryName[] = "00000000";      //Current directory name on the SD card

SdCardLogger::SdCardLogger(){
	_fixCounter = 0;
	_lastFlushMillis = 0;
}

void SdCardLogger::update(){
	//Time to flush?
	if(isTimeToFlush()) flushSdBuffer();
}

void SdCardLogger::log(const NavData &navData){

	printL( _logFile, navData.latitudeH ); //Prints int like a float
	_logFile.print( ',' );
	printL( _logFile, navData.longitudeH ); //Prints int like a float
	_logFile.print( ',' );
	_logFile.print( navData.satellites );
	_logFile.print( ',' );
	_logFile.print( navData.speed, 1 );
	_logFile.print( ',' );
	_logFile.print( navData.altitude, 1);
	_logFile.print( ',' );
	_logFile.print( navData.heading, 1);
	_logFile.print( ',' );
	_logFile.print( navData.temperature.current, 1);
	_logFile.print( ',' );
	_logFile.print( navData.heart.bpm );
	_logFile.print( ',' );
	_logFile.print( navData.date.year );
	_logFile.print( ',' );
	if (navData.date.month < 10) _logFile.print( '0' );
	_logFile.print( navData.date.month );
	_logFile.print( ',' );
	if (navData.date.day < 10) _logFile.print( '0' );
	_logFile.print( navData.date.day );
	_logFile.print( ',' );
	if (navData.timeUTC.hours < 10) _logFile.print( '0' );
	_logFile.print(navData.timeUTC.hours );
	_logFile.print( ',' );
	if (navData.timeUTC.minutes < 10) _logFile.print( '0' );
	_logFile.print( navData.timeUTC.minutes );
	_logFile.print( ',' );
	if (navData.timeUTC.seconds < 10) _logFile.print( '0' );
	_logFile.print( navData.timeUTC.seconds );
	_logFile.print( ',' );
	if (navData.timeUTC.centiseconds < 10) _logFile.print( '0' );
	_logFile.print( navData.timeUTC.centiseconds );
	_logFile.println();

	_fixCounter++;
}

bool SdCardLogger::initLogFile(uint8_t day, uint8_t month, uint16_t year){

	//Init SD card
	//if(_sdCard.begin(SdioConfig(DMA_SDIO)) == false) {	//Flush takes 6-7ms
	if(_sdCard.begin(SdioConfig(FIFO_SDIO)) == false) {		//Flush takes 2-3ms!!

		#ifdef DEBUG_SD_LOGGER
			Serial.print("SD LOGGER: init SD card failed!");
			Serial.println("");
		#endif

		return false;
	}

	//Name of the directory
  	sprintf(_directoryName, "%02d%02d%4d", day, month, year);

	if(_sdCard.exists(_directoryName) == false){  
		if(_sdCard.mkdir(_directoryName) == false) {

			#ifdef DEBUG_SD_LOGGER
				Serial.print("SD LOGGER: mkdir ");
				Serial.print(_directoryName);
				Serial.print("failed!");
				Serial.println("");
			#endif

			return false;
		}
	}

	//Directory name is always the current date in the format: ddmmyyyy
	//File name is in the format: "data_xxxxx.log" where "xxxxx" is a progressive integer from 0-10000
	//a folder can contain at most 10000 log files
	char fullpath[25];
	int fileIndex = 0;
	do{
		if(fileIndex > 10000) return false;
		sprintf(fullpath, "%s/%s%d.log", _directoryName, "data_", fileIndex);
		fileIndex++;
	}while(_sdCard.exists(fullpath));
	
	//Write the header only if the file doesn't exist
	bool printHeader = (!_sdCard.exists(fullpath));

	//O_CREAT | O_WRITE | O_APPEND
	//O_RDWR | O_CREAT | O_TRUNC
	if(_logFile.open(fullpath, O_CREAT | O_WRITE) == false){

		#ifdef DEBUG_SD_LOGGER
			Serial.print("SD LOGGER: failed to open file ");
			Serial.print(fullpath);
			Serial.println("");
		#endif

		return false;
	}

	if(printHeader) {
		_logFile.println( F(LOG_COLUMN_HEADER) );
		flushSdBuffer();
	}

	_fixCounter = 0;
	_lastFlushMillis = 0;

	return true;
}

//=================================
//If the tracking mode is based on distance it might take
//even hours to reach 6 fixes so to avoid losing data
//the buffer is flushed either when it is almost full
//(6 fixes ~480 bytes) or the elasped time since last flush
//exceedes X seconds AND _fixCounter is not zero.
bool SdCardLogger::isTimeToFlush(){
	return (_fixCounter >= 6 || 
	(_fixCounter > 0 && millis() - _lastFlushMillis >= 20000));
}

void SdCardLogger::flushSdBuffer(){
	_logFile.flush();
	_lastFlushMillis = millis();
	_fixCounter = 0;
}

void SdCardLogger::stopLog(){
  flushSdBuffer();
  _logFile.close();
}

void SdCardLogger::printFixLostMark(){
  _logFile.println("****FIX_LOST****");
  flushSdBuffer();
}


void SdCardLogger::printL(Print &outs, int32_t degE7)
{
	//Extract and print negative sign
	if (degE7 < 0) {
		degE7 = -degE7;
		outs.print( '-' );
	}

	//Whole degrees
	int32_t deg = degE7 / 10000000L;
	outs.print( deg );
	outs.print( '.' );

	//Get fractional degrees
	degE7 -= deg*10000000L;

	//Print leading zeroes, if needed
	int32_t factor = 1000000L;
	while ((degE7 < factor) && (factor > 1L)){
		outs.print('0');
		factor /= 10L;
	}

	//Print fractional degrees
	outs.print( degE7 );
}