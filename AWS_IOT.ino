#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
//#include <ArduinoJson.h>
#include <DHT.h>
#include <WiFi.h>

#define AWS_IOT_PUBLISH_TOPIC1  "esp32/temp"
#define AWS_IOT_PUBLISH_TOPIC2  "esp32/humi"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

#define DHTPIN 27 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float Humidity;
float Temperature;

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler
  client.onMessage(messageHandler);

  Serial.print("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}


void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

}

void setup() {
  Serial.begin(115200);
  dht.begin();
  connectAWS();
}

void loop() {
  client.loop();
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();
 
  char tempString[8];
  
  dtostrf(Temperature, 1, 2, tempString);
  Serial.print("Temperature: ");
  Serial.println(tempString);
  
  client.publish(AWS_IOT_PUBLISH_TOPIC1, tempString);
  
  char humiString[8];
  
  dtostrf(Humidity, 1, 2, humiString);
  Serial.print("Humidity: ");
  Serial.println(humiString);
  
  client.publish(AWS_IOT_PUBLISH_TOPIC2, humiString);
  delay(2000);
}
