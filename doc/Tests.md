# GPS modules comparison

The following are the results of the first simple comparison of two tracks recorded with the purchased [Ublox NEO-M8N Evaluation board](https://www.gnss.store/gnss-gps-modules/44-ublox-neo-m8n-gps-gnss-receiver-board-with-sma-for-uav-robots.html) and my [custom NEO-M9N board](https://github.com/Gbertaz/GloBoSaT/blob/master/images/neo-m9n.jpg). Far from being a scientific data comparison but enough to have an idea if my design works.

### Test case
Simple car ride of 4.2km indicated by the car odometer.\
Both tracks were recorded under the same cloudy conditions, same temperature and with the same [Active antenna](https://www.gnss.store/rf-gps-antennas/25-high-performance-active-gps-antenna.html).

Red:  NEO-M9N [Tracking mode](doc/TrackingModes.md) TIME @ 10hz - automotive \
Blue: NE0-M8N [Tracking mode](doc/TrackingModes.md) TIME @ 5hz - automotive

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
 
 The same route has been recorded with the [Tracking mode](doc/TrackingModes.md) set on DISTANCE and here is the output, notice the significantly reduced number of fixes:
 
![Tracking mode distance](https://github.com/Gbertaz/GloBoSaT/blob/master/images/detail10m.png)

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
