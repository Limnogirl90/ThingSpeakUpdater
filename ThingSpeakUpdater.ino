// ThingSpeak Updater

#include <ESP8266WiFi.h>

const char* ssid     = "dd-wrt";
const char* password = "bobmarley";
String writeAPIKey   = "H52CGJQNZRRW4FZH";

const char* thingSpeakAddress = "api.thingspeak.com";
const int updateThingSpeakInterval = 16*1000;

long lastConnectionTime = 0;
boolean lastConnected = false;
int failedCounter=0;

void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.print("\n\nConnecting to ");
  Serial.println(ssid); 
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected. IP address: "+WiFi.localIP());
}


void loop() {
  delay(5000);
  
  String analogValue0 = "100.00";//String(analogRead(A0), DEC);
  if ((WiFi.status() == WL_CONNECTED) && 
      (millis() - lastConnectionTime > updateThingSpeakInterval))
  {
    updateThingSpeak("field1="+analogValue0);
  }
}


void updateThingSpeak(String tsData)
{
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(thingSpeakAddress, httpPort)) {
    failedCounter++;
    Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");   
    Serial.println();
    
    lastConnectionTime = millis(); 
    return;
  }
  client.print("POST /update HTTP/1.1\n");
  client.print("Host: api.thingspeak.com\n");
  client.print("Connection: close\n");
  client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
  client.print("Content-Type: application/x-www-form-urlencoded\n");
  client.print("Content-Length: ");
  client.print(tsData.length());
  client.print("\n\n");

  client.print(tsData);
    
  lastConnectionTime = millis();
    
  if (client.connected())
  {
      delay(10);
      Serial.println("Connecting to ThingSpeak...");
      Serial.println();
      
      failedCounter = 0;
      while(client.available()){
        String line = client.readStringUntil('\r');
        Serial.print(line);
      }
  
      Serial.println();
      Serial.println("closing connection");
  }
  else
  {
      failedCounter++;
  
      Serial.println("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");   
      Serial.println();
  }
}
