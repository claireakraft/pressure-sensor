#include "USBSerial.h"
#include "mbed.h"
#include <I2C.h>
#include <math.h>


USBSerial SERIAL;
Thread thread1;

char Radd = 0xB9;
char Wadd = 0xB8;

DigitalOut TurnOn(p32);  // i2c
DigitalOut TurnOn2(p22); // port 22

I2C i2c(p14, p15);

void pressure() {
    char low[1], med[1], high[1], input1[2];

    input1[0] = 0x10;
    input1[1] = 0x10;
    SERIAL.printf("the input is %d\r\n", input1[1]);
    if (i2c.write(Wadd, (const char*)input1, 2, false) != 0){
        SERIAL.printf("Failed to update\r\n");
    }

  while (true) {

    low[0] = 0x28;
    med[0] = 0x29;
    high[0] = 0x2A;
    
    i2c.write(Wadd, low, 1, true);
    i2c.read(Radd, low, 1, false);
    //SERIAL.printf("the low bit %i\r\n", low[0]);

    i2c.write(Wadd, med, 1, true);
    i2c.read(Radd, med, 1, false);
    //SERIAL.printf("the med bit %i\r\n", med[0]);

    i2c.write(Wadd, high, 1, true);
    i2c.read(Radd, high, 1, false);
    //SERIAL.printf("the high bit %i\r\n", high[0]);

    int twos = (high[0] << 16) + (med[0] << 8) + low[0];
    //SERIAL.printf("the combined number is %i\r\n", twos);

    float Pressure = (twos*100)/4096.0;
    int Pressure1 = Pressure;
    SERIAL.printf("Relative pressure is %i Pa.\r\n", Pressure1);

    float PressureH = Pressure / 3386.389;
    int PressureH1 = PressureH;
    //SERIAL.printf("the pressure is %i inHg\r\n", PressureH1);

    float pFloat     = PressureH / 1.0;
    uint16_t pWhole  = PressureH / 1;
    uint8_t pDecimal = (pFloat - (pWhole)) * 100.0;
    //pLight    = 1;
    //SERIAL.printf("pdecimal is %i", pDecimal);
    float test = PressureH *100.0;
    int test1 = test;
    SERIAL.printf("Relative pressure is %i.%i inHg.\r\n\r\n", pWhole, pDecimal);


    thread_sleep_for(1000);

  }
}

// main() runs in its own thread in the OS
int main() {

  TurnOn = 1;
  TurnOn2 = 1;
  

  char value[1];
  char out[1];
  value[0] = 0x0F;
  out[0] = 0x00;
  thread_sleep_for(10);
  uint8_t response = i2c.write(Wadd, value, 1, true);
  //   SERIAL.printf("Response was %i\r\n", response);
  uint8_t response2 = i2c.read(Radd, out, 1, false);
  SERIAL.printf("Response was %i and %i\r\n", response, response2);
  if (out[0] == 0xB1) {
    SERIAL.printf("RIGHT sensor\r\n");
  } else {
    SERIAL.printf("WRONG sensor\r\n");
  }


  thread1.start(pressure);

    while (true) {
        thread_sleep_for(1000);
    }
}
