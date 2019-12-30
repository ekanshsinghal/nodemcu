#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <WiFiClient.h>

const char* kSsid = "YOUR_WIFI_SSID";
const char* kPassword = "YOUR_WIFI_PASSWORD";
MDNSResponder mdns;

ESP8266WebServer server(80);
#undef HOSTNAME
#define HOSTNAME "esp8266"

const uint16_t kIrLed = 4;  //(D2).

IRsend irsend(kIrLed);

void handleRoot() {
  server.send(200, "text/html",
    "<html>" \
      "<head><title>" HOSTNAME " Demo</title></head>" \
      "<body>" \
        "<h1>Hello from " HOSTNAME ", you can send NEC encoded IR signals from here!</h1>" \
        "<p><a href=\"ir?code=0x540A\">Send POWER</a></p>" \
        "<p><a href=\"ir?code=0x240A\">Send VOL+</a></p>" \
        "<p><a href=\"ir?code=0x640A\">Send VOL-</a></p>" \
      "</body>" \
    "</html>");
}

void handleIr() {
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "code") {
      uint32_t code = strtoul(server.arg(i).c_str(), NULL, 16);
      Serial.println(code);
      #if SEND_NEC
      irsend.sendSony(code, 15);
      #endif
    }
  }
  handleRoot();
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

void setup(void) {
  irsend.begin();

  Serial.begin(115200);
  WiFi.begin(kSsid, kPassword);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(kSsid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP().toString());

  if (mdns.begin(HOSTNAME, WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/ir", handleIr);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
