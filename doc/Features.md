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


# ANT+ integration

The heart rate measurements are obtained by an ANT+ usb dongle hooked up to the Teensy 4.1. It works but it is not realible at all. For this reason I am planning to integrate the Nordic Semiconductor nRF52832.
I already have the nRF52 Development Kit that I received from https://www.oemsecrets.com/