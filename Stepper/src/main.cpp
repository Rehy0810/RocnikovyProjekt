#include <Arduino.h>
#include <FS.h>                   
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Stepper.h>

const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 5, 4, 2, 14);
bool probiha= false;

char hodiny[4]={
 10, 11, 21, 22 
  };

const long utcOffsetInSeconds = 3600;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

WiFiUDP ntpUDP;

char mqtt_server[40];
char mqtt_port[6] = "8080";
char ntp_server[33] = "";
char static_ip[16] = "0.0.0.0";
char static_gw[16] = "0.0.0.0";
char static_sn[16] = "0.0.0.0";
NTPClient timeClient(ntpUDP, ntp_server, utcOffsetInSeconds);
bool shouldSaveConfig = false;
//Ukládání konfigurace
void saveConfigCallback () {
  Serial.println("Uložení konfigurace");
  shouldSaveConfig = true;
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  myStepper.setSpeed(60);
  //čtení konfigurace z FS json
  Serial.println("konfigurace FS json...");

  if (SPIFFS.begin()) {
    Serial.println("Připojování konfiguračního souboru");
    if (SPIFFS.exists("/config.json")) {
      //soubor existuje, čtení a načítání
      Serial.println("Načítání konfiguračního souboru");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        size_t size = configFile.size();
        // Alokace bufferu
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_port, json["mqtt_port"]);
          strcpy(ntp_server, json["ntp_server"]);

          if(json["ip"]) {
            Serial.println("Nastavování ip adresy ze souboru config.json");
            strcpy(static_ip, json["ip"]);
            strcpy(static_gw, json["gateway"]);
            strcpy(static_sn, json["subnet"]);
            Serial.println(static_ip);
          } else {
            Serial.println("Nenalezená žádná ip adresa v config.json ");
          }
        } else {
          Serial.println("Nepodařilo se načíst config.json");
        }
      }
    }
  } else {
    Serial.println("Nepodařilo se připojit ke config.json");
  }
  
  Serial.println(static_ip);
  Serial.println(ntp_server);
  Serial.println(mqtt_server);


  // Parametry navíc, které mají být nakonfigurovány
  // Po připojení dostaneme parametr .getValue ()
  WiFiManagerParameter custom_mqtt_server("server", "Zadejte adresu mqtt serveru", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "Zadejte mqtt port", mqtt_port, 5);
  WiFiManagerParameter custom_ntp_server("ntp", "Zadejte adresu NTP serveru", ntp_server, 34);

  //WiFiManager
  WiFiManager wifiManager;

  //Uložení nastavení
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //Nastavení statické ip adresy
  IPAddress _ip,_gw,_sn;
  _ip.fromString(static_ip);
  _gw.fromString(static_gw);
  _sn.fromString(static_sn);

  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);
  
  //Přidání parametrů sítě
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_ntp_server);

  wifiManager.resetSettings();
  wifiManager.setMinimumSignalQuality();
  
  wifiManager.setTimeout(120);

  //Nastavení automatického připojení k síti
  if (!wifiManager.autoConnect("AutoConnectAP", "pass")) {
    Serial.println("Připojení se nezdařilo");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  Serial.println("Připojeno... )");

  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(ntp_server, custom_ntp_server.getValue());

  if (shouldSaveConfig) {
    Serial.println("Ukládání nastavení");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
    json["ntp_server"] = ntp_server;

    json["ip"] = WiFi.localIP().toString();
    json["gateway"] = WiFi.gatewayIP().toString();
    json["subnet"] = WiFi.subnetMask().toString();

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("...");
    }

    json.prettyPrintTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

  Serial.println("Lokální ip adresa");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.subnetMask());
}

void otoceni(){
  probiha=true;
 Serial.print("start");
  Serial.println("Otáčení");
  myStepper.step(stepsPerRevolution);
 probiha=false;
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
    if((timeClient.getHours()==hodiny[i] && timeClient.getMinutes() == 13 && timeClient.getSeconds() <= 50) != probiha){
      Serial.print("Pokus");
      otoceni();
    }
  }
}
