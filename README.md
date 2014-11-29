# PIC based APRS Tracking Beacon

I started this project as a fun experiment to see if I could generate an AFSK signal with a small PIC and a resistor ladder network.  It worked out well so I made it into a full tracking beacon.  We fly these on high altitude balloons to track and recover the balloon's payload.  Here's a few of the key features:

- Simple and inexpensive to build.  AFSK is generated using a resistor network so no modem chip is needed
- Adjustable audio output allows interfacing with a variety of radios
- Integrated GPS with helical antenna - no cables to break and works in any orientation
- NMEA data from the GPS is stored on a FAT32 formatted micro SD card
- Software is modular and easy to understand

Well, the last one might be open to debate :-)  It runs on a PIC18F2525 with an SD card hooked up to the SPI port.  I put the GPS and GPS antenna on the same PCB but unfortunately that antenna can no longer be bought from sparkfun - Sarantel seems to be out of business. 

An MPLAB-X project is included and works with the latest version (2.26).  I use the Microchip XC8 compiler.  More details can be seen here: http://wiki.ad7zj.net/wiki/index.php/PIC_APRS_Beacon
