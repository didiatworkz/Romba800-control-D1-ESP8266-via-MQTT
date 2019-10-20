# ESP-01_Roomba

MODIFIED VERSION TO FIX 2 PROBLEMS:
- WIFI reconnection problem (please check first your hardware, some ESP01 have serious problem with WIFI);
- Wake UP Roomba 500 with lastest firmware 3.8.2 which introduce POWER SAVING MODE (the same present in all newst series as 600, 700, 900); this could be a problem to comand Roomba via ESP when Roomba is sleeping...


An arduino program to add MQTT commands to your roomba 500 or 600 series.

https://www.youtube.com/watch?v=t2NgA8qYcFI


Arduino Sketch:
https://github.com/thehookup/MQTT-Roo...

Required Arduino Libraries:
http://www.airspayce.com/mikem/arduin...
https://github.com/jfturcot/SimpleTimer
https://github.com/knolleary/pubsubcl...

Configuration.yaml Additions:
https://github.com/thehookup/MQTT-Roo...

customize.yaml Additions:
https://github.com/thehookup/MQTT-Roo...

NodeRED Flows:
https://github.com/thehookup/MQTT-Roo...

*REMEMBER TO CHANGE HOSTNAME OF MQTT SERVER

NodeRED scheduling node:
https://www.npmjs.com/package/node-re...

Wiring Schematic:
https://github.com/thehookup/MQTT-Roo...

Parts:
ESP-01: https://amzn.to/2qVB2p8
PNP Transistors: https://amzn.to/2FaUfrS
Buck Converters: https://amzn.to/2K7FY33 (choice high quality DC-DC buck converter because power supply of Roomba is very unstable and it can burst resistor of your buck converter)

Roomba upgrades:
Lithium Battery: https://amzn.to/2JnbGbp
Brush Upgrades: https://amzn.to/2qVDTOB

Tools:
Soldering Iron: https://amzn.to/2JUP3Mm
USB to TTL Adapter: https://amzn.to/2Hak9BW

Roomba OI Document:
http://cfpm.org/~peter/bfz/iRobot_Roo...

