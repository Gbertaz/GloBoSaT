# GloBoSaT
First prototype of a General Purpose Satellite Tracker

The aim of the project is to build an high performance general purpose GPS tracker and logger that can be brought everywhere to log some amazing trips.

Based on NeoGPS library: https://github.com/SlashDevin/NeoGPS

![Tracking mode Time](https://github.com/Gbertaz/GloBoSaT/blob/master/images/front_back.jpg)

# Contents

* [Features](doc/Features.md)
* [Tracking modes](doc/TrackingModes.md)
* [Tests](doc/Tests.md)

# Planned features and developments

* Waypoints management
* Tracks and routes management
* Replace the Nextion Display with a direct sunlight readable display (JDI LPM027M128B)
* Replace the GPS module with the more recent Ublox NEO-M9N
* Integration of the Nordic Semiconductor nRF52832
* Add a barometric pressure and relative humidity sensor
* Custom PCB
* 3D printed enclosure


## Hardware

* Teensy 4.1
* Display 3.2" Nextion Enhanced Version NX4024K032
* Adafruit Powerboost-1000C
* [Ublox NEO-M8N Evaluation board](https://www.gnss.store/gnss-gps-modules/44-ublox-neo-m8n-gps-gnss-receiver-board-with-sma-for-uav-robots.html)
* [Custom Ublox NEO-M9N board](https://github.com/Gbertaz/GloBoSaT/blob/master/images/neo-m9n.jpg)
* [Active GNSS Antenna](https://www.gnss.store/rf-gps-antennas/28-high-performance-multi-band-gnss-active-quad-helix-antenna-for-rtk.html)
* Temperature sensor DS18B20+
* Resistors: 2 4.7K, 1 10K
* Lipo battery 1s 3.7V
* Usb type A standard port female
* ANT+ usb dongle
* Switch
* JST connectors
* Dupont connector
