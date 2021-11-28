# Romba 800 control D1 (ESP8266) via MQTT


An arduino program to add MQTT commands to your Roomba 800 series and control via home assistant

## Inspiration

To make my Roomba "Smart" for Home Assistant I have tested several instructions based on this video: https://www.youtube.com/watch?v=t2NgA8qYcFI
This fork is based on https://github.com/pilot1981/control-Roomba-ESP01-using-MQTT-and-HA and has been adapted for the Roomba 800 series.
In addition, improvements and enhancements were implemented, so that not everyone like me does everything wrong.

## Preparation

### Parts
+ D1mini: https://amzn.to/2OStB1A
+ PNP Transistors 2N3906: https://amzn.to/312FY1A
+ Buck Converters 24V -> 3V: https://amzn.to/32uC3uy *(choice high quality DC-DC buck converter because power supply of Roomba is very unstable and it can burst resistor of your buck converter)*

### Tools
+ Soldering Iron
+ Some cables

### Libraries
Check the *libraries* folder in this project and import the .zip Files into Arduino IDE

### Roomba OI Document
https://git.io/JMWYH

### Prepare Arduino IDE for D1mini
#### Add ESP8266 boards
1) Open arduino IDE
2) Goto File -> Preferences
3) Add this URL `https://arduino.esp8266.com/stable/package_esp8266com_index.json` to *Additional Boards Manager URLs:*
![add_boards](https://github.com/didiatworkz/Romba800-control-D1-ESP8266-via-MQTT/raw/master/.github/add_boards.png)
4) Press *OK*

#### Choose the D1mini board
1) Choose *Tools+ -> *Boards* -> *ESP8266 Boards (..)* -> *LOLIN(WEMOS) D1 R2 & mini*

![choose_boards](https://github.com/didiatworkz/Romba800-control-D1-ESP8266-via-MQTT/raw/master/.github/choose_board.png)

#### Check settings and set Port
1) Choose *Tools+ -> *Port* -> *COM x* to select the right COM Port on your PC
2) The settings should be the same as in the image:

![check_settings](https://github.com/didiatworkz/Romba800-control-D1-ESP8266-via-MQTT/raw/master/.github/check_settings.png)

---

## Installation

### Build 
![Schematic](https://github.com/didiatworkz/Romba800-control-D1-ESP8266-via-MQTT/raw/master/Roomba_Wiring_schematic.png)

1) FIRST, BE SURE POWER SUPPLY OF DC DC BUCK CONVERTER HAVE GOOD QUALITY AND OUTPUT IS AT 3,3V;
2) SECOND, BE SURE WIRING ARE OK (please see attached photos)!
3) TEST YOUR FIRMWARE


### Firmware

1) Open the arduino project: *Roomba800/Roomba800.ino*
2) Change the the parameters:
```c
const char* ssid = "<YOUR SSID>";
const char* password = "<YOUR WIFI-PASSWORD>";
const char* mqtt_server = "<YOUR MQTT SERVER IP>";
const int mqtt_port = 1883;
const char *mqtt_user = "<YOUR MQTT USERNAME>";
const char *mqtt_pass = "<YOUR MQTT PASSWORD>";
const char *mqtt_client_name = "Roomba";
```
3) Verify code
4) Upload code to D1mini

### Home Assistant 
#### Setup MQTT
1) Goto *Supervisor* -> *Add-On Store* -> *Mosquitto broker*
2) Click on *Install*
3) After the Add-On is installed choose the Mosquitto broker from the *Dashboard*
4) Goto to *Configuration* tab
5) Change ` logins: [] ` to the same credentials like in the *Roomba800.ino*
```yaml
logins:
  - username: <YOUR MQTT USERNAME>
    password: <YOUR MQTT PASSWORD>
```
6) Click on *Save* and *Restart Add-on*

#### Add sensors / switch
1) Copy code from *configuration_custom_switch_sensor.yaml* into your configuration.yaml - You can also spit the file if you use a separate sensor.yaml
2) Copy code from *customize.yaml* to customize.yaml
3) Check Configuration via *Configuration* -> *Server Control* -> *Check Configuration* and restart the Server management if configuration files are valid.
4) Add the sensors/switch to your Lovelace interface -> Default see: *lovelace_custom.yaml* or  with button-card addon (install via HACS): *lovelace_custom_button-card.yaml*
5) Done

## Troubleshooting

Between each configuration/connection I suggest to do a factory default reset to be sure the roomba isn't in a strange/wrong state (ex. safe mode): you can do it please press and Hold the Dock and Spot button down for 10 seconds then release. I notice a strange state where to wake up and start to clean I have to press clean button for 3 or 4 times; after reset procedure all work fine and Roomba seem to wake up fine! This problem can prevent to start scheduled cleaning!

Many china products broke itself very soon (ex. buck converter), verify input/outpout voltage!
