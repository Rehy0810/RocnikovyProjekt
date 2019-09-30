Automatický dávkovač granulí pro psy řízený mikročipem ESP 8266

Ilustrační video(https://www.thingiverse.com/thing:27854/apps/print/)

Součásti projektu
- https://arduino-shop.cz/arduino/1457-esp8266-esp-12e-ota-wemos-d1-ch340-wifi-arduino-ide-uno-r3.html?gclid=EAIaIQobChMI3buOzebF5AIVROh3Ch25xAU7EAQYBCABEgJTtPD_BwE
- https://arduino-shop.cz/arduino/7317-lafvin-arduino-kit-elektro-soucastky-l003.html
- https://arduino-shop.cz/arduino/832-arduino-krokovy-motor-driver.html?gclid=EAIaIQobChMIioOi367X5AIVgrTtCh2qdQJ0EAQYASABEgKkwPD_BwE
- šnekový podavač vymodelovaný v 3ds max a vytisknutý na 3d tiskárně
-nádoba na granule

Rozvržení práce
1. nakoupit díly a začít se stavbou
2. připojit k arduinu a začít se softwarem
3. dokončení softwaru a celkově doladit projekt
4. zkompletování dokumentace

Schéma zapojení krokového motoru přes adaptér do ESP
https://navody.arduino-shop.cz/navody-k-produktum/krokovy-motor-a-driver.html

_______________________________________________________________________________________________________________________________
Září-rozdělaná stavba boxu, modelování šnekového podavače v programu 3ds Max



Zkušební kód pro krokový motor na pojený přes adaptér

#include <Stepper.h>

const int stepsPerRevolution = 200; 

Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

int stepCount = 0;  

void setup() {
}
void loop() {  
  int sensorReading = analogRead(A0);
  
  int motorSpeed = map(sensorReading, 0, 1023, 0, 100);
  
  if (motorSpeed > 0) {
    myStepper.setSpeed(motorSpeed);
    myStepper.step(stepsPerRevolution / 100);
  }
}



  Kód pro připojeni ESP8266 k WIFI
  
  #include <ESP8266WiFi.h>

void setup()
{
  Serial.begin(115200);
  
  Serial.println();

  WiFi.begin("network-name", "pass-to-network");

  Serial.print("Connecting");
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500); 
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  
  Serial.println(WiFi.localIP());
}

void loop() {}
