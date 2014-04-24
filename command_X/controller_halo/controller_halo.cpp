#include "controller_halo.h"

using namespace std;

#include <string>

std::string string_to_hex(const std::string& input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}

Halo_XBee::Halo_XBee():Halo(BlackLib::UART1,
                      		BlackLib::Baud19200,
                      		BlackLib::ParityNo,
                      		BlackLib::StopOne,
                      		BlackLib::Char8
							) {
	Halo.open( BlackLib::ReadWrite | BlackLib::NonBlock );
	Halo.flush( BlackLib::bothDirection );
	state = WAITING7E;
}

controller_state Halo_XBee::getControllerInfo() {
	return controllerInfo;
}


void Halo_XBee::refreshValue() {
	string buf = Halo.read();
	// cout << "Read " << buf.length() << " bytes" << endl;
	for (size_t i = 0; i < buf.length(); i++) {
		uint8_t c = buf[i];
		switch (state) {
			case WAITING7E:
				if (c == 0x7E) {
					state = SEVENEREAD;
				} else {
					continue;
				}
			break;
			case SEVENEREAD:
			if (c != 0x0A) {
				tmpValue[0] = c;
				state = FIRSTREAD;
			} else {
				state = WAITING7E;
			}
			break;
			case FIRSTREAD:
			if (c != 0x0A) {
				tmpValue[1] = c;
				state = SECONDREAD;
			} else {
				state = WAITING7E;
			}
			break;
			case SECONDREAD:
			if (c != 0x0A) {
				tmpValue[2] = c;
				this->updateController();
				state = WAITING7E;
			} else {
				state = WAITING7E;
			}
			break;
		}
	}
}

void Halo_XBee::updateController() {
	uint8_t identity, sign, value;
	identity = tmpValue[0];
	sign = tmpValue[1];
	value = tmpValue[2];

	switch (identity) {
		case 0x00:  //AXIS_0
			controllerInfo.AXIS_0=(sign==0? -1:1);
			controllerInfo.AXIS_0=controllerInfo.AXIS_0*value;
			break;
		case 0x01:
			controllerInfo.AXIS_1=(sign==0? -1:1);
			controllerInfo.AXIS_1=controllerInfo.AXIS_1*value;
			break;
		case 0x02:
			controllerInfo.AXIS_2=(sign==0? -1:1);
			controllerInfo.AXIS_2=controllerInfo.AXIS_2*value;
			break;
		case 0x03:
			controllerInfo.AXIS_3=(sign==0? -1:1);
			controllerInfo.AXIS_3=controllerInfo.AXIS_3*value;
			break;
		case 0x04:
			controllerInfo.AXIS_4=(sign==0? -1:1);
			controllerInfo.AXIS_4=controllerInfo.AXIS_4*value;
			break;
		// case 0x10:
		// 	controllerInfo.BUTTON_0=(sign==0? -1:1);
		// 	controllerInfo.BUTTON_0=controllerInfo.BUTTON_0*value;
		// 	break;
		// case 0x11:
		// 	controllerInfo.BUTTON_1=(sign==0? -1:1);
		// 	controllerInfo.BUTTON_1=controllerInfo.BUTTON_1*value;
		// 	break;
		// case 0x12:
		// 	controllerInfo.BUTTON_2=(sign==0? -1:1);
		// 	controllerInfo.BUTTON_2=controllerInfo.BUTTON_2*value;
		// 	break;
		// case 0x13:
		// 	controllerInfo.BUTTON_3=(sign==0? -1:1);
		// 	controllerInfo.BUTTON_3=controllerInfo.BUTTON_3*value;
		// 	break;
		// case 0x14:
		// 	controllerInfo.BUTTON_4=(sign==0? -1:1);
		// 	controllerInfo.BUTTON_4=controllerInfo.BUTTON_4*value;
		// 	break;
		// case 0x15:
		// 	controllerInfo.BUTTON_5=(sign==0? -1:1);
		// 	controllerInfo.BUTTON_5=controllerInfo.BUTTON_5*value;
		// 	break;
		// case 0x16:
		// 	controllerInfo.BUTTON_6=(sign==0? -1:1);
		// 	controllerInfo.BUTTON_6=controllerInfo.BUTTON_6*value;
		// 	break;
		// case 0x17:
		// 	controllerInfo.BUTTON_7=(sign==0? -1:1);
		// 	controllerInfo.BUTTON_7=controllerInfo.BUTTON_7*value;
		// 	break;
		// case 0x18:
		// 	controllerInfo.BUTTON_8=(sign==0? -1:1);
		// 	controllerInfo.BUTTON_8=controllerInfo.BUTTON_8*value;
		// 	break;
		// case 0x19:
		// 	controllerInfo.BUTTON_9=(sign==0? -1:1);
		// 	controllerInfo.BUTTON_9=controllerInfo.BUTTON_9*value;
		// 	break;
		default:
			break;
	}
}
