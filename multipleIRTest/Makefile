# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -std=gnu++0x
# LFLAGS = -L /usr/local/lib/libblackLib.a

# the build target executable:
TARGET = multiIRTest.out

PIXDIR = ../PVision/
I2CDIR = ../I2CBase/

all: $(TARGET)

$(TARGET): I2CBase.o PCA9548A.o PVision.o $(I2CDIR)I2CBase.h $(PIXDIR)PCA9548A.h $(PIXDIR)PVision.h
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) main.cpp I2CBase.o PCA9548A.o PVision.o

I2CBase.o: $(I2CDIR)I2CBase.h
	$(CC) $(CFLAGS) -c $(I2CDIR)I2CBase.cpp

PCA9548A.o: $(PIXDIR)PCA9548A.h $(I2CDIR)I2CBase.h
	$(CC) $(CFLAGS) -c $(PIXDIR)PCA9548A.cpp

PVision.o: $(PIXDIR)PVision.h
	$(CC) $(CFLAGS) -c $(PIXDIR)PVision.cpp

clean:
	$(RM) $(TARGET) *.o
