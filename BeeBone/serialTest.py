import serial
port_name=raw_input("Please input port name: ")
port_name='/dev/'+port_name
bee_port=serial.Serial(port_name,115200)

if bee_port.isOpen():
	print "PORT has been open"


bee_port.flush()
bee_port.write("LOL")


bee_port.close()