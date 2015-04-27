#define I2C_SLAVE_ADDRESS 0x4 // the 7-bit address (remember to change this when adapting this example)

// Get this from https://github.com/rambo/TinyWire
#include <TinyWireS.h>

// The default buffer size, Can't recall the scope of defines right now
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

volatile uint8_t i2c_regs[] =
{
    0xDE, 
    0xAD, 
    0xBE, 
    0xEF, 
};

//Character variable used to echo data back. 
char chrSendData;

//Variables used in getting and parsing data.
char rxChrData; //Receives the data.
char rxString[12];  //Varbiable for holding one string of data.
int rxIndex = 0; //Used to index rxString.


int pwmValA;

// Tracks the current register pointer position
volatile byte reg_position;

void requestEvent(){  

  TinyWireS.send(chrSendData);
}

/**
 * The I2C data received -handler
 *
 * This needs to complete before the next incoming transaction (start, data, restart/stop) on the bus does
 * so be quick, set flags for long running tasks to be called from the mainloop instead of running them directly,
 */
void receiveEvent(uint8_t howMany)
{
    if (TinyWireS.available()){  
      if (howMany < 1)
      {
          // Sanity-check
          return;
      }
      if (howMany > TWI_RX_BUFFER_SIZE)
      {
          // Also insane number
          return;
      }
  
      //rxChrData = TinyWireS.receive();
      howMany--;
      if (!howMany)
      {
          // This write was only to set the buffer for next read
          return;
      }
      while(howMany--)
      {
          rxChrData = TinyWireS.receive();
          rxString[rxIndex] = char(rxChrData);
          rxIndex++;  
          if (rxChrData == 0xff){
              
              //rxString 2-3 each holds the value you 9.
              //The lines below print "27" on the serial line.
              pwmValA = int(100*rxString[2]+10*rxString[3]+rxString[4]);
              Serial.println(pwmValA);
              analogWrite(1, pwmValA);
              rxIndex = 0;  
          }
          
      }
    }
}

void setup()
{
    //pinMode(3, OUTPUT); // OC1B-, Arduino pin 3, ADC
    //digitalWrite(3, LOW); // Note that this makes the led turn on, it's wire this way to allow for the voltage sensing above.
    Serial.begin(9600);
    pinMode(1, OUTPUT); // OC1A, also The only HW-PWM -pin supported by the tiny core analogWrite
    TinyWireS.begin(I2C_SLAVE_ADDRESS);
    TinyWireS.onReceive(receiveEvent);
    TinyWireS.onRequest(requestEvent);
}

void loop()
{
/**
 * This is the only way we can detect stop condition (http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=984716&sid=82e9dc7299a8243b86cf7969dd41b5b5#984716)
 * it needs to be called in a very tight loop in order not to miss any (REMINDER: Do *not* use delay() anywhere, use tws_delay() instead).
 * It will call the function registered via TinyWireS.onReceive(); if there is data in the buffer on stop.
 */
TinyWireS_stop_check();

//Puts the data we got into a variable to send back.
chrSendData = char(rxChrData);

}
