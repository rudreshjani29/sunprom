#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <DHT.h>
#include <BH1750.h>
#include <Wire.h>

const char* WIFI_SSID = "Humaapkehainkaun";  
const char* WIFI_PASS = "12345678"; 

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883  
#define AIO_USERNAME    "rudreshjani29"  
#define AIO_KEY         "aio_QaNy16NnjtU0dbaovpMKNK2uF9oB"  

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish tempFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish humFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish lightFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/light");

#define DHTPIN 4  
DHT dht(DHTPIN, DHT22);
BH1750 lightMeter;

void setup() {
  Serial.begin(115200);
  Serial.println("Timestamp,Temperature,Humidity,Light");  // CSV Header for Serial Logging

  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  connectToMQTT();

  Wire.begin();
  lightMeter.begin();
  dht.begin();
  Serial.println("Sensors initialized.");
}

void loop() {
  if (!mqtt.connected()) {
    connectToMQTT();
  }
  mqtt.processPackets(5000);  

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float lux = lightMeter.readLightLevel();

  if (!isnan(temperature) && !isnan(humidity) && !isnan(lux)) {
    Serial.print(temperature);
    Serial.print(",");
    Serial.print(lux);
    Serial.print(",");
    Serial.println(humidity);

    tempFeed.publish(temperature);
    humFeed.publish(humidity);
    lightFeed.publish(lux);
  } else {
    Serial.println("Sensor reading failed!");
  }

  delay(2000);  
}

void connectToMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Connecting to Adafruit IO...");
    if (mqtt.connect()) {
      Serial.println("Connected!");
    } else {
      Serial.print("Failed, retrying in 5 seconds. Error: ");
      Serial.println(mqtt.connectErrorString(mqtt.connect()));
      delay(5000);
    }
  }
}