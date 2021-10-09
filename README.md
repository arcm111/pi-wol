# pi-wol
Wake up diveces connected to your local network from anywhere using Raspberry Pi.

![circuit diagram](imgs/circuit.svg)

## Buttons
* Button [GPIO17]: Enables/disables the board
* Button [GPIO27]: Powers off the operating system.

## LEDs
* LED [GPIO22]: Indicates that the operating system has successfully booted up.
* LED [GPIO04]: Indicates that the internet connection is available.
* LED [GPIO05]: Indicates that a wol request has been received.
* LED [GPIO26]: Indicates an error.

## Other Pins
* GPIO06: Connected to a pull-up resistor and used to check if the board is connected to the device or not.

## Required Libraries:
* wiringPi
* pthread

## WiringPi Pins Mapping:
* GPIO04 -> 7
* GPIO05 -> 21
* GPIO06 -> 22
* GPIO17 -> 0
* GPIO22 -> 3
* GPIO26 -> 25
* GPIO27 -> 2

For more information about Pi 2 Model B pins mapping check:
[Raspberry Pi 2 Model B Pinout](https://learn.sparkfun.com/tutorials/raspberry-gpio/all)

## Web Server
Provides a user interface over the web to send wol requests to Raspberry Pi.
### Requirements:
* A running http server such as Apache.
* A compiled wol-cli binary added to web server directory with the name "wol".


