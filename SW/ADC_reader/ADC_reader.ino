#include <Wire.h>
#include <MCP342x.h>

// 0x68 is the default address for all MCP342x devices
uint8_t address1 = 0xA0;
MCP342x adc1 = MCP342x(address1);
uint8_t address2 = 0xA3;
MCP342x adc2 = MCP342x(address2);

float H2ppm=0.2; // 0.2mV/ppm 

/*
ADC1:
CH1 CH4 REF PIN
CH2 CH4 GAS PIN

ADC2:
CH1 OUTPUT WITH THE REFERENCE ON THE - SIDE
CH2 REFERENCE MEASURED FROM GND

*/


void list_devices()
{
  Serial.println("The MCP3422 and MCP3426 use I2C address 0x68, all other devices can be");
  Serial.println("configured to use any address in the range 0x68 - 0x6F (inclusive).");
  Serial.println("Be aware that the DS1307 uses address 0x68.");
  Serial.println();
  
  for (uint8_t add = 0X0; add < 0X80; add++) {
    //Serial.print("Trying ");
    //Serial.println(add);
    Wire.requestFrom(add, (uint8_t)1);
    if (Wire.available()) {
      Serial.print("Found device at: 0x");
      Serial.println(add, HEX);
    }
  }
  Serial.println("Done");
}

void setup(void)
{
  Serial.begin(9600);
  Wire.begin();
  list_devices();

  // Enable power for MCP342x (needed for FL100 shield only)
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);

  // Reset devices
  MCP342x::generalCallReset();
  delay(1); // MC342x needs 300us to settle, wait 1ms

  // Check device present
  Wire.requestFrom(address1, (uint8_t)1);
  if (!Wire.available()) {
    Serial.print("No device found at address ");
    Serial.println(address1, HEX);
    while (1)
      ;
  }

}

void loop(void)
{
  long value1 = 0;
  long value2 = 0;
  long value3 = 0;
  long value4 = 0;
    uint8_t err1,err2;

  MCP342x::Config status;

  // Initiate a conversion; convertAndRead() will wait until it can be read
   err1 = adc1.convertAndRead(MCP342x::channel1, MCP342x::oneShot,
                                    MCP342x::resolution18, MCP342x::gain1,
                                    1000000, value1, status);



  // Initiate a conversion; convertAndRead() will wait until it can be read
   err2 = adc2.convertAndRead(MCP342x::channel1, MCP342x::oneShot,
                                    MCP342x::resolution18, MCP342x::gain1,
                                    1000000, value3, status);

  if (err1)
  {
    Serial.print("Convert error: ");
    Serial.println(err1);
  }
  else
  {
    Serial.print("Value1: ");
    Serial.println(value1);
  }

  
    if (err2)
  {
    Serial.print("Convert error: ");
    Serial.println(err2);
  }
  else
  {
    Serial.print("Value3: ");
    Serial.println(value3);
  }

  delay(10);
//////////////////////////////////////////
  // Initiate a conversion; convertAndRead() will wait until it can be read
   err1 = adc1.convertAndRead(MCP342x::channel2, MCP342x::oneShot,
                                    MCP342x::resolution18, MCP342x::gain1,
                                    1000000, value2, status);



  // Initiate a conversion; convertAndRead() will wait until it can be read
   err2 = adc2.convertAndRead(MCP342x::channel2, MCP342x::oneShot,
                                    MCP342x::resolution18, MCP342x::gain1,
                                    1000000, value4, status);

  if (err1)
  {
    Serial.print("Convert error: ");
    Serial.println(err1);
  }
  else
  {
    Serial.print("Value1: ");
    Serial.println(value2);
  }

  
    if (err2)
  {
    Serial.print("Convert error: ");
    Serial.println(err2);
  }
  else
  {
    Serial.print("Value3: ");
    Serial.println(value4);
  }




  
  delay(10);
  delay(1000);
}


