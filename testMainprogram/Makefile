# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -std=gnu++0x
# LFLAGS = -L /usr/local/lib/libblackLib.a

# the build target executable:
TARGET = controlTest.out

OBJDIR = ../BlackLib/
STEPPERDIR = ../BlackStepper/
JOYDIR = ../command_X/controller_halo/

all: $(TARGET)

$(TARGET): controller_halo.o $(OBJDIR)BlackCore.o $(OBJDIR)BlackGPIO.o $(OBJDIR)BlackPWM.o BlackStepper.o DualStepperMotor.o $(OBJDIR)BlackLib.h $(STEPPERDIR)BlackStepper.h $(STEPPERDIR)DualStepperMotor.h
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) main.cpp BlackStepper.o DualStepperMotor.o controller_halo.o $(OBJDIR)BlackCore.o $(OBJDIR)BlackGPIO.o $(OBJDIR)BlackPWM.o $(OBJDIR)BlackUART.o

BlackStepper.o: $(STEPPERDIR)BlackStepper.h
	$(CC) $(CFLAGS) -c $(STEPPERDIR)BlackStepper.cpp

DualStepperMotor.o: $(STEPPERDIR)DualStepperMotor.h $(STEPPERDIR)BlackStepper.h
	$(CC) $(CFLAGS) -c $(STEPPERDIR)DualStepperMotor.cpp

controller_halo.o: $(JOYDIR)controller_halo.h $(OBJDIR)BlackUART.h
	$(CC) $(CFLAGS) -c $(JOYDIR)controller_halo.cpp



clean:
	$(RM) $(TARGET) BlackStepper.o DualStepperMotor.o controller_halo.o
