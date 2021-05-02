# C-Dispatcher-for-Embedded-Systems

This is my full implementation for the embedded system task dispatcher of Ron Kreymborgi (Ron Kreymborg, "A Compact Task Dispatcher for Embedded Systems"). It's a small but yet powerful task dispatcher aimed specifically at resource starved systems. It is suitable for many small embedded systems which have restrictions on program memory size.

# Paper:
The dispatcher is described in this paper: https://drive.google.com/file/d/0Bw1wxZHj3VakNnZ3eFVEbUhWZ2s
![Added Work Zones on CARLA](pics/dispatcher.gif?raw=true)

# Test Application:
I have also integrated it into a PIC32 project and provided a test application for my implementation. It's provided in Microchip MPLAB-X:
Download Link: https://drive.google.com/file/d/151xt032m-8rKEwMDToFIfY7PQZEtip31
To run it, install the Microchip MPLAB-X development environment as well as XC32 compiler (make sure that you install PLIB).
![Added Work Zones on CARLA](pics/test_results.png?raw=true)
