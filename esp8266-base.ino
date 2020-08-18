/**
   BasicHTTPSClientNofingerprint.ino
   1.03.2019
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

// Fingerprint for demo URL, expires on June 2, 2019, needs to be updated well before this date
const uint8_t fingerprint[33] = {0x01, 0x7D, 0xC4, 0xAB, 0x36, 0x0F, 0xD2, 0x4E, 0x4F, 0xE4, 0xD6, 0x58, 0x3A, 0x0B, 0x23, 0xBD, 0xE8, 0xB4, 0x19, 0xFE, 0x6A, 0x7E, 0xF0, 0xDA, 0xB6, 0xE6, 0x6D, 0x9B, 0x6D, 0xAF, 0xD0, 0x17};
ESP8266WiFiMulti WiFiMulti;

// Set up digital pins
const int trigPin = 5;
const int echoPin = 4;

long distanceSensorDuration = 0;
double distance = 0;

void setup() {
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  Serial.println();
  Serial.println();
  Serial.println();
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("3easdkMrpz58kwq", "thisisthelastpasswordiwanttotype");
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    //client->setFingerprint(fingerprint);
    client->setInsecure();
    HTTPClient https;
    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://dev.techgronomist.com/api/1/iotClient/")) {  // HTTPS
      Serial.print("[HTTPS] POST...\n");
      // start connection and send HTTP header
      getDistance();
      StaticJsonBuffer<69> jsonBuffer;
      char json[40];
      sprintf(json, "{'sensor':'distance', 'data':%lf}", distance);
      JsonObject& root = jsonBuffer.parseObject(json);
      String data;
      root.printTo(data);
      int httpCode = https.POST(data);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] POST... code: %d\n", httpCode);
        // file found at server
        if (httpCode == 200 || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
  Serial.println("Wait 5s before next round...");
  delay(5000);
}

void getDistance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  distanceSensorDuration = pulseIn(echoPin, HIGH);
  distance = distanceSensorDuration * 0.034 / 2;
  Serial.println(distance);
}
