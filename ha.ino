#include <WiFi.h>
#include <DHT.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>

#define DHTPIN 27 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastCheck;

// wifi
const char* ssid = "SK_WiFiGIGAF380";
const char* password = "1701021605";
WiFiClient client;

// thingSpeak 
unsigned long ChannelID = 1072715;
const char* WriteAPIKey = "WH9VT1Q9AIGQF9MQ";


void setup() {
  Serial.begin(115200);
  dht.begin();
  delay(10);
    
  initWiFi();
  ThingSpeak.begin(client);

  lastCheck = 0;
}

void loop() {
  float c_time = 60000 * 0.1;
  if(millis() - lastCheck > c_time) {
    static boolean data_state = true;
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    Serial.print("Temperature : ");
    Serial.print(temperature);
    Serial.println("C");
    Serial.print("Humidity : ");
    Serial.print(humidity);
    Serial.println("%");

    if(data_state == true){
      ThingSpeak.writeField(1072692, 1, temperature, WriteAPIKey);
      data_state = false;
    } else {
      ThingSpeak.writeField(1072692, 2, humidity, WriteAPIKey);
      data_state = true;
    }

    lastCheck = millis();
  }
 
  delay(15000);
}

void initWiFi(){
  Serial.println();
  Serial.println();
  Serial.println("Connectiong to ssid ...");
  // attempt to connect to WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println(".");
  }
  // 접속성공!
  Serial.println();
  Serial.println("Connected WiFi");
  //사용하는 ip출력
  Serial.println(WiFi.localIP());
  Serial.println();
}
