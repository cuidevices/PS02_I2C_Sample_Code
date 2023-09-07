# PS02 Arduino I<sup>2</sup>C Sample Code

## Table of Contents

- [Hardware](#hardware)
- [Software Required](#software)
- [PS02 Pressure Sensors](#ps02-pressure-sensors)
- [Circuit](#circuit)
- [PS02 Registers](#ps02-registers)
- [Data Acquisition](#data-acquisition)
- [Data Conversion](#data-conversion)

## Hardware

- [Arduino Mega](https://docs.arduino.cc/hardware/mega-2560)
- [PS02 Pressure Sensor](https://www.cuidevices.com/product/sensors/pressure-sensors/ps02-series)

## Software

- [Arduino IDE](https://www.arduino.cc/en/Main/Software)
- [PS02 Arduino Sample Code](https://github.com/cuidevices/PS02_I2C_Sample_Code)

## PS02 Pressure Sensors

The PS02 is a combined pressure & temperature sensor that communicates over an I<sup>2</sup>C interface. The pressure sensor is available in either absolute pressure or gauge pressure variants.

These sensors are powered by 3.3V. The I<sup>2</sup>C interface operates at a clock speed of 100 kHz - 400 kHz. The I<sup>2</sup>C address is 0x6D.

## Circuit

The following connections are for the [Arduino Mega board](https://docs.arduino.cc/hardware/mega-2560), however, most all Arduino boards will be compatible with this code. The connections to various Arduino boards will be different than this example, so consult Arduino documentation for additional information.

| Function | PS02 Color | Arduino Mega Pin |
| -------- | ---------- | ---------------- |
|   +3.3V  |     Red    |       3.3V       |
|  Ground  |    Blue    |        GND       |
|    SCL   |   Yellow   |        21        |
|    SDA   |    White   |        20        |

## PS02 Registers

The pressure and temperature values are stored in 24-bit (3-byte) registers on the PS02.

|   Contents  | Register Address |
| ----------- | ---------------- |
|   Pressure  |       0x06       |
| Temperature |       0x09       |

## Data Acquisition

To read data from the PS02, first perform an I<sup>2</sup>C write of a single byte containing the register address. For example, to read from the pressure register, write 0x06 to address 0x6D.

Once the register address has been set, perform a 3-byte read. The data is read MSB first.

```c
Wire.beginTransmission(address);
Wire.write(0x06);
Wire.endTransmission();

Wire.requestFrom(address, 3);
uint32_t pressure = 0;
while(Wire.available())
{
  pressure = (pressure << 8) | Wire.read();
}
```

## Data Conversion

The raw 24-bit values can be converted to standard units (kPa for pressure, °C for temperature) with the following equations:

$$
Pressure (kPa) = pressure_{max} / 0.8 \cdot (x / 2^{23} - 0.1)
$$

$$
Temperature (°C) =
\begin{cases}
x/2^{16} + 25,  & x < 2^{23} \\
\frac{x - 2^{24}}{2^{16}} + 25, & x >= 2^{23}
\end{cases}
$$

```c
float to_pressure(uint32_t x) {
  return max_pressure_kPa / 0.8f * (x/8388608.0f - 0.1f);
}

float to_temperature(uint32_t tx) {
  if(tx < 8388608) {
    return tx/65536.0f + 25.0f;
  }
  else {
    return (tx - 16777216.0f)/65536.0f + 25.0f;
  }
}
```