#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <Stepper.h> 
#include <FS.h>
#include <WiFiClient.h>                         
#include <ArduinoJson.h>  

/*const char *ssid     = "ESPNet";
const char *password = "";*/
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
char blynk_token[33] = "192.168.1.1";
char static_ip[16] = "0.0.0.0";
char static_gw[16] = "0.0.0.0";
char static_sn[16] = "0.0.0.0";
NTPClient timeClient(ntpUDP, blynk_token, utcOffsetInSeconds);

//flag for saving data
bool shouldSaveConfig = false;

const char MAIN_page[] PROGMEM = R"=====(
<HTML>
	<HEAD>
			<TITLE>My first web page</TITLE>
	</HEAD>
<BODY>
	<CENTER>
			<B>Time settings </B>
	</CENTER>
  <p></p>	
</BODY>
</HTML>
)=====";
//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

ESP8266WebServer server(80);

void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  myStepper.setSpeed(60);

  //clean FS, for testing
  //SPIFFS.format();
  server.on("/", handleRoot);
  server.begin();                  //Start server
  Serial.println("HTTP server started");
  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_port, json["mqtt_port"]);
          strcpy(blynk_token, json["blynk_token"]);

          if(json["ip"]) {
            Serial.println("setting custom ip from config");
            strcpy(static_ip, json["ip"]);
            strcpy(static_gw, json["gateway"]);
            strcpy(static_sn, json["subnet"]);
            Serial.println(static_ip);
          } else {
            Serial.println("no custom ip in config");
          }
        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read
  Serial.println(static_ip);
  Serial.println(blynk_token);
  Serial.println(mqtt_server);


  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 5);
  WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token, 34);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //set static ip
  IPAddress _ip,_gw,_sn;
  _ip.fromString(static_ip);
  _gw.fromString(static_gw);
  _sn.fromString(static_sn);

  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);
  
  //add all your parameters here
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_blynk_token);

  //reset settings - for testing
  //wifiManager.resetSettings();

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  wifiManager.setMinimumSignalQuality();
  
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("AutoConnectAP", "pass")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  Serial.println("Connected... )");

  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(blynk_token, custom_blynk_token.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
    json["blynk_token"] = blynk_token;

    json["ip"] = WiFi.localIP().toString();
    json["gateway"] = WiFi.gatewayIP().toString();
    json["subnet"] = WiFi.subnetMask().toString();

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.prettyPrintTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.subnetMask());
}

void otoceni(){
  probiha=true;
 Serial.print("sdfha");
  Serial.println("clockwise");
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
  server.handleClient();
}
