#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Stepper.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>


//const char *ssid     = "ESPNet";
//const char *password = "";

const long utcOffsetInSeconds = 3600;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "192.168.1.1", utcOffsetInSeconds);

void setup(){
  Serial.begin(115200);
   WiFiManager wifiManager;
  wifiManager.autoConnect("ArduinoESP.");

  //if you get here you have connected to the WiFi
  Serial.println("connected...");
  /*WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 ); 
    Serial.print ( "." );
  }*/

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
  delay(1000);
}
