# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -std=gnu++0x

# the build target executable:
TARGET = sensorTest


all: $(TARGET)

$(TARGET): Adafruit_MMA8451.o Adafruit_MMA8451.h
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) main.cpp Adafruit_MMA8451.o

Adafruit_MMA8451.o: Adafruit_MMA8451.h
	$(CC) $(CFLAGS) -c Adafruit_MMA8451.cpp
clean:
	$(RM) $(TARGET) *.o
