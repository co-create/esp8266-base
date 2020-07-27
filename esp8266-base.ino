#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// WiFi Constants
const char* ssid = "3easdkMrpz58kwq";
const char* password = "thisisthelastpasswordiwanttotype";

// Set up digital pins
const int trigPin = 5;
const int echoPin = 4;

long distanceSensorDuration = 0;
double distance = 0;

void setup () {
	Serial.begin(115200);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.print("Connecting..");
	}

 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
 Serial.begin(9600);
}

void loop() {
	if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
		HTTPClient http;  //Declare an object of class HTTPClient
		http.begin("http://54.153.13.255:8000/client/");  //Specify request destination
    getDistance();
		int httpCode = http.GET();                                                                  //Send the request
		if (httpCode > 0) { //Check the returning code
			String payload = http.getString();   //Get the request response payload
			Serial.println(payload);                     //Print the response payload
		  Serial.println(distance);
		}
		http.end();   //Close connection
	}
	delay(30000);    //Send a request every 30 seconds
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
