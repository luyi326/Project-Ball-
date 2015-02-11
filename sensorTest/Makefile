# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -std=gnu++0x
# LFLAGS = -L /usr/local/lib/libblackLib.a

# the build target executable:
TARGET = sensorTest

LIBDIR = ../9axis/

all: $(TARGET)

$(TARGET): ADXL345.o HMC5883L.o L3G4200D.o $(LIBDIR)ADXL345.h $(LIBDIR)HMC5883L.h $(LIBDIR)L3G4200D.h
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) main.cpp ADXL345.o HMC5883L.o L3G4200D.o

ADXL345.o: $(LIBDIR)ADXL345.h
	$(CC) $(CFLAGS) -c $(LIBDIR)ADXL345.cpp

HMC5883L.o: $(LIBDIR)HMC5883L.h
	$(CC) $(CFLAGS) -c $(LIBDIR)HMC5883L.cpp

L3G4200D.o: $(LIBDIR)L3G4200D.h
	$(CC) $(CFLAGS) -c $(LIBDIR)L3G4200D.cpp

clean:
	$(RM) $(TARGET) ADXL345.o HMC5883L.o L3G4200D.o
