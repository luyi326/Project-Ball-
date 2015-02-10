#include <stdio.h>
#include <stdlib.h>
#include "controller_halo/controller_halo.h"
#include "../BlackLib/BlackUART.h"
// #include "../BlackLib/BlackCore.h"
#include <stdint.h>
#include <unistd.h>
#include <iostream>


using namespace std;
using namespace BlackLib;


int main(){
  // BlackLib::BlackUART myUart(BlackLib::UART1,
  //                     BlackLib::Baud19200,
  //                     BlackLib::ParityNo,
  //                     BlackLib::StopOne,
  //                     BlackLib::Char8 );



  // myUart.open( BlackLib::ReadWrite | BlackLib::NonBlock);
  // myUart.flush( BlackLib::bothDirection );
  // while (1) {
  //   // myUart.write("FXXXK");
  //   // sleep(1);
  //   std::string readFromUart = myUart.read();
  //   std::cout << "Test output on loopback: " << readFromUart << endl;
  //   sleep(1);
  // }

  // char writeBuffer[]  = "this is test.\n";
  // myUart.write(writeBuffer, sizeof(writeBuffer));

  Halo_XBee xbee;
  while(1){
    xbee.refreshValue();
    printf("axis0 is %d\n", xbee.getControllerInfo().AXIS_0);
    usleep(500);
  }
  return 0;
}
