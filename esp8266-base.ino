/**
   BasicHTTPSClientNofingerprint.ino
   1.03.2019
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <string>

// Fingerprint for demo URL, expires on June 2, 2019, needs to be updated well before this date
const uint8_t fingerprint[33] = {0x01, 0x7D, 0xC4, 0xAB, 0x36, 0x0F, 0xD2, 0x4E, 0x4F, 0xE4, 0xD6, 0x58, 0x3A, 0x0B, 0x23, 0xBD, 0xE8, 0xB4, 0x19, 0xFE, 0x6A, 0x7E, 0xF0, 0xDA, 0xB6, 0xE6, 0x6D, 0x9B, 0x6D, 0xAF, 0xD0, 0x17};
ESP8266WiFiMulti WiFiMulti;

// DIGITAL PINS ESP8266 NODE MCU TEST
//const int trigPin = 5;
//const int echoPin = 4;

// DIGITAL PINS ESP01 PROD
const int trigPin = 0;
const int echoPin = 2;

long distanceSensorDuration = 0;
double distance = 0;
String mac = "";
String distanceStr = "";

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
  //WiFiMulti.addAP("byk_phone", "harrypotter2020");

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  mac = WiFi.macAddress();
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

      String data = "{\"sensor\":\"distance\", \"data\":" + distanceStr + ", \"mac\":\"" + mac + "\"}";
      Serial.print(data);
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
  delay(1000);
}

void getDistance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  distanceSensorDuration = pulseIn(echoPin, HIGH);
  distance = distanceSensorDuration * 0.0135 / 2;
  distanceStr = String(distanceSensorDuration * 0.0135 / 2, 10);
  Serial.println(distance);
  Serial.println(distanceStr);
}
