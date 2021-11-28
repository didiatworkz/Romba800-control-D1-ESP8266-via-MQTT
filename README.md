# Romba 800 control D1 (ESP8266) via MQTT


An arduino program to add MQTT commands to your Roomba 800 series and control via home assistant

## Inspiration

To make my Roomba "Smart" for Home Assistant I have tested several instructions based on this video: https://www.youtube.com/watch?v=t2NgA8qYcFI
This fork is based on https://github.com/pilot1981/control-Roomba-ESP01-using-MQTT-and-HA and has been adapted for the Roomba 800 series.
In addition, improvements and enhancements were implemented, so that not everyone like me does everything wrong.

## Preparation

### Parts
+ D1 mini: https://amzn.to/2OStB1A
+ PNP Transistors: https://amzn.to/312FY1A
+ Buck Converters: https://amzn.to/32uC3uy *(choice high quality DC-DC buck converter because power supply of Roomba is very unstable and it can burst resistor of your buck converter)*

## Tools
+ Soldering Iron: https://amzn.to/2JUP3Mm
+ Some cables

## Schematic
![Schematic](https://github.com/didiatworkz/Romba800-control-D1-ESP8266-via-MQTT/raw/master/Roomba_Wiring_schematic.png)

## Libraries
Check the *libraries* folder in this project and import the .zip Files into Arduino IDE

## Roomba OI Document
https://git.io/JMWYH

---
