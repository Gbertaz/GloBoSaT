# GloBoSaT
First prototype of a General Purpose Satellite Tracker

The aim of the project is to build an high performance general purpose GPS tracker and logger that can be brought everywhere to log some amazing trips.

Based on NeoGPS library: https://github.com/SlashDevin/NeoGPS

![Tracking mode Time](https://github.com/Gbertaz/GloBoSaT/blob/master/images/front_back.jpg)



# Current features

![Tracking mode Time](https://github.com/Gbertaz/GloBoSaT/blob/master/images/main.jpg)
![Tracking mode Time](https://github.com/Gbertaz/GloBoSaT/blob/master/images/time.jpg)
![Tracking mode Time](https://github.com/Gbertaz/GloBoSaT/blob/master/images/location.jpg)

The following data is displayed on the screen, values in **bold** are calculated, the others are provided directly by the GPS:

* Speed [Km/h]
	* Current
	* **Max**
	* **Moving average**
	* **Total average**

* Time
	* Current date and time
	* **Total time**
	* **Trip time**
	* **Stopped time**
	* **Last stop**
	* **Dawn and Dusk**

* Distance [Km]
	* **Odometer**
	* **Straight line distance from Home**

* Elevation above Mean Sea Level [m]
	* Current
	* **Max**
	* **Min**

* Location
	* Coordinates of the current location in DMS format
	* Heading

* ANT+ sensors integration
	* Heart rate [bpm]

* Miscellaneous
	* SD card logging
	* Temperature [Celsius]
	* **Number of satellites being tracked**
	* Distance [Km] and relative bearing to a given location
	* Battery percentage
	* Tracking mode (Time, Distance)
	* Dynamic platform (Portable, Stationary, Pedestrian, Automotive)


# Tracking modes and Dynamic platform

![Tracking mode Time](https://github.com/Gbertaz/GloBoSaT/blob/master/images/tracking_mode_time.jpg)
![Tracking mode Distance](https://github.com/Gbertaz/GloBoSaT/blob/master/images/tracking_mode_distance.jpg)


Tracking mode **Distance** is perfect for walks or trekking: the logger writes an entry every *X* meters. It is very useful when the speed is low and you don't want to end up with a confusing and not necessary heap of data. The resulting log file is much smaller.
The system automatically calculates and sets the best GPS update rate that allows to detect a movement equal to the target distance at current speed. Optimal GPS update rate means also lower power consumption.

Tracking mode **Auto** is not implemented yet.


# GPS modules comparison

The following are the results of the first simple comparison of two tracks recorded with the purchased [Ublox NEO-M8N Evaluation board](https://www.gnss.store/gnss-gps-modules/44-ublox-neo-m8n-gps-gnss-receiver-board-with-sma-for-uav-robots.html) and my [custom NEO-M9N board](https://github.com/Gbertaz/GloBoSaT/blob/master/images/neo-m9n.jpg). Far from being a scientific data comparison but enough to have an idea if my design works.

### Test case
Simple car ride of 4.2km indicated by the car odometer.\
Both tracks were recorded under the same cloudy conditions, same temperature and with the same [Active antenna](https://www.gnss.store/rf-gps-antennas/25-high-performance-active-gps-antenna.html).

Red:  NEO-M9N recorded @ 10hz\
Blue: NE0-M8N recorded @ 5hz

![Gps modules comparison](https://github.com/Gbertaz/GloBoSaT/blob/master/images/test1.png)

 They seem to be pretty accurate. The NEO-M8N is a little bit off on the upper left corner and on the right of the picture. After having processed the tracks with the [parser](https://github.com/Gbertaz/GlobosatTrackParser) I got the following output:
 
```
NEO-M8N TRACKING MODE TIME 5 Hz

Total GPS fixes:	1903
Lost GPS fixes:		5
Speed MAX:		76,3 Km/h
Speed AVG:		43,3 Km/h
Altitude MAX:		74,7 meters
Altitude MIN:		63,4 meters
Temperature MAX:	27,7 °C
Temperature MIN:	26,1 °C
Time total:		0:6:19
Time moving:		0:5:53
Distance:		4,3 Km
```
 
```
NEO-M9N TRACKING MODE TIME 10 Hz

Total GPS fixes:	3625
Lost GPS fixes:		3
Speed MAX:		79,9 Km/h
Speed AVG:		45,9 Km/h
Altitude MAX:		82,1 meters
Altitude MIN:		68,4 meters
Temperature MAX:	28,1 °C
Temperature MIN:	26,7 °C
Time total:		0:6:2
Time moving:		0:5:33
Distance:		4,3 Km
```
 
 The same route has been recorded with the tracking mode set on DISTANCE and here is the output, notice the significantly reduced number of fixes:
 
```
NEO-M9N TRACKING MODE DISTANCE 10 meters

Total GPS fixes:	311
Speed MAX:		80,9 Km/h
Speed AVG:		45,0 Km/h
Altitude MAX:		75,7 meters
Altitude MIN:		65,9 meters
Temperature MAX:	28,4 °C
Temperature MIN:	26,8 °C
Time total:		0:5:42
Time moving:		0:5:37
Distance:		4,2 Km
```

# Planned features and developments

* Waypoints management
* Tracks and routes management
* Replace the Nextion Display with a direct sunlight readable display
* Replace the GPS module with the more recent Ublox NEO-M9N
* Add a barometric pressure and relative humidity sensor
* Custom PCB
* 3D printed enclosure


## Hardware

* Teensy 4.1
* Display 3.2" Nextion Enhanced Version NX4024K032
* Adafruit Powerboost-1000C
* [Ublox NEO-M8N Evaluation board](https://www.gnss.store/gnss-gps-modules/44-ublox-neo-m8n-gps-gnss-receiver-board-with-sma-for-uav-robots.html)
* [Active GNSS Antenna](https://www.gnss.store/rf-gps-antennas/28-high-performance-multi-band-gnss-active-quad-helix-antenna-for-rtk.html)
* Temperature sensor DS18B20+
* Resistors: 2 4.7K, 1 10K
* Lipo battery 1s 3.7V
* Usb type A standard port female
* ANT+ usb dongle
* Switch
* JST connectors
* Dupont connector
