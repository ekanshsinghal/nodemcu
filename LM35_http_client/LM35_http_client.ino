#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

float temp_celsius = 0;

void setup(){
  Serial.begin(115200);
  pinMode(A0, INPUT);
  Serial.print("Connecting to WiFi");
  WiFi.begin("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi is connected");
  Serial.println(WiFi.localIP());
}

void loop() {
  if(WiFi.status() == WL_CONNECTED) {
    StaticJsonDocument<300> JSONbuffer;
    JsonObject JSONencoder = JSONbuffer.createNestedObject(); 
    JSONencoder["Temperature"] = (analogRead(A0) / 1024.0) *330.0;
    
    char JSONmessageBuffer[300];
    
    serializeJsonPretty(JSONencoder, JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
 
    HTTPClient http;

    http.begin("http://192.168.100.161:8090/post");
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(JSONmessageBuffer);
    String payload = http.getString();

    Serial.println(httpCode);
    Serial.println(payload);

    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
  delay(10000);
}
