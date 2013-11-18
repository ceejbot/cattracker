# Cat Tracker

An Arduino project for tracking a cat using GPS. 

## Overview

A GPS with a datalogger is attached to a cat collar. Heatshrink tubing holds the components to the collar & keeps them reasonably dry. 

The collar also has Bluetooth LE, paired to Bluetooth module on a stationary Beaglebone Black. When the cat walks past the Beagle, the devices connect. A node.js service running on the Beagle sends a command to the collar CPU asking for the current data payload. On successful receipt, it clears the memory on the GPS.

The Beagle is also running a web service that plots the GPS data points on a [Mapbox](https://www.mapbox.com) map.

## Parts List

[Teensy 2.0](http://www.pjrc.com/store/teensy.html) Arduino as a controller.

Sparkfun [Bluetooth Mate Silver](https://www.sparkfun.com/products/10393) to communicate between collar & basestation.

Adafruit [Ultimate GPS breakout](http://www.adafruit.com/products/746), with built-in datalogger.

[Beaglebone Black](http://www.adafruit.com/products/1278)

[Bluetooth USB module](http://www.adafruit.com/products/1327)

## Build

Diagram & how-to TBD.

## Software

* [gpsreader](./gpsreader/): runs on the Arduino; responds to commands from base station
* [catbase](./gpsreader/): runs on the Beagle; issues commands on connection
* catmap: the mapping server

### Required Libraries

[Adafruit GPS Library](https://github.com/adafruit/Adafruit-GPS-Library)  <br />

## Visualization

TBD

## Licence 

MIT.
