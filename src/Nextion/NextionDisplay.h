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

#ifndef NEXTIONDISPLAY_H
#define NEXTIONDISPLAY_H

#include <Arduino.h>

class NexDisplay {

	public:
    NexDisplay(HardwareSerial& serial);
		void begin(unsigned long baud = 115200);
    void setNumber(const char *objName, uint32_t val);
    void setText(const char *objName, const char *buffer);
    void sendCommand(const char *command);
    void listen();

    uint8_t currentPageId;
    uint8_t lastPageId;
    
	private:

    enum state {
      waiting = 1,
      startReading,
      readInProgress,
      readEnd
	  };
    
    byte _ndx = 0;
    uint8_t _len;
    state _currentState;
    char _receivedChars[8];
    HardwareSerial* _port;
		void parseCommand();
};

#endif

