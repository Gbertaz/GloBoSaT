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


Tracking mode **Distance** is perfect for walks or trekking: the logger writes an entry every *X* meters. It is very useful when the speed is very low and you don't want to end up with a confusing and not necessary heap of data. The resulting log file is much smaller.
The system automatically calculates and sets the best GPS update rate that allows to detect a movement equal to the target distance at current speed. Optimal GPS update rate means also lower power consumption.

Tracking mode **Auto** is not implemented yet.


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
