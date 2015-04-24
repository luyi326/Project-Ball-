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
  Serial.begin(9600);  // start serial for output
}

void loop()
{
 

  byte lb;
  Wire.requestFrom(I2C_SLAVE_ADDR,1); 
  if (Wire.available()) {
    //    hb=Wire.read();
    lb=Wire.read();
  }
  
  Serial.println(lb);
  char txStringChar[4] = {1, 8, 5, ':'};
  if (txIndex <= 4){
      txChar = char(txStringChar[txIndex]);
      txIndex++;
  }
  else {
   txIndex = 0;
  } 
  
  Wire.beginTransmission(4); // transmit to device #44 (0x2c)
                             // device address is specified in datasheet
  Wire.write(txChar);         // sends value byte  
  Wire.endTransmission();     // stop transmitting
 delay(20);
}
