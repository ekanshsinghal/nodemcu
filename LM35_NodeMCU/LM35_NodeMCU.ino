#include <ESP8266WiFi.h>

const int outputpin = A0;
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
float minimum = 100;
float maximum = 0;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT);
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi is connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  Serial.println("HTTP Server has started");
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  float analogValue = analogRead(outputpin);
  float celsius = (analogValue/3.1);
  if ( celsius > maximum) {
    maximum = celsius;
  }
  if ( celsius < minimum ) {
    minimum = celsius;
  }
  String message = "Temperature(in C) = ";
  message += celsius;
  message += "<br/><br/>Min = &nbsp;";
  message += minimum;
  message += "<br/><br/>Max = &nbsp;";
  message += maximum;
  client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><h1 style='font-size:72px'>");
  client.print(message);
  client.print("</h1></html>");
}
