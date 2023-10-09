#include <WiFi.h>
#include <PubSubClient.h>

#include "ssid_define.h"

// WiFi
const char ssid[] = MY_SSID;
const char passwd[] = MY_SSID_PASS;

// Pub/Sub
const char* mqttHost = MQTT_HOST;  // MQTTのIPかホスト名
const int mqttPort = 1883;        // MQTTのポート
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char* topic = "mytopic";  // 送信先のトピック名
char* payload;                // 送信するデータ

void setup() {
  Serial.begin(115200);

  // Connect WiFi
  connectWiFi();

  // Connect MQTT
  connectMqtt();
}

void loop() {

  // 送信処理 topic, payloadは適宜
  payload = "payload";
  mqttClient.publish(topic, payload);
  delay(5000);

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

/**
       Connect WiFi
*/
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
/**
       Connect MQTT
*/
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
