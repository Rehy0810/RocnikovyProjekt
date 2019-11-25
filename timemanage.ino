#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Stepper.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>


//const char *ssid     = "ESPNet";
//const char *password = "";
char hodiny[4]={
  13, 12, 15, 18 
  };
const long utcOffsetInSeconds = 3600;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "192.168.1.1", utcOffsetInSeconds);

void setup(){
  Serial.begin(9600);
   WiFiManager wifiManager;
  wifiManager.autoConnect("ArduinoESP.");
  Serial.println("connected...");
  timeClient.begin();
}

void loop() {
  timeClient.update();
  
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());

  for(int i=0;i<3;i++){
  if(timeClient.getHours()==hodiny[i] && timeClient.getMinutes() == 55 && timeClient.getSeconds() == 01)
    Serial.print("Rehy");
  }
}

void otoceni(){
  
}
