#include <WiFi.h>
#include <PubSubClient.h>
#include <MCP342X.h>

// Replace with your network credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Replace with your MQTT broker IP address or hostname
const char* mqtt_server = "your_MQTT_broker";

// Replace with your MQTT topic
const char* topic = "your_topic";

// Replace with your ADC addresses
const uint8_t adc1_address = 0x69;
const uint8_t adc2_address = 0x6C;

// Replace with your ADC channel and gain settings
const uint8_t channel = 1;
const uint8_t gain = GAIN_1X;
const uint8_t bits = BIT_18;

// Initialize the WiFi and MQTT clients
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Initialize the ADCs
MCP342X adc1(adc1_address);
MCP342X adc2(adc2_address);

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Connect to MQTT broker
  mqttClient.setServer(mqtt_server, 1883);
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32-s3-client")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.println("Failed to connect to MQTT broker");
      delay(1000);
    }
  }

  // Initialize the ADCs
  adc1.begin(channel, gain, bits);
  adc2.begin(channel, gain, bits);

  // Set the ADC conversion mode
  adc1.setConversionMode(ONE_SHOT);
  adc2.setConversionMode(ONE_SHOT);
}

void loop() {
  // Read the ADC values
  float value1 = adc1.readADC();
  float value2 = adc2.readADC();

  // Print the ADC values to the Serial Monitor
  Serial.print("ADC1: ");
  Serial.println(value1);
  Serial.print("ADC2: ");
  Serial.println(value2);

  // Publish the values to the MQTT broker
  char payload[32];
  snprintf(payload, sizeof(payload), "%.2f,%.2f", value1, value2);
  mqttClient.publish(topic, payload);

  // Wait for 1 second before reading the values again
  delay(1000);
}