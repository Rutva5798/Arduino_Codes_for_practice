
/*Joystick interface with ESP32, 
 *analog read differs from arduino board readings
 *
 *NOTE: Connect +5V pin of joystick with 3.3V pin of ESP32S
 *
 *Place joystick with wires at bottom side and check dir:
 *center both -> 1850-1970 
 *Up -> y= 1850-1970 , x=4095 max 
 *Dn -> y= 1850-1970 , x=0 min
 *L -> x=1850-1970, y=0
 *R -> x=1850-1970, y=4095
 * 
 */

#include <WiFi.h>
#include <PubSubClient.h>   //MTQQ client

//Joystick redings classified
#define Center_L 1800     //1700  //1900
#define Center_H 2000     //2100  //1920
#define Max 4095
#define Min 0
#define Offset 05         //200
#define slow_detect_offset 500
#define Max_limit (Max - ((Max-Center_H)/2))
#define Min_limit (Min + ((Center_L- Min)/2))

//Joystick interface 
#define MOVE_STATE_NONE 0
#define MOVE_STATE_FORWARD 1
#define MOVE_STATE_BACKWARD 2
#define MOVE_STATE_LEFT 3
#define MOVE_STATE_RIGHT 4
#define MOVE_STATE_UL 5
#define MOVE_STATE_DL 6
#define MOVE_STATE_DR 7
#define MOVE_STATE_UR 8
#define MOVE_STATE_FORWARD_SLOW 9
#define MOVE_STATE_BACKWARD_SLOW 10
#define MOVE_STATE_LEFT_SLOW 11
#define MOVE_STATE_RIGHT_SLOW 12

#define VRx 34  //ADC1
#define VRy 35  //ADC1
#define SW 21

int xPosition = 0;
int yPosition = 0;
int SW_state = 0;

int MOVE_STATE = 0;

char n[1]={1};
char publish_topic[20] = "dcv7/joystick/car-";

void fun_print(int x, int y, int z);
void fun_logic(int xPosition, int yPosition, int SW_state );


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

  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT); 


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
        mqttClient.publish("dcv7/joystick/car-1","Car-1 Begins");
        mqttClient.publish("dcv7/joystick/car-2","Car-2 Begins");
        mqttClient.publish("dcv7/joystick/car-3","Car-3 Begins");
        // ... and resubscribe  
        // subscribe to topic 
        mqttClient.subscribe("dcv7/car/select");
        
        delay(5000);

//        
      }
      delay(1000);
      
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  //publish_topic[18]=(char)payload[0]; 
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
    
  }
  Serial.println();
  if (((char)payload[0]=='1')||((char)payload[0]=='2')||((char)payload[0]=='3'))
    publish_topic[18]=(char)payload[0];
  else  
    publish_topic[18]='\0';
  Serial.println(publish_topic);
  mqttClient.publish(publish_topic,"Joystick is at center ");

}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  connectToWiFi();

  setupMQTT();


}

void loop() {
  // put your main code here, to run repeatedly:

  if(WiFi.status() != WL_CONNECTED) 
    connectToWiFi();
    
  if (!mqttClient.connected())
    reconnect();
    
  mqttClient.loop();

  // Once car selected by client, publish an announcement... to selected car 1/2/3
  //mqttClient.publish(publish_topic,"Beggin");
  xPosition = analogRead(VRx);
  yPosition = analogRead(VRy);
  SW_state = digitalRead(SW);
               
    fun_logic(xPosition, yPosition, SW_state);
     

}

void fun_print(int x, int y, int z)
{
  
  Serial.print("X: ");
  Serial.println(x);
  Serial.print("Y: ");
  Serial.println(y);
  Serial.print("SW state: ");
  Serial.println(z);
  Serial.println(); 
}

