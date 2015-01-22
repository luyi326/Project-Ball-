# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -std=gnu++0x
# LFLAGS = -L /usr/local/lib/libblackLib.a

# the build target executable:
TARGET = sensorTest.out


all: $(TARGET)

$(TARGET): adxl345.o adxl345.h
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) bb_adxl345_demo.c adxl345.o

adxl345.o: adxl345.h
	$(CC) $(CFLAGS) -c adxl345.c
clean:
	$(RM) $(TARGET) adxl345.o
