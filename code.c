#include <ESP8266WiFi.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_BMP280.h>1

#define DHTPIN 14
#define DHTTYPE DHT11
#define RAINPIN A0
#define BMP280_ADDRESS 0x76

char ssid[] = "Snehal";
char pass[] = "12345678";
String host = "api.thingspeak.com";
String apiKey = "0AKITM6M04VBI07Q";

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);

  dht.begin();
  bmp.begin(BMP280_ADDRESS);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
    }
    Serial.println("Connected to WiFi");
  }

  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  int rainStatus = digitalRead(RAINPIN);
  float pressure = bmp.readPressure() / 100.0;

  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.println("C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  Serial.print("Rain Status: ");
  Serial.println(rainStatus == HIGH ? "Wet" : "Dry");

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println("hPa");

  // Send data to ThingSpeak
  sendToThingSpeak(temp, humidity, rainStatus, pressure);

  delay(6000);  // Send data every 60 seconds
}

void sendToThingSpeak(float temp, float humidity, int rainStatus, float pressure) {
  WiFiClient client;

  String data = "field1=" + String(temp) + "&field2=" + String(humidity) + "&field3=" + String(rainStatus) + "&field4=" + String(pressure);

  String request = "POST /update HTTP/1.1\r\n";
  request += "Host: " + host + "\r\n";
  request += "Connection: close\r\n";
  request += "X-THINGSPEAKAPIKEY: " + apiKey + "\r\n";
  request += "Content-Type: application/x-www-form-urlencoded\r\n";
  request += "Content-Length: " + String(data.length()) + "\r\n";
  request += "\r\n";
  request += data;

  if (client.connect(host.c_str(), 80)) {
    client.print(request);
    delay(1000);  // Allow time for server response
    client.stop();
  } else {
    Serial.println("Connection to ThingSpeak failed.");
  }
}
