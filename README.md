# GloBoSaT
General Purpose Satellite Tracker

The aim of the project is to build an high performance general purpose Gps tracker and logger that can be brought everywhere to log some amazing trips.

# Current features

These are the values displayed on the screen, either calculated or directly provided by the Gps: 

* Speed [Km/h]
	* Current
	* Max
	* Moving average
	* Total average

* Time
	* Current date and time
	* Total time
	* Trip time
	* Stopped time
	* Last stop
	* Dawn and Dusk

* Distance [Km]
	* Odometer
	* Straight line distance from Home

* Elevation [m]
	* Current
	* Max
	* Min

* Location
	* Coordinates of the current location in DMS format
	* Heading

* ANT+ sensors integration
	* Heart rate [bpm]

* Miscellaneous
	* Temperature [Celsius]
	* Number of satellites being tracked
	* Distance [Km] and relative bearing to a given location
	* Battery percentage
	* Tracking mode (Time, Distance)
	* Dynamic platform (Portable, Stationary, Pedestrian, Automotive)


# Planned features and developments

* Waypoints management
* Replace the Nextion Display with a direct sunlight readable display
* Add a barometric pressure and relative humidity sensor (Bosh BMP280)
* Update the gps module to use the more recent Ublox NEO-M9N
* Create a custom PCB


## Hardware

* Teensy 4.1
* Display 3.2" Nextion Enhanced Version NX4024K032
* Adafruit Powerboost-1000C
* Ublox NEO-M8N Evaluation board
* Active GNSS Antenna
* Temperature sensor DS18B20+
* Resistors: 2 4.7K, 1 10K
* Battery Lipo 1s 3.7V
* Usb type A standard port female
* ANT+ usb dongle
* Switch
* JST connectors
* Dupont connector
