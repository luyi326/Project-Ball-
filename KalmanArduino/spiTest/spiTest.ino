/*

 * SPI pin numbers:
 * SCK   13  // Serial Clock.
 * MISO  12  // Master In Slave Out.
 * MOSI  11  // Master Out Slave In.
 * SS    10  // Slave Select
 */
#include "pins_arduino.h"

char buf [100];
volatile byte pos;
volatile boolean process_it;
volatile char trans = 'A';

void setup (void)
{
 Serial.begin (9600);   // debugging
 Serial.println("Setup");

 // have to send on master in, *slave out*
 pinMode(MISO, OUTPUT);

 // turn on SPI in slave mode
 SPCR |= _BV(SPE);

 // turn on interrupts
 SPCR |= _BV(SPIE);

 pos = 0;
 process_it = false;
}  // end of setup


// SPI interrupt routine
ISR (SPI_STC_vect)
{
  //Serial.println("interrupt triggered");
byte c = SPDR;
  //Serial.println(c);

 // add to buffer if room
 if (pos < sizeof buf)
   {
   buf [pos++] = c;

   // example: newline means time to process buffer
   if (c == '\n')
     process_it = true;

   }  // end of room available
   SPDR = trans;
   trans++;
}

// main loop - wait for flag set in interrupt routine
void loop (void)
{
 if (process_it)
   {
   buf [pos] = 0;
   Serial.println (buf);
   pos = 0;
   process_it = false;
   }  // end of flag set

}  // end of loop
