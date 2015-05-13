# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -std=gnu++0x -lrt
# LFLAGS = -L /usr/local/lib/libblackLib.a

# the build target executable:
TARGET = turnTest.out

OBJDIR = ../BlackLib/
STEPPERDIR = ../BlackStepper/
ARDUINO_DIR = ../arduinoConnector/
PIDDIR = ../PID/

PID_OBJ = PID.o
PID_HEADER = $(PIDDIR)PID.h

all: $(TARGET)

$(TARGET): $(OBJDIR)BlackCore.o $(OBJDIR)BlackGPIO.o $(OBJDIR)BlackPWM.o BlackStepper.o DualStepperMotor.o arduinoConnector.o $(PID_HEADER) $(PID_OBJ) $(OBJDIR)BlackLib.h $(STEPPERDIR)BlackStepper.h $(STEPPERDIR)DualStepperMotor.h $(OBJDIR)BlackSPI.h
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) main.cpp BlackStepper.o DualStepperMotor.o arduinoConnector.o $(OBJDIR)BlackCore.o $(OBJDIR)BlackGPIO.o $(OBJDIR)BlackPWM.o $(OBJDIR)BlackSPI.o $(PID_OBJ)

BlackStepper.o: $(STEPPERDIR)BlackStepper.h
	$(CC) $(CFLAGS) -c $(STEPPERDIR)BlackStepper.cpp

DualStepperMotor.o: $(STEPPERDIR)DualStepperMotor.h $(STEPPERDIR)BlackStepper.h
	$(CC) $(CFLAGS) -c $(STEPPERDIR)DualStepperMotor.cpp

arduinoConnector.o: $(ARDUINO_DIR)arduinoConnector.h $(OBJDIR)BlackSPI.h
	$(CC) $(CFLAGS) -c $(ARDUINO_DIR)arduinoConnector.cpp

PID.o: $(PIDDIR)PID.h
	$(CC) $(CFLAGS) -c $(PIDDIR)PID.cpp

clean:
	$(RM) $(TARGET) *.o
