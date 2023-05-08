#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "LANCOMBEIA";
const char* password = "beialancom";
const char* mqtt_server = "mqtt.beia-telemetrie.ro";
// training/esp32s3/MB

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  while (!client.connected()) {
    if (client.connect("ESP32-S3")) {
      Serial.println("Connected to MQTT broker");
      client.subscribe("test_topic");
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  client.loop();
  client.publish("training/esp32s3/MB", "Hello, world!");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message content: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
