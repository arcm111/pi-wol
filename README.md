# pi-wol
Wake up diveces connected to your local network from anywhere using Raspberry Pi.

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

[Raspberry Pi 2 Model B Pinout](https://learn.sparkfun.com/tutorials/raspberry-gpio/all)

![circuit diagram](imgs/circuit.svg)
