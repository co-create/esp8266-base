#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
 
//Wi-Fi settings
#define WIFI_SSID "3easdkMrpz58kwq"
#define WIFI_PASSWORD "thisisthelastpasswordiwanttotype"

const char* ssid = "3easdkMrpz58kwq";
const char* password = "thisisthelastpasswordiwanttotype";
 
void setup () {
 
Serial.begin(115200);
WiFi.begin(ssid, password);
 
while (WiFi.status() != WL_CONNECTED) {
 
delay(1000);
Serial.print("Connecting..");
 
}
 
}
 
void loop() {
 
if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
HTTPClient http;  //Declare an object of class HTTPClient
 
http.begin("http://54.153.13.255:8000/client/");  //Specify request destination
int httpCode = http.GET();                                                                  //Send the request
 
if (httpCode > 0) { //Check the returning code
 
String payload = http.getString();   //Get the request response payload
Serial.println(payload);                     //Print the response payload
 
}
 
http.end();   //Close connection
 
}
 
delay(30000);    //Send a request every 30 seconds
 
}