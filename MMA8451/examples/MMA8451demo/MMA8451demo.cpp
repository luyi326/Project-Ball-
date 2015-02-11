/**************************************************************************/
/*!
    @file     Adafruit_MMA8451.h
    @author   K. Townsend (Adafruit Industries)
    @license  BSD (see license.txt)

    This is an example for the Adafruit MMA8451 Accel breakout board
    ----> https://www.adafruit.com/products/2019

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section  HISTORY

    v1.0  - First release
*/
/**************************************************************************/

#include <iostream>
#include <stdint.h>
#include <unistd.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_MMA8451.h"

using namespace std;

Adafruit_MMA8451 mma = Adafruit_MMA8451();

void setup(void) {
  // Serial.begin(9600);

  cout << "Adafruit MMA8451 test!" << endl;


  if (! mma.begin()) {
    cout << "Couldnt start" << endl;
    while (1);
  }
  cout << "MMA8451 found!" << endl;

  mma.setRange(MMA8451_RANGE_2_G);

  cout << "Range = " << (2 << mma.getRange() << "G" << endl;

}

void loop() {
  // Read the 'raw' data in 14-bit counts
  mma.read();
  cout << "X:\t" << mma.x;
  cout << "\tY:\t" << mma.y;
  cout << "\tZ:\t" << mma.z;
  cout << endl;

  /* Get a new sensor event */
  sensors_event_t event;
  mma.getEvent(&event);

  /* Display the results (acceleration is measured in m/s^2) */
  cout << "X: \t" << event.acceleration.x << "\t";
  cout << "Y: \t" << event.acceleration.y << "\t";
  cout << "Z: \t" << event.acceleration.z << "\t";
  cout << "m/s^2 " << endl;

  /* Get the orientation of the sensor */
  uint8_t o = mma.getOrientation();

  switch (o) {
    case MMA8451_PL_PUF:
      cout << "Portrait Up Front" << endl;
      break;
    case MMA8451_PL_PUB:
      cout << "Portrait Up Back" << endl;
      break;
    case MMA8451_PL_PDF:
      cout << "Portrait Down Front" << endl;
      break;
    case MMA8451_PL_PDB:
      cout << "Portrait Down Back" << endl;
      break;
    case MMA8451_PL_LRF:
      cout << "Landscape Right Front" << endl;
      break;
    case MMA8451_PL_LRB:
      cout << "Landscape Right Back" << endl;
      break;
    case MMA8451_PL_LLF:
      cout << "Landscape Left Front" << endl;
      break;
    case MMA8451_PL_LLB:
      cout << "Landscape Left Back" << endl;
      break;
    }
  cout << endl
  usleep(50000);

}


int main (int argc, char** argv) {
  setup();
  while (1) {
    loop();
  }
  return 1;
}