# Tracking modes and Dynamic platform

![Tracking mode Time](https://github.com/Gbertaz/GloBoSaT/blob/master/images/tracking_mode_time.jpg)
![Tracking mode Distance](https://github.com/Gbertaz/GloBoSaT/blob/master/images/tracking_mode_distance.jpg)

The settings section allow to select the tracking mode, logging interval and the dynamic platform.

# Tracking mode time

Writes a log entry every *X* time. At the moment the minimum frequency is 1 Hz. The maximum is 10 Hz.

# Tracking mode distance

This mode is perfect for walks or trekking: the logger writes an entry every *X* meters. It is very useful when the speed is low and you don't want to end up with a confusing and not necessary heap of data. The resulting log file is much smaller.
The system calculates and automatically adjusts the GPS update rate according to the current speed. The slower you are moving the slower is the update rate and viceversa.\ 
At the moment the minimum update rate is 1 Hz. The maximum is 10 Hz.
Optimal GPS update rate means also lower power consumption.

# Tracking mode distance

Not implemented yet.