void fun_logic(int xPosition, int yPosition, int SW_state)
{


  //Fprward  
  if((xPosition >= (Max-Offset)) && (xPosition <= (Max+Offset)))
  {

    //To check for up-right
    if(((Max_limit) <= yPosition) && (yPosition <= (Max)))
    {
      if (MOVE_STATE != MOVE_STATE_UR)
      {
        MOVE_STATE = MOVE_STATE_UR;
        Serial.println("Move in Up-Right Direction ");
        mqttClient.publish(publish_topic,"Move in Up-Right Direction ");
  
        fun_print(xPosition, yPosition, SW_state); 
  
      }      
    }
    //Check for up-right
    else if(((Min) <= yPosition) && (yPosition <= (Min_limit)))
    {
      if (MOVE_STATE != MOVE_STATE_UL)
      {
        MOVE_STATE = MOVE_STATE_UL;
        Serial.println("Move in Up-Left Direction ");
        mqttClient.publish(publish_topic,"Move in Up-Left Direction ");
        fun_print(xPosition, yPosition, SW_state); 
  
      }      
    } 
    //Default up-dir
    else
    {
          if (MOVE_STATE != MOVE_STATE_FORWARD)
          {
            MOVE_STATE = MOVE_STATE_FORWARD;
            Serial.println("Move Forward ");
            mqttClient.publish(publish_topic,"Moving in Forward direction ");
      
            fun_print(xPosition, yPosition, SW_state);
      
            
          }
    }
      
  }

  
  //Down
  else if((xPosition >= Min) && (xPosition <= (Min+Offset)))
  {

    //To check for down-left
    if(((Min_limit) <= yPosition) && (yPosition <= (Min)))
    {
      if (MOVE_STATE != MOVE_STATE_DL)
      {
        MOVE_STATE = MOVE_STATE_DL;
        Serial.println("Move in Down-Left Direction ");
        mqttClient.publish(publish_topic,"Move in Down-Left Direction ");
  
        fun_print(xPosition, yPosition, SW_state); 
  
      }      
    }
    //Check for down-right
    else if(((Max_limit) <= yPosition) && (yPosition <= (Max)))
    {
      if (MOVE_STATE != MOVE_STATE_DR)
      {
        MOVE_STATE = MOVE_STATE_DR;
        Serial.println("Move in Down-Right Direction ");
        mqttClient.publish(publish_topic,"Move in Down-Right Direction ");
        fun_print(xPosition, yPosition, SW_state); 
  
      }      
    } 
    //Default down-dir
    else
    {
      if (MOVE_STATE != MOVE_STATE_BACKWARD)
      {
        MOVE_STATE = MOVE_STATE_BACKWARD;
        Serial.println("Move Backward ");
        mqttClient.publish(publish_topic,"Moving in Backward direction ");
  
        fun_print(xPosition, yPosition, SW_state);      
  
      }      
    }
        
  }

  



  //Left
  else if((yPosition >= Min) && (yPosition <= (Min + Offset)))
  {
      //To check for up-left
    if(((Max_limit) <= xPosition) && (xPosition <= (Max)))
    {
      if (MOVE_STATE != MOVE_STATE_UL)
      {
        MOVE_STATE = MOVE_STATE_UL;
        Serial.println("Move in Up-Left Direction ");
        mqttClient.publish(publish_topic,"Move in Up-Left Direction ");
  
        fun_print(xPosition, yPosition, SW_state); 
  
      }      
    }
    //Check for down-Left
    else if(((Min) <= xPosition) && (xPosition <= (Min_limit)))
    {
      if (MOVE_STATE != MOVE_STATE_DL)
      {
        MOVE_STATE = MOVE_STATE_DL;
        Serial.println("Move in Down-Left Direction ");
        mqttClient.publish(publish_topic,"Move in Down-Left Direction ");
        fun_print(xPosition, yPosition, SW_state); 
  
      }      
    } 
    //Default left-dir
    else
    {
      if (MOVE_STATE != MOVE_STATE_LEFT)
      {
        MOVE_STATE = MOVE_STATE_LEFT;
        Serial.println("Move in Left Direction ");
        mqttClient.publish(publish_topic,"Moving in Left direction ");
  
        fun_print(xPosition, yPosition, SW_state); 
  
      }      
    }
  }

  //Right
  else if((yPosition >= (Max-Offset)) && (yPosition <= (Max+Offset)))
  {

    //To check for up-right
    if(((Max_limit) <= xPosition) && (xPosition <= (Max)))
    {
      if (MOVE_STATE != MOVE_STATE_UR)
      {
        MOVE_STATE = MOVE_STATE_UR;
        Serial.println("Move in Up-Right Direction ");
        mqttClient.publish(publish_topic,"Move in Up-Right  Direction ");
  
        fun_print(xPosition, yPosition, SW_state); 
  
      }      
    }
    //Check for down-right
    else if(((Min) <= xPosition) && (xPosition <= (Min_limit)))
    {
      if (MOVE_STATE != MOVE_STATE_DR)
      {
        MOVE_STATE = MOVE_STATE_DR;
        Serial.println("Move in Down-Right Direction ");
        mqttClient.publish(publish_topic,"Move in Down-Right Direction ");
        fun_print(xPosition, yPosition, SW_state); 
  
      }      
    }
    
    //Default right-dir
    else
    {
      if (MOVE_STATE != MOVE_STATE_RIGHT)
      {
        MOVE_STATE = MOVE_STATE_RIGHT;
        Serial.println("Moving in Right direction ");
        mqttClient.publish(publish_topic,"Moving in Right direction ");
  
        fun_print(xPosition, yPosition, SW_state); 
  
      }      
    }
  }

  
  //Default-center
  else if(((Center_L) <= xPosition) && (xPosition <= (Center_H)) && ((Center_L) <= yPosition) && (yPosition <= (Center_H)))
  {
    if (MOVE_STATE != MOVE_STATE_NONE)
    {
      MOVE_STATE = MOVE_STATE_NONE;
      Serial.println("Joystick is at center ");
      mqttClient.publish(publish_topic,"Joystick is at center ");

      fun_print(xPosition, yPosition, SW_state);


    }      
  }
  


  //To check speed of motion from center and if joystick at rest
  else if((((Center_L) <= xPosition) && (xPosition <= (Center_H))) || (((Center_L) <= yPosition) && (yPosition <= (Center_H))) )
  {
      if ((xPosition >= (Center_L)) && (xPosition <= (Center_H)))
      {
            if((yPosition <= ((Center_L) - slow_detect_offset) ) && (yPosition >= (Min+slow_detect_offset)))
            {
              if (MOVE_STATE != MOVE_STATE_LEFT_SLOW)
              {
                          MOVE_STATE = MOVE_STATE_LEFT_SLOW;
                          Serial.println("Move in Left Direction  -   Slow speed ");
                          mqttClient.publish(publish_topic,"Move in Left Direction  -   Slow speed ");  
                          fun_print(xPosition, yPosition, SW_state);                     
              }

          
            }      
           
            else if((((Center_H) + slow_detect_offset) <= yPosition) && (yPosition <= (Max-slow_detect_offset)))
            {
              if (MOVE_STATE != MOVE_STATE_RIGHT_SLOW)
              {
                          MOVE_STATE = MOVE_STATE_RIGHT_SLOW;
                          Serial.println("Move in Right Direction  -   Slow speed ");
                          mqttClient.publish(publish_topic,"Move in Right Direction  -   Slow speed ");                         
                    
                          fun_print(xPosition, yPosition, SW_state); 
              }
            } 
  
      }   

      else if (((Center_L) <= yPosition) && (yPosition <= (Center_H))) 
      {
            if((xPosition <= ((Center_L) - slow_detect_offset) ) && (xPosition >= (Min+slow_detect_offset)))
            { 
              if (MOVE_STATE != MOVE_STATE_BACKWARD_SLOW)
              {
                MOVE_STATE = MOVE_STATE_BACKWARD_SLOW;
                Serial.println("Move in Backward Direction  -   Slow speed ");
                mqttClient.publish(publish_topic,"Move in Backward Direction  -   Slow speed  ");                              
                    
                fun_print(xPosition, yPosition, SW_state); 
          
              }      
            }
            
            else if((((Center_H) + slow_detect_offset) <= xPosition) && (xPosition <= (Max-slow_detect_offset)))
            {
              if (MOVE_STATE != MOVE_STATE_FORWARD_SLOW)
              {   
                MOVE_STATE = MOVE_STATE_FORWARD_SLOW;
                Serial.println("Move in Forward Direction  -   Slow speed ");
                mqttClient.publish(publish_topic,"Move in Forward Direction  -   Slow speed ");
                fun_print(xPosition, yPosition, SW_state); 
              }      
            } 
  
      } 
       
  }
  
//  else
//  {
//    Serial.println("Moving inbetween");
//    fun_print(xPosition, yPosition, SW_state); 
//    delay((1000));
//  }

}
