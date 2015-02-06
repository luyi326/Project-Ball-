<<<<<<< HEAD
#include <stdio.h>
#include <stdlib.h>
#include "../BlackLib/BlackUART.h"
// #include "../BlackLib/BlackCore.h"
#include <stdint.h>
#include <iostream>


using namespace BlackLib;


int main(){
      BlackLib::BlackUART  myUart(BlackLib::UART1,
                             BlackLib::Baud19200,
                             BlackLib::ParityNo,
                             BlackLib::StopOne,
                             BlackLib::Char8 );

      myUart.open( BlackLib::ReadWrite | BlackLib::NonBlock );
      myUart.flush( BlackLib::bothDirection );

      char writeBuffer[]  = "this is test.\n";
      myUart.write(writeBuffer, sizeof(writeBuffer));


      while(1){

      sleep(1);

      std::string readFromUart = myUart.read();

      if (readFromUart.length()>0){

      std::cout << readFromUart;
}
}

=======
#include <stdio.h>
#include <stdlib.h>
#include "../BlackLib/BlackUART.h"
// #include "../BlackLib/BlackCore.h"
#include <stdint.h>
#include <iostream>


using namespace BlackLib;


int main(){
  BlackLib::BlackUART myUart(BlackLib::UART1,
                      BlackLib::Baud19200,
                      BlackLib::ParityNo,
                      BlackLib::StopOne,
                      BlackLib::Char8 );

  myUart.open( BlackLib::ReadWrite | BlackLib::NonBlock );
  myUart.flush( BlackLib::bothDirection );

  char writeBuffer[]  = "this is test.\n";
  myUart.write(writeBuffer, sizeof(writeBuffer));


  while(1){

    sleep(1);

    std::string readFromUart = myUart.read();

    if (readFromUart.length()>0){
      std::cout << readFromUart;
    }
  }
>>>>>>> origin/JoystickConnector
}