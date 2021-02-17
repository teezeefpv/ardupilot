==============
FETtec OneWire
==============

This is a half duplex serial protocol with a 2Mbit/s Baudrate created by Felix Niessen (former Flyduino KISS developer) from `FETtec <https://fettec.net/>`_.


Ardupilot to ESC protocol
=========================

How many bytes per message?
Which CRC? What proprieties has this CRC?
How many ESCs are supported? At wicht Rate?
What is the pause time between messages?
What can we configure in the ESCs?
What is the signal resolution?
Can the motors rotate in both directions? How is that done ?

ESC to Ardupilot protocol
=========================

How many bytes per message?
Which CRC? What proprieties has this CRC?

It supports ESC telemetry, so information from the ESC status can be sent back to the autopilot:

- Electronic rotations per minute (RPM) (must be divided by number of motor poles to translate to propeller RPM)
- Input voltage (V)
- Current draw (A)
- Power consumption (W)
- Temperature (°C)

At wicht Rate?

This information allows the autopilot to:

- dynamicaly change the center frequency of the notch filters used to reduce frame vibration noise in the gyros
- log the status of each ESC to the SDCard or internal Flash, for post flight analysis
- send the status of each ESC to the Ground Station or companion computer for real-time monitoring
