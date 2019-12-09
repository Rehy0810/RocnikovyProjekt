#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Stepper.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 5, 4, 2, 14);


bool probiha=false;

char hodiny[4]={
 11, 12, 21, 22 
  };
const long utcOffsetInSeconds = 3600;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup(){
  Serial.begin(9600);
   WiFiManager wifiManager;
  wifiManager.autoConnect("ArduinoESP.");
  Serial.println("connected...");
  timeClient.begin();
  myStepper.setSpeed(60);
}

void loop() {
  
  if(millis()%100 == 0){
    timeClient.update();
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  }
  for(int i=0;i<3;i++){
  if((timeClient.getHours()==hodiny[i] && timeClient.getMinutes() == 10 && timeClient.getSeconds() <= 50) != probiha){
    Serial.print("Rehy");
    otoceni();
  }
    
  }
}

void otoceni(){
 probiha=true;
 Serial.print("sdfha");
  Serial.println("clockwise");
  myStepper.step(stepsPerRevolution);
 probiha=false;

}
