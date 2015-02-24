# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -std=gnu++0x
# LFLAGS = -L /usr/local/lib/libblackLib.a

# the build target executable:
TARGET = ADXL345Test

OBJDIR = ../BlackLib/
IMUDIR = ./

all: $(TARGET)

$(TARGET): $(IMUDIR)ADXL345.o $(IMUDIR)ADXL345.h
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) ADXLtest.cpp ADXL345.o

ADXL345.o: $(IMUDIR)ADXL345.h
	$(CC) $(CFLAGS) -c $(IMUDIR)ADXL345.cpp

clean:
	$(RM) $(TARGET) *.o
