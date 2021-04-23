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

#ifndef EasyNextionLibrary_h
  #include "NextionDisplay.h"
#endif

#ifndef NEXTION_CALLBACK_H
  #include "NextionCallback.h"
#endif

NexDisplay::NexDisplay(HardwareSerial& serial){
  _port = &serial;
}

void NexDisplay::begin(unsigned long baud){
  _port->begin(baud);
  delay(50);
  _port->flush();
  _currentState = waiting;
  _ndx = 0;
  _len = 0;
}

void NexDisplay::setNumber(const char *objName, uint32_t val){
  _port->print(objName);
  _port->print("=");
  _port->print(val);
  _port->print("\xFF\xFF\xFF");
}

void NexDisplay::setText(const char *objName, const char *buffer){
  _port->print(objName);
  _port->print("=\"");
  _port->print(buffer);
  _port->print("\"");
  _port->print("\xFF\xFF\xFF");
}

void NexDisplay::sendCommand(const char *command){
  _port->print(command);
  _port->print("\xFF\xFF\xFF");
}

void NexDisplay::listen(){

  if(_currentState == readEnd){
    _currentState = waiting;
    parseCommand();
  }
  
  while(_port->available() > 0){

    char c = _port->read();

    if(_currentState == readInProgress){
      _receivedChars[_ndx] = c;
      _ndx++;
      if(_ndx == _len) _currentState = readEnd;
    }
    else if(_currentState == startReading){
      _len = c;
      _currentState = readInProgress;
    }
    else if(c == '#'){
      _ndx = 0;
      _len = 0;
      _currentState = startReading;
    }
  }
}

void NexDisplay::parseCommand(){
  switch(_receivedChars[0]){
    case 'P': //Page
      lastPageId = currentPageId;
      currentPageId = _receivedChars[1];
      onPageChange(_receivedChars[1]);
      break;
    case 'R': //Rate (Gps update rate)
      onGpsRateSet(_receivedChars[1]);
      break;
    case 'D': //Dynamic (Gps dynamic platform)
      onGpsPlatformSet(_receivedChars[1]);
      break;
    case 'T': //Tracking mode (Auto, Time, Distance)
      onGpsTrackingModeSet(_receivedChars[1]);
      break;
    case 'M': //Meters (Distance target between each log record)
      onGpsTargetDistanceSet(_receivedChars[1]);
      break;
  }
}