// ESP8266 Web Server

// připojení potřebných knihoven
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// vytvoření proměnných s názvem WiFi sítě a heslem
const char* nazevWifi = "Rehackovi";
const char* hesloWifi = "rehackovi1234";

// incializace webserveru na portu 80
ESP8266WebServer server(80);

// propojovací pin indikační LED diody
#define LEDka 14

// podprogram s hlavní zprávou, která je vytištěna
// při zadání IP adresy do prohlížeče
void zpravaHlavni() {
  // zapnutí LED diody
  digitalWrite(LEDka, HIGH);
  // načtení hodnoty analogového pinu a času
  // od spuštění Arduina ve formátu String
  String analog = String(analogRead(A0));
  String cas = String(millis()/1000);
  // vytvoření zprávy, která bude vytištěna
  // v prohlížeči (\n znamená nový řádek)
  String zprava = "Ahoj Arduino svete!\n";
  zprava += "Analogovy pin A0: ";
  zprava += analog;
  zprava += "\nCas od spusteni Arduina je ";
  zprava += cas;
  zprava += " vterin.";
  // vytištění zprávy se statusem 200 - OK
  server.send(200, "text/plain", zprava);
  // vypnutí LED diody
  digitalWrite(LEDka, LOW);
}

// podprogram s chybovou zprávou, která je vytištěna
// při zadání IP adresy s neexistující podstránkou
void zpravaNeznamy() {
  // zapnutí LED diody
  digitalWrite(LEDka, HIGH);
  // vytvoření zprávy s informací o neexistujícím odkazu
  // včetně metody a zadaného argumentu
  String zprava = "Neexistujici odkaz\n\n";
  zprava += "URI: ";
  zprava += server.uri();
  zprava += "\nMetoda: ";
  zprava += (server.method() == HTTP_GET)?"GET":"POST";
  zprava += "\nArgumenty: ";
  zprava += server.args();
  zprava += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    zprava += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  // vytištění zprávy se statusem 404 - Nenalezeno
  server.send(404, "text/plain", zprava);
  // vypnutí LED diody
  digitalWrite(LEDka, LOW);
}

void setup(void) {
  // nastavení LED diody jako výstupní a její vypnutí
  pinMode(LEDka, OUTPUT);
  digitalWrite(LEDka, LOW);
  // zahájení komunikace po sériové lince
  Serial.begin(9600);
  // zahájení komunikace po WiFi s připojením
  // na router skrze zadané přihl. údaje
  WiFi.begin(nazevWifi, hesloWifi);
  // čekání na úspěšné připojení k routeru,
  // v průběhu čekání se vytiskne každých
  // 500 milisekund tečka po sériové lince
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // odřádkování a výpis informací o úspěšném připojení
  // včetně přidelené IP adresy od routeru
  Serial.println("");
  Serial.print("Pripojeno k WiFi siti ");
  Serial.println(nazevWifi);
  Serial.print("IP adresa: ");
  Serial.println(WiFi.localIP());
  // kontrola funkčnosti MDNS
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder je zapnuty.");
  }
  // nastavení vytištění hlavní zprávy po přístupu
  // na samotnou IP adresu
  server.on("/", zpravaHlavni);
  // pokud chceme vytisknout pouze menší zprávy, není
  // nutné je vytvářet v podprogramech jako zpravaHlavni,
  // viz. ukázka níže
  
  // nastavení vytištění jiné zprávy po přístupu na
  // podstránku ukazka, tedy např. 10.0.0.31/ukazka
  server.on("/ukazka", []() {
    String zprava = "Ukazka odkazu pro vice stranek.";
    server.send(200, "text/plain", zprava);
  });
  // nastavení vytištění informací o neznámém
  // odkazu pomocí podprogramu zpravaNeznamy  
  server.onNotFound(zpravaNeznamy);
  // zahájení aktivity HTTP serveru
  server.begin();
  Serial.println("HTTP server je zapnuty.");
}

void loop(void) {
  // pravidelné volání detekce klienta,
  // v případě otevření stránky se provedou
  // funkce nastavené výše
  server.handleClient();
  delay(10);
}
