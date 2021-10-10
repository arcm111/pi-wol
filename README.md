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
Pi Pin | wiringPi
------ | ------
GPIO04 | 7
GPIO05 | 21
GPIO06 | 22
GPIO17 | 0
GPIO22 | 3
GPIO26 | 25
GPIO27 | 2

For more information about Pi 2 Model B pins mapping check:
[Raspberry Pi 2 Model B Pinout](https://learn.sparkfun.com/tutorials/raspberry-gpio/all)

## Socket Server
The compiled binary in the server folder when executed will start a UDP Socket server listening on port 5649 for incoming wol requests.  
### Configuration
To listen for requests as soon as the Raspberry Pi device has powered on, we need to run the server at startup.  
One way to do that is to add a command to the end of the `/etx/rc.local` file just before the `exit 0` line:  
```console
/home/arcm/projects/c/wol-server/server >> /var/log/wol-errors 2>&1 &
```
This command will run the script on startup using `sudo` privilage and save the outputs into a log file saved in `/var/log/wol-errors`.  

## Web Server
Provides a user interface over the web to send wol requests to Raspberry Pi.  
To be able to access the website remotely, port-forwarding settings need to be configured on the router to allow incoming traffic on port 80 to be forwarded to Raspberry Pi.  
### Requirements:
* A running http server such as Apache.
* A compiled wol-cli binary added to web server directory with the name "wol".


