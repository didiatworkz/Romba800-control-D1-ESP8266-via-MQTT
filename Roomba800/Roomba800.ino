#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Roomba.h>
#include <ArduinoOTA.h>


//USER CONFIGURED SECTION START//
const char* ssid = "<YOUR SSID>";
const char* password = "<YOUR WIFI-PASSWORD>";
const char* mqtt_server = "<YOUR MQTT SERVER IP>";
const int mqtt_port = 1883;
const char *mqtt_user = "<YOUR MQTT USERNAME>";
const char *mqtt_pass = "<YOUR MQTT PASSWORD>";
const char *mqtt_client_name = "Roomba"; // Client connections can't have the same connection name
//USER CONFIGURED SECTION END//


WiFiClient espClient;
PubSubClient client(espClient);
Roomba roomba(&Serial, Roomba::Baud115200);


// Variables
bool toggle = true;
const int noSleepPin = 2;
bool boot = true;
long battery_Current_mAh = 0;
long battery_Voltage = 0;
long battery_Total_mAh = 0;
long battery_percent = 0;
char battery_percent_send[50];
char battery_Current_mAh_send[50];
uint8_t tempBuf[10];

//Functions

void setup_wifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
}

void reconnect()
{
  // Loop until we're reconnected
  int retries = 0;
  while (!client.connected())
  {
    if(retries < 50)
    {
      // Attempt to connect
      if (client.connect(mqtt_client_name, mqtt_user, mqtt_pass, "roomba/status", 0, 0, "Ready"))
      {
        // Once connected, publish an announcement...
        if(boot == false)
        {
          client.publish("checkIn/roomba", "Reconnected");
        }
        if(boot == true)
        {
          client.publish("checkIn/roomba", "Rebooted");
          boot = false;
        }
        // ... and resubscribe
        client.subscribe("roomba/commands");
      }
      else
      {
        retries++;
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
    if(retries >= 50)
    {
      ESP.restart();
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length)
{
  String newTopic = topic;
  payload[length] = '\0';
  String newPayload = String((char *)payload);
  if (newTopic == "roomba/commands")
  {
    if (newPayload == "start")
    {
      startCleaning();
    }
    if (newPayload == "stop")
    {
      stopCleanig();
    }
    if (newPayload == "home")
    {
      goHome();
    }
    if (newPayload == "status")
    {
      sendInfoRoomba();
    }
    if (newPayload == "ping")
    {
      ping();
    }

  }
}


void startCleaning()
{
  awake();
  roomba.start();
  delay(50);
  roomba.safeMode();
  delay(50);
  roomba.cover();
  client.publish("roomba/status", "Cleaning");
}

void stopCleanig()
{
  roomba.start();
  delay(50);
  roomba.cover();
  client.publish("roomba/status", "Halted");
}

void goHome()
{
  awake();
  roomba.start();
  delay(50);
  roomba.safeMode();
  delay(50);
  roomba.coverAndDock();
  client.publish("roomba/status", "Returning");
}

void ping()
{
  uint8_t song[] = {64, 32, 64, 32, 64, 32, 60, 16, 67, 12, 64, 32, 60, 16, 67, 12, 64, 64, 71, 32, 71, 32, 71, 32, 72, 16, 67, 12, 64, 32, 60, 16, 67, 12, 64, 64};
  int song_size = sizeof(song) / 2;
  awake();
  Serial.write(128);
  Serial.write(131);
  Serial.write(140);
  Serial.write(0);
  Serial.write(song_size);
  for (int i = 0; i < sizeof(song); i++) {
    Serial.write(song[i]);
  }
  Serial.write(141);
  Serial.write(0);
  client.publish("roomba/status", "Pong"); 
}

void sendInfoRoomba()
{
  awake();
  roomba.start();
  roomba.getSensors(21, tempBuf, 1);
  battery_Voltage = tempBuf[0];
  delay(50);
  roomba.getSensors(25, tempBuf, 2);
  battery_Current_mAh = tempBuf[1]+256*tempBuf[0];
  delay(50);
  roomba.getSensors(26, tempBuf, 2);
  battery_Total_mAh = tempBuf[1]+256*tempBuf[0];
  if(battery_Total_mAh != 0)
  {
    int nBatPcent = 100*battery_Current_mAh/battery_Total_mAh;
    String temp_str2 = String(nBatPcent);
    temp_str2.toCharArray(battery_percent_send, temp_str2.length() + 1); //packaging up the data to publish to mqtt
    client.publish("roomba/battery", battery_percent_send);
  }
  if(battery_Total_mAh == 0)
  {
    client.publish("roomba/battery", "NO DATA");
  }
  String temp_str = String(battery_Voltage);
  temp_str.toCharArray(battery_Current_mAh_send, temp_str.length() + 1); //packaging up the data to publish to mqtt
  client.publish("roomba/charging", battery_Current_mAh_send);
  if(atoi(battery_Current_mAh_send) == 5) 
  {
    client.publish("roomba/status", "Charging Fault Condition");
  }
  else if(atoi(battery_Current_mAh_send) == 4) 
  {
    client.publish("roomba/status", "Waiting");
  }
  else if(atoi(battery_Current_mAh_send) == 3) 
  {
    client.publish("roomba/status", "Charging");
  }
  else if(atoi(battery_Current_mAh_send) == 2) 
  {
    client.publish("roomba/status", "Charging");
  }
  else if(atoi(battery_Current_mAh_send) == 1) 
  {  
    client.publish("roomba/status", "Charging");
  }
  else if(atoi(battery_Current_mAh_send) == 0) 
  {
    client.publish("roomba/status", "Not charging");
  }
}

void awake()
{
  client.publish("roomba/status", "Wake up");
  digitalWrite(noSleepPin, HIGH);
  delay(1000);
  digitalWrite(noSleepPin, LOW);
  delay(1000);
  digitalWrite(noSleepPin, HIGH);
  delay(1000);
  digitalWrite(noSleepPin, LOW);
}

void setup()
{
  pinMode(noSleepPin, OUTPUT);
  digitalWrite(noSleepPin, HIGH);
  Serial.begin(115200);
  Serial.write(129);
  delay(50);
  Serial.write(11);
  delay(50);
  setup_wifi();
  ArduinoOTA.setHostname(mqtt_client_name);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop()
{ 
  ArduinoOTA.handle();
  delay(1000);
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}
