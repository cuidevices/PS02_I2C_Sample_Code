#include <Wire.h>

//I²C address for PS02 sensor
const int address = 0x6D;

//PS02 sensors are available in a number of different pressure ranges.
//In this example, we use a 0-250kPa sensor.
const float max_pressure_kPa = 250;

//Convert raw 24-bit value to pressure in kPa
float to_pressure(uint32_t x) {
  return max_pressure_kPa / 0.8f * (x/8388608.0f - 0.1f);
}

//Convert raw 24-bit value to temperature in °C
float to_temperature(uint32_t tx) {
  if(tx < 8388608) {
    return tx/65536.0f + 25.0f;
  }
  else {
    return (tx - 16777216.0f)/65536.0f + 25.0f;
  }
}

void setup() {
  //Default I2C clock speed of 100kHz is fine
  Wire.begin();

  //Enable serial logging so that we can send pressure & temperature measurements to PC
  Serial.begin(115200);
}

void loop() {
  //To get the pressure measurement, we must first "write" the register address to the I²C bus
  Wire.beginTransmission(address);
  Wire.write(0x06);
  Wire.endTransmission();

  //Now read 3 bytes (24 bits) and combine them into a single value
  Wire.requestFrom(address, 3);
  uint32_t pressure = 0;
  while(Wire.available())
  {
    pressure = (pressure << 8) | Wire.read();
  }

  //Send the pressure value to the PC over serial, converting it from a raw 24-bit value to the kPa measurement
  Serial.print("Pressure: ");
  Serial.print(to_pressure(pressure));
  Serial.println(" kPa");

  //To get the temperature measurement, we must first "write" the register address to the I²C bus
  Wire.beginTransmission(address);
  Wire.write(0x09);
  Wire.endTransmission();
  
  //Now read 3 bytes (24 bits) and combine them into a single value
  Wire.requestFrom(address, 3);
  uint32_t temperature = 0;
  while(Wire.available())
  {
    temperature = (temperature << 8) | Wire.read();
  }

  //Send the temperature value to the PC over serial, converting it from a raw 24-bit value to the °C measurement
  Serial.print("Temperature: ");
  Serial.print(to_temperature(temperature));
  Serial.println(" °C");

  //Wait 1 second before taking another measurement
  delay(1000);
}
