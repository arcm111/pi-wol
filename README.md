# pi-wol
Wake up devices connected to your local network from anywhere using Raspberry Pi.

![circuit diagram](imgs/circuit.svg)

## Buttons
* Button [GPIO17]: Enables/disables the board
* Button [GPIO27]: Powers off the operating system.

## LEDs
* LED [GPIO22]: Indicates that the operating system has successfully booted up.
* LED [GPIO04]: Indicates that internet connection is available.
* LED [GPIO05]: Indicates that a wol request has been received or when the operating system has started.
* LED [GPIO26]: Indicates an error or when the operating system is being powered off.

## Other Pins
* GPIO06: Connected to a pull-up resistor and used to check if the board is connected to the device or not.

## Required Libraries:
* [wiringPi](http://wiringpi.com/download-and-install/)
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
The compiled binary in the server folder when executed will start a `UDP Socket` server listening on port `5649` for incoming wol requests.  
### Configuration
1. To listen for requests as soon as the Raspberry Pi device has powered on, we need to run the server at startup.  
One way to do that is to add a command to the end of the `/etx/rc.local` file just before the `exit 0` line:  
```
/path/to/socket/server/directory/server >> /var/log/wol-errors 2>&1 &
```
This command will run the script on startup using `sudo` privilage and save the outputs into a log file saved in `/var/log/wol-errors`.  
2. To be able to connect to this server remotely, port-forwarding needs to be configured on the router to forward traffic on UDP port 5649 to Raspberry Pi.  
3. The server needs the `broadcast ip address` for the local network on which Raspberry Pi is connected to. The broadcast ip need to be added into the `wol.conf` file. For example:
```
broadcast ip = 192.168.1.255
```
4. To send wol requests, a connection needs to be established first and then a magic packet needs to be sent. Once received the server will forward it to the broadcasting ip found in `wol.conf` which in turn will be forwarded to the destination device matching the `MAC` address found in the packet. Then if the destination device is configured with the wake-on-lan feature enabled, it will boot up.

## Magic Packet
A magic packet is a broadcast frame containing 6 bytes of all 255 (`FF FF FF FF FF FF`), followed by 16 repititions of the destination's `MAC` address. The total length of the packet is 102 bytes.  
For example to send a magic packet to a netword device with a `MAC` address of `B0:36:81:C4:C3:9F`, the following magic packet needs to be sent:
```
FF FF FF FF FF FF B0 36 81 C4 C3 9F B0 36 81 C4 C3 9F B0 36 81 C4 C3 9F  
B0 36 81 C4 C3 9F B0 36 81 C4 C3 9F B0 36 81 C4 C3 9F B0 36 81 C4 C3 9F  
B0 36 81 C4 C3 9F B0 36 81 C4 C3 9F B0 36 81 C4 C3 9F B0 36 81 C4 C3 9F  
B0 36 81 C4 C3 9F B0 36 81 C4 C3 9F B0 36 81 C4 C3 9F B0 36 81 C4 C3 9F  
B0 36 81 C4 C3 9F B0 36 81 C4 C3 9F
```

## Web Server
Provides a user interface over the web to send wol requests to Raspberry Pi. The web server was mainly intended for testing.  
To be able to access the website remotely, port-forwarding settings need to be configured on the router to allow incoming traffic on port 80 to be forwarded to Raspberry Pi.  
### Requirements:
* A running http server such as Apache.
* A compiled wol-cli binary added to web server directory with the name "wol".


