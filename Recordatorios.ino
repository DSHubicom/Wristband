#include <WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

DFRobotDFPlayerMini myDFPlayer;

// Update these with values suitable for your network.
const char* ssid = "jc";
const char* password = "juancarlos";
const char* mqtt_server = "damaso";
#define mqtt_port 1883
#define MQTT_USER "vperhmxp"
#define MQTT_PASSWORD ""
#define MQTT_SERIAL_PUBLISH_CH "/dsh/damaso/reminders/requests"
#define MQTT_SERIAL_RECEIVER_CH "/dsh/damaso/reminders/notifications"


bool flagRequest = true;

//char json1 [] = " { \"Recordatorios\": [ { \"tiempo\": 5000 ,\"sonido\": 2}, { \"tiempo\": 10000 ,\"sonido\": 3} , { \"tiempo\": 7000 ,\"sonido\": 1} ] }";

WiFiClient wifiClient;

PubSubClient client(wifiClient);

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
           
      Serial.println("connected");
      //Once connected, publish an announcement...
      client.subscribe(MQTT_SERIAL_RECEIVER_CH);
      // ... and resubscribe 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte * payload, unsigned int length) {
    Serial.println("-------new message from broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");  
    Serial.write(payload, length);
    Serial.println();
    
    if( payload != NULL ) {
      int sonido = ((int)((char *)payload)[0])-48; 
      Serial.println(sonido);
      switch( sonido ) {
        case 0:
          myDFPlayer.play(4); 
          break;
        case 1:
          myDFPlayer.play(5); 
          break;
        case 2:
          myDFPlayer.play(6); 
          break;
        case 3:
          myDFPlayer.play(7); 
          break;
        case 7:
          myDFPlayer.play(8); 
          break;
        case 9:
          myDFPlayer.play(9); 
          break;
        default:
          break;    
      }    
    }
   
}

void setup() {
  Serial1.begin(9600, SERIAL_8N1, 25, 26);
  Serial.begin(115200);
  if (!myDFPlayer.begin(Serial1)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  myDFPlayer.volume(100);  //Set volume value (0~30)
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

  Serial.setTimeout(500);// Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback); 
  reconnect();
}


void loop() {
  if (!client.connected())
    reconnect();
  client.loop();  
}
