#include <Wire.h>

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

void setup()
{
    Wire.begin(5, 6); // SDA = GPIO5, SCL = GPIO6
    Serial.begin(9600);
    while (!Serial);
    Serial.println("setup complete");
    pinMode(8, OUTPUT);

}

void loop()
{
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
