# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -std=gnu++0x -lblackLib
# LFLAGS = -L /usr/local/lib/libblackLib.a

# the build target executable:
TARGET = test

all: $(TARGET)

$(TARGET): Timing.o BlackCore.o BlackPWM.o Timing.h /home/debian/workspace/BlackLib/v2_0/BlackLib.h
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) main.cpp Timing.o BlackCore.o BlackPWM.o

Timing.o: Timing.h
	$(CC) $(CFLAGS) -c  Timing.cpp

clean:
	$(RM) $(TARGET)