#include <stdio.h>
#include <stdlin.h>
#include "../BlackLib/BlackUART.h"

int main(){
      BlackLib::BlackUART  myUart(BlackLib::UART4,
                             BlackLib::Baud19200,
                             BlackLib::ParityENo,
                             BlackLib::StopOne,
                             BlackLib::Char8 );

      myUart.open( BlackLib::ReadWrite | BlackLib::NonBlock );
      myUart.flush( BlackLib::bothDirection );

      char writeBuffer[]  = "this is test.\n";
      myUart.write(writeBuffer, sizeof(writeBuffer));

      sleep(1);

      std::string readFromUart = myUart.read();

      std::cout << "Test output on loopback: " << readFromUart;

}