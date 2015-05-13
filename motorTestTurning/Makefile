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

BLACK_LIB_OBJ = $(OBJDIR)BlackCore.o $(OBJDIR)BlackGPIO.o $(OBJDIR)BlackPWM.o $(OBJDIR)BlackSPI.o
BLACK_LIB_HEADER = $(OBJDIR)BlackLib.h $(OBJDIR)BlackGPIO.h $(OBJDIR)BlackPWM.h $(OBJDIR)BlackSPI.h

STEPPER_OBJ = BlackStepper.o DualStepperMotor.o
STEPPER_HEADER = $(STEPPERDIR)BlackStepper.h $(STEPPERDIR)DualStepperMotor.h

PID_OBJ = PID.o
PID_HEADER = $(PIDDIR)PID.h

all: $(TARGET)

$(TARGET): $(OBJDIR) $(BLACK_LIB_OBJ) $(STEPPER_OBJ) arduinoConnector.o $(PID_HEADER) $(PID_OBJ) $(STEPPER_HEADER)
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) main.cpp $(STEPPER_OBJ) arduinoConnector.o $(BLACK_LIB_OBJ) $(PID_OBJ)

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
