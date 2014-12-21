# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -std=gnu++0x
# LFLAGS = -L /usr/local/lib/libblackLib.a

# the build target executable:
TARGET = motorTest

OBJDIR = ../BlackLib/
STEPPERDIR = ../BlackStepper/

all: $(TARGET)

$(TARGET): $(OBJDIR)BlackCore.o $(OBJDIR)BlackGPIO.o BlackStepper.o $(OBJDIR)BlackLib.h
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) main.cpp BlackStepper.o $(OBJDIR)BlackCore.o $(OBJDIR)BlackGPIO.o

BlackStepper.o: $(ACCELDIR)BlackStepper.h
	$(CC) $(CFLAGS) -c $(STEPPERDIR)BlackStepper.cpp

clean:
	$(RM) $(TARGET)
