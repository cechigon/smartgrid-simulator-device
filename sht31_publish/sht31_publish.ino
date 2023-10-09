#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>

#include "ssid_define.h"

#include <Wire.h>
#include "AE_SHT31.h"

// WiFi
const char ssid[] = MY_SSID;
const char passwd[] = MY_SSID_PASS;

// Pub/Sub
const char* mqttHost = MQTT_HOST;  // MQTTのIPかホスト名
const int mqttPort = 1883;        // MQTTのポート
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

char* topic;  // 送信先のトピック名
char* payload;                // 送信するデータ

// SHT31のアドレスを設定
AE_SHT31 SHT31 = AE_SHT31(0x45);

void setup() {
  Serial.begin(115200);

  // Connect WiFi
  connectWiFi();

  // Connect MQTT
  connectMqtt();

  // SHT31をソフトリセット
  SHT31.SoftReset();
  // 内蔵ヒーター 0:OFF 1:ON
  SHT31.Heater(0);
}

void loop()
{
  // SHT31から温湿度データを取得
  SHT31.GetTempHum();

  Serial.println("--------------------------");

  topic = "esp32/temperature";
  char tmp[4];
  dtostrf(SHT31.Temperature(), 3, 1, tmp);
  mqttClient.publish(topic, tmp);
  Serial.print("Temperature ('C) : ");
  Serial.println(tmp);

  topic = "esp32/humidity";
  char hum[5];
  dtostrf(SHT31.Humidity(), 4, 2, hum);
  mqttClient.publish(topic, hum);
  Serial.print("Humidity (%) : ");
  Serial.println(hum);

  delay(800);

  // WiFi
  if (WiFi.status() == WL_DISCONNECTED) {
    connectWiFi();
  }
  // MQTT
  if (!mqttClient.connected()) {
    connectMqtt();
  }

  mqttClient.loop();
}

void connectWiFi() {
  WiFi.begin(ssid, passwd);
  Serial.print("WiFi connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.print(" connected. ");
  Serial.println(WiFi.localIP());
}

void connectMqtt() {
  mqttClient.setServer(mqttHost, mqttPort);
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    String clientId = "ESP32-" + String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
    }
    delay(1000);
    randomSeed(micros());
  }
}
