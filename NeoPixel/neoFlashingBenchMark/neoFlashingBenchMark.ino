#include <Wire.h>
#define I2C_SLAVE_ADDR  0x4
volatile boolean haveData = false;
volatile int value =0;

char chrPrint;
char charPrintOld;

char txChar;
int txIndex = 0;


void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)

}

void loop()
{
  int i=0;
  for (i=1;i<15;i++){
  txChar=0x10+i;
  
  Wire.beginTransmission(4); // transmit to device #44 (0x2c)
                             // device address is specified in datasheet
  Wire.write(txChar);         // sends value byte  
  Wire.endTransmission();     // stop transmitting
   delay(1000);
  }
}
