#include <WiFi.h>
#include <PubSubClient.h>   //MTQQ client

const char *SSID = "R2S";
const char *PWD = "7622849986";
//const char *SSID = "ROBOT-Ext-4";
//const char *PWD = "99$inmedi";
int status = WL_IDLE_STATUS;
// MQTT client

//define the mqttClient passing the WiFiClient
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 
//define the cloud MQTT broker address and the port, hivemq
char *mqttServer = "broker.hivemq.com";
int mqttPort = 1883;
int notConnectedCounter=0;
//WiFi connect
void connectToWiFi() {
  Serial.print("Connecting to ");
 
  WiFi.begin(SSID, PWD);
  Serial.println(SSID);
  while (WiFi.status() != WL_CONNECTED) 
  {
        Serial.print("Attempting to connect to WEP network, SSID: ");
        Serial.println(SSID);
        //    status = WiFi.begin(SSID, PWD);
        //     Serial.println(WiFi.status());
        //     Serial.println(WiFi.RSSI()); 
        
        //ESP32 sometimes connects to the Wifi network almost immediately and sometimes it doesn’t seem to connect at all 
        notConnectedCounter++;
        if(notConnectedCounter > 150) 
        { // Reset board if not connected after 5s
          Serial.println("Resetting due to Wifi not connecting...");
          ESP.restart();
        }
        
        delay(500);
  }
  Serial.print("Connected...    local_IP: ");
  Serial.println(WiFi.localIP());
  
}

//MQTT setup
void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  Serial.println((mqttServer));
  Serial.println((mqttPort));

  // set the callback function
  mqttClient.setCallback(callback);
}

//connect the ESP32 MQTT client to the broker so that the client can send data
void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");

      //try to connect random clientId, changes everytime we connect
      char* clientId = "clientId-nEfFe44LGQ";
      //clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect("RS")) {
        Serial.println("Connected.");
        // Once connected, publish an announcement...
        mqttClient.publish("esp-25072022","Beggin");
        // ... and resubscribe  
        // subscribe to topic 
        mqttClient.subscribe("/dcv7/joystick");
        delay(5000);
      }
      delay(1000);
      
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  connectToWiFi();

  setupMQTT();


}

void loop() {
  // put your main code here, to run repeatedly:
  //char data[10];
  
  if (!mqttClient.connected())
    reconnect();
  mqttClient.loop();

  //Serial.println(data);
  mqttClient.publish("esp-25072022","Qwerty");
  //mqttClient.publish("/dcv7/joystick", data);
}
