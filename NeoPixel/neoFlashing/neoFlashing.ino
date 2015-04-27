#define I2C_SLAVE_ADDRESS 0x4 // the 7-bit address (remember to change this when adapting this example)
 

// Get this from https://github.com/rambo/TinyWire
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

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
volatile char rxData; //Receives the data. mode is between 0x0 to 0x1, angle is between 0x11 to 0xff
volatile char rxDataTemp; //Receives the data temp.

//for neopixel
#define NEO_PIN        3  
#define NUMPIXELS      16
#define FOLLOW_BOUND  (4)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);
int delayval = 50; // delay for half a second


int next_state=0;
int dir=0;

//kneo_fx





void requestEvent(){  

  TinyWireS.send(chrSendData);
}

/**
 * The I2C data received -handler0x19+
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
  
      rxData = TinyWireS.receive();
      }
}

void setLoopColor(Adafruit_NeoPixel& pxl, int8_t pos,uint8_t R,uint8_t G,uint8_t B){
  pixels.setPixelColor(pos%NUMPIXELS,R,G,B);
}

void setup()
{
    TinyWireS.begin(I2C_SLAVE_ADDRESS);
    TinyWireS.onReceive(receiveEvent);
    TinyWireS.onRequest(requestEvent);
      // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
    #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
    #endif
    // End of trinket special code

    pixels.begin(); // This initializes the NeoPixel library.
}

void loop()
{
/**
 * This is the only way we can detect stop condition (http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=984716&sid=82e9dc7299a8243b86cf7969dd41b5b5#984716)
 * it needs to be called in a very tight loop in order not to miss any (REMINDER: Do *not* use delay() anywhere, use tws_delay() instead).
 * It will call the function registered via TinyWireS.onReceive(); if there is data in the buffer on stop.
 */
TinyWireS_stop_check();

  if (rxData<0x10 && rxData>0x0){
   next_state=int(rxData);
  //change mode
  }else if (rxData>=0x10 && rxData<0xff){
   next_state=0x10;
  }
  
  
  
  switch(next_state){
    case 0:
      break;
    case 0x10:
        int i=0;
        for(int i=0;i<NUMPIXELS;i++){
          pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.
	}
    //following mode
        //int dir=(rxData-0x10)*NUMPIXELS/(0xff-0x10);
        dir=rxData-0x10;
     	for (i=FOLLOW_BOUND;i>=0;i--){
	  setLoopColor(pixels,dir+i,255-(i*255/FOLLOW_BOUND),0,0);
	  setLoopColor(pixels,dir-i,0,0,255-i*255/FOLLOW_BOUND);
//          setLoopColor(pixels,dir+3,50,50,0);
//	  setLoopColor(pixels,dir-3,0,50,50);
          pixels.show();
	  tws_delay(100);
	}
      break;
  }
}


