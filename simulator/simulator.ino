#include <WiFi.h>
#include <PubSubClient.h>

#include "ssid_define.h"

// WiFi
const char ssid[] = MY_SSID;
const char passwd[] = MY_SSID_PASS;

// Pub/Sub
const char *mqttHost = MQTT_HOST; // MQTTのIPかホスト名
const int mqttPort = 1883;        // MQTTのポート
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const String device_type = "car";
const String device_id = "0";
const String device_topic = device_type + '/' + device_id + '/';

bool charge_flag = true;
bool use_flag = false;

bool charge_of_status = false;
double charge_of_battery = 4 * 1000; // Wh
double charge_of_max = 40 * 1000;    // Wh
double intput_power = 3 * 1000;      // Wh
double output_power = 3 * 1000;      // Wh

void setup()
{
  Serial.begin(115200);

  // Connect WiFi
  connectWiFi();

  // Connect MQTT
  connectMqtt();
}

void mqtt_publish(String target, long int_data)
{
  int len;

  String target_topic = device_topic + target;
  len = target_topic.length() + 1;
  char topic[len];
  target_topic.toCharArray(topic, len);

  String send_data = String(int_data);
  len = send_data.length() + 1;
  char payload[len];
  send_data.toCharArray(payload, len);

  mqttClient.publish(topic, payload);
}

void loop()
{

  if (charge_flag)
  {
    if (charge_of_max > charge_of_battery)
    {
      charge_of_battery += (intput_power / 3600);
    }
    mqtt_publish("charge-of-status", 1);
    mqtt_publish("input-power", round(intput_power));
  }
  else
  {
    mqtt_publish("charge-of-status", 0);
    mqtt_publish("input-power", 0);
  }

  if (use_flag)
  {
    if (0 < charge_of_battery)
    {
      charge_of_battery += (output_power / 3600);
    }
    mqtt_publish("output-power", round(output_power));
  }
  else
  {
    mqtt_publish("output-power", 0);
  }

  mqtt_publish("charge-of-battery", round(charge_of_battery));
  mqtt_publish("charge-of-max", round(charge_of_max));

  delay(1000);

  // WiFi
  if (WiFi.status() == WL_DISCONNECTED)
  {
    connectWiFi();
  }
  // MQTT
  if (!mqttClient.connected())
  {
    connectMqtt();
  }
  mqttClient.loop();
}

void connectWiFi()
{
  WiFi.begin(ssid, passwd);
  Serial.print("WiFi connecting...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.print(" connected. ");
  Serial.println(WiFi.localIP());
}

void connectMqtt()
{
  mqttClient.setServer(mqttHost, mqttPort);
  while (!mqttClient.connected())
  {
    Serial.println("Connecting to MQTT...");
    String clientId = "ESP32-" + String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str()))
    {
      Serial.println("connected");
    }
    delay(1000);
    randomSeed(micros());
  }
}
