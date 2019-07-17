#include <PubSubClient.h>
#include <ESP8266WiFi.h>
//only if you need to connect to multiple AP WIFI
//#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <SimpleTimer.h>
#include <Roomba.h>

//USER CONFIGURED SECTION START//
const char* ssid = "xxxxxxx";
const char* password = "xxxxxxxx";
const char* mqtt_server = "xxxxxxxx";
const int mqtt_port = 1883;
const char *mqtt_user = "xxxxxxxx";
const char *mqtt_pass = "xxxxxx";
const char *mqtt_client_name = "Roomba"; // Client connections can't have the same connection name
//USER CONFIGURED SECTION END//

WiFiClient espClient;
PubSubClient client(espClient);
SimpleTimer timer;
Roomba roomba(&Serial, Roomba::Baud115200);

#define noSleepPin 0

// Variables
bool boot = true;
long battery_Current_mAh = 0;
long battery_Voltage = 0;
long battery_Total_mAh = 0;
long battery_percent = 0;
char battery_percent_send[50];
char battery_Current_mAh_send[50];
uint8_t tempBuf[10];
String status_log;
boolean connectioWasAlive = true;


//Function to connect to WIFI first time
void setup_wifi() 
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
  }
}

//verbose function to monitor WIFI connection in loop function, it not connected to WIFI; you can also use simpler setup_wifi() function
void monitorWiFi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    if (connectioWasAlive == true)
    {
      connectioWasAlive = false;
      Serial.print("Looking for WiFi ");
    }
    Serial.print(".");
    delay(500);
  }
  else if (connectioWasAlive == false)
  {
    connectioWasAlive = true;
    Serial.printf(" connected to %s\n", WiFi.SSID().c_str());
  }
}

//Function to reconnect to MQTT SERVER
void reconnect() 
{
  // Loop until we're reconnected
  int retries = 0;
  while (!client.connected()) 
  {
    if(retries < 50)
    {
      // Attempt to connect
      if (client.connect(mqtt_client_name, mqtt_user, mqtt_pass, "roomba/status", 0, 0, "Dead Somewhere")) 
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

//Function to richiamare????
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
      stopCleaning();
    }
  }
}

//Function to start cleaning of Roomba
void startCleaning()
{
//128 start command
  Serial.write(128);
  delay(50);
//131 safe command
  Serial.write(131);
  delay(50);
//135 cleaning mode
  Serial.write(135);
  client.publish("roomba/status", "Cleaning");
}

//Function to send to dock station your Roomba
void stopCleaning()
{
//128 start command
  Serial.write(128);
  delay(50);
//131 safe command
  Serial.write(131);
  delay(50);
//143 dock station direction
  Serial.write(143);
  client.publish("roomba/status", "Returning");
}

//Function to send Roomba info to MQTT Server
void sendInfoRoomba()
{
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
}


//Mainly function to start before all other functions, it run only one time/first time you boot ESP01
void setup() 
{
//configura ESP per comunicare al baud rate di 115200
  Serial.begin(115200);
//129 baud (codice 11 indica il baud rate 115200, che è comunque quello di default del roomba)
  Serial.write(129);
  delay(50);
  Serial.write(11);
  delay(50);
// configura la parte WIFI del ESP01 per collegarsi al AP WIFI con i dati defini all'inizio  
  WiFi.mode(WIFI_STA);
  setup_wifi();

  // Reset the Roomba.
  Serial.begin(115200);
  pinMode(noSleepPin, OUTPUT);
  digitalWrite(noSleepPin, HIGH);
  // it wake up Roomba the first time
  resetRoomba();
  // it connect to MQTT server
  client.setServer(mqtt_server, mqtt_port);
  //richiama la funzione dediucata di start o stop in base al comando ricevuto via MQTT
  client.setCallback(callback);
  timer.setInterval(5000, sendInfoRoomba);
  timer.setInterval(59000, StayAwake);
}

//Function to wake up Roomba 500 from power saving mode (you have to use it only if you have this feature in last firmware Roomba(
void StayAwake() {
  status_log += "Pulsing the BRC pin...\n";
  digitalWrite(noSleepPin, LOW);
  delay(100);
  digitalWrite(noSleepPin, HIGH);
}

//Function to wake up Roomba 500 from power saving mode (you have to use it only if you have this feature in last firmware Roomba(
void resetRoomba() {
//128 start
//129 baud (codice 11 indica il baud rate 115200, il codice 7 indica il baud rate 19200)
  byte command[] = { 128, 129, 11, 7 };
//inviando 3 segnali consecutivi di baud rate change si abbasserebbe il baud rate utilizzato per comunicare col Roomba, invece si vuole solo tenere acceso il Roomba  
  digitalWrite(noSleepPin, LOW);
  delay(100);
  digitalWrite(noSleepPin, HIGH);
  Serial.begin(19200);
  SendCommandList(command, 4);
  delay(500);
  Serial.begin(115200);
  SendCommandList(command, 4);
  digitalWrite(noSleepPin, LOW);
  delay(100);
  digitalWrite(noSleepPin, HIGH);
  timer.setTimeout(5000, StayAwake);
}

//Function to wake up Roomba 500 from power saving mode (you have to use it only if you have this feature in last firmware Roomba(
void SendCommandList( byte *ptr, byte len ) {
  status_log += "TX:";
  for ( int i = 0; i < len ; i++ ) {
    status_log += " ";
    status_log += ptr[i];
    Serial.write(ptr[i]);
  }
  delay(25);
  status_log += "\n";
}

//Function to loop continuosily MQTT server re-connection + riconnessione WIFI
void loop() 
{
//monitor MQTT connection
if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
  timer.run();

//monitor WIFI connection
//  monitorWiFi();
  setup_wifi(); 
}
