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

#ifndef CONFIG_H
#define CONFIG_H

#define MAP_TILES_CALIBRATION "Maps/cal_z15.gbts"   //Map configuration file
#define MAP_DATA_DELIMITER "|"
#define TILES_NUMBER 48
#define TILE_WIDTH 256
#define TILE_HEIGHT 256

#define BATTERY_MIN_VOLTAGE 3000        //Minimum battery voltage [mV]
#define BATTERY_MAX_VOLTAGE 4200        //Maximum battery voltage [mV]
#define BATTERY_ANALOG_PING 24          //Analog pin

#define DISPLAY_TARGET_FPS 5            //Display FPS

#define GPS_SPEED_THRESHOLD 0.8         //Threshold below which the GPS speed is ignored [km/h]
//#define GPS_SPEED_LOW_PASS_FILTER       //The Gps speed goes through a low pass filter

#define TEMPERATURE_FREQ 10000          //Temperature sensor readout freqency [ms]
#define SD_INIT_RETRY_INTERVAL 10000    //Delay between SD card initialisations attempts [ms]
#define LOG_ON_STARTUP true             //Start to log on SD card as soon as the GPS is available

#define ONE_WIRE_BUS 2                  //PIN temperature sensor DS18B20

//Header line of the log file
#define LOG_COLUMN_HEADER "latitude," "longitude," "satellites," "speed," "altitude," "heading," "temperature," "bpm," "year," "month," "day," "hours," "minutes," "seconds," "centiseconds"
#define LOG_DATA_DELIMITER ","

//Simulates the Gps by reading the track from the SD card
//#define SIMULATED
#ifdef SIMULATED
    #define TRACK_FILE_PATH "13012021/data_1.log"
#endif

//To disable all the serial prints just comment out DEBUG_INFO define
//#define DEBUG_INFO
#ifdef DEBUG_INFO
    //#define DEBUG_DISPLAY
    //#define DEBUG_DS18B20
    //#define DEBUG_GPS_FIX_SKIPPED
    //#define DEBUG_GPS_ELAPSED_MILLIS
    //#define DEBUG_GPS_SETTINGS
    //#define DEBUG_TRACK_READER
    //#define DEBUG_SD_LOGGER
    //#define DEBUG_MAP
    //#define DEBUG_BATTERY
#endif

#endif