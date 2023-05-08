#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>


const char* ssid = "LANCOMBEIA";
const char* password = "beialancom";
const char* mqtt_server = "mqtt.beia-telemetrie.ro";
// training/esp32s3/MB

WiFiClient espClient;
PubSubClient client(espClient);

float readADC(int address, int channel)
{
  Serial.println("ADC function begin");
    // Construct command byte for MCP342x ADC
    byte cmd = B11000000;
    if (channel == 0) {
        cmd |= B00000000;
    } else if (channel == 1) {
        cmd |= B00010000;
    } else if (channel == 2) {
        cmd |= B00100000;
    } else if (channel == 3) {
        cmd |= B00110000;
    }
    // Send command byte to MCP342x ADC
    Wire.beginTransmission(address);
    Wire.write(cmd);
    Wire.endTransmission();
    // Wait for conversion to complete
    delay(100);
    // Read conversion result from MCP342x ADC
    Wire.requestFrom(address, 3);
    byte msb = Wire.read();
    byte lsb = Wire.read();
    byte signbit = msb & B10000000;
    int result = ((msb & B00001111) << 8) | lsb;
    if (signbit) {
        result -= 4096;
    }
    // Calculate voltage from conversion result
    float voltage = (result * 2.048) / 32768.0;
    return voltage;
}


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

    Wire.begin(5, 6); // SDA = GPIO5, SCL = GPIO6
    pinMode(8, OUTPUT);
    Serial.println("setup complete");

}

void loop() {
  client.loop();
  client.publish("training/esp32s3/MB", "Main loop start");
  Serial.println("loop begining");
  digitalWrite(8, HIGH);
  delay(1000);
  digitalWrite(8, LOW);
  delay(1000);
    // Read all 4 channels of MCP342x ADC 1
    float ch0_1 = readADC(0x69, 0);
    float ch1_1 = readADC(0x69, 1);
    float ch2_1 = readADC(0x69, 2);
    float ch3_1 = readADC(0x69, 3);

    // Read all 4 channels of MCP342x ADC 2
    float ch0_2 = readADC(0x6C, 0);
    float ch1_2 = readADC(0x6C, 1);
    float ch2_2 = readADC(0x6C, 2);
    float ch3_2 = readADC(0x6C, 3);



    // Send data over MQTT
    char payload_ADC[50];
    snprintf(payload_ADC, 50, "%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f", ch0_1, ch1_1,ch2_1,ch3_1, ch0_2, ch1_2,ch2_2 ,ch3_2 );
 
    client.publish("training/esp32s3/MB", payload_ADC );
    // Print voltage readings to serial monitor
    Serial.print("ADC 1: CH0 = ");
    Serial.print(ch0_1);
    Serial.print(", CH1 = ");
    Serial.print(ch1_1);
    Serial.print(", CH2 = ");
    Serial.print(ch2_1);
    Serial.print(", CH3 = ");
    Serial.println(ch3_1);

    Serial.print("ADC 2: CH0 = ");
    Serial.print(ch0_2);
    Serial.print(", CH1 = ");
    Serial.print(ch1_2);
    Serial.print(", CH2 = ");
    Serial.print(ch2_2);
    Serial.print(", CH3 = ");
    Serial.println(ch3_2);

    Serial.println("loop complete");

    delay(1000); // Wait 1 second before reading again


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
