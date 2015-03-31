# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -std=gnu++0x -lrt

# the build target executable:
TARGET = IRAssembly.out

OBJDIR = ../BlackLib/
STEPPER_DIR = ../BlackStepper/
SERVO_DIR = ../BlackServo/
PIXDIR = ../PVision/
I2CDIR = ../I2CBase/

BLACK_LIB_OBJ = $(OBJDIR)BlackCore.o $(OBJDIR)BlackGPIO.o $(OBJDIR)BlackPWM.o $(OBJDIR)BlackADC.o
BLACK_LIB_HEADER = $(OBJDIR)BlackLib.h $(OBJDIR)BlackGPIO.h $(OBJDIR)BlackPWM.h $(OBJDIR)BlackADC.h

MOTOR_OBJ = BlackServo.o BlackStepper.o DualStepperMotor.o
MOTOR_HEADER = $(STEPPER_DIR)BlackStepper.h $(SERVO_DIR)BlackServo.h $(STEPPER_DIR)DualStepperMotor.h

I2C_OBJ = I2CBase.o PCA9548A.o PVision.o IRRim.o vec.o BlobCompare.o
I2C_HEADER = $(I2CDIR)I2CBase.h $(PIXDIR)PCA9548A.h $(PIXDIR)PVision.h $(PIXDIR)IRRim.h $(PIXDIR)vec.h $(PIXDIR)BlobCompare.h

all: $(TARGET)

$(TARGET): $(BLACK_LIB_OBJ) $(MOTOR_OBJ) $(BLACK_LIB_HEADER) $(MOTOR_HEADER) $(I2C_HEADER) $(I2C_OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) main.cpp $(MOTOR_OBJ) $(BLACK_LIB_OBJ) $(I2C_OBJ)

# Stepper. Servos and double steppers
BlackStepper.o: $(STEPPER_DIR)BlackStepper.h $(OBJDIR)BlackPWM.h
	$(CC) $(CFLAGS) -c $(STEPPER_DIR)BlackStepper.cpp

BlackServo.o: $(SERVO_DIR)BlackServo.h $(OBJDIR)BlackPWM.h
	$(CC) $(CFLAGS) -c $(SERVO_DIR)BlackServo.cpp

DualStepperMotor.o: $(STEPPER_DIR)DualStepperMotor.h $(STEPPER_DIR)BlackStepper.h
	$(CC) $(CFLAGS) -c $(STEPPER_DIR)DualStepperMotor.cpp

#I2C base util, mux, IR sensors, and collection of sensors
I2CBase.o: $(I2CDIR)I2CBase.h
	$(CC) $(CFLAGS) -c $(I2CDIR)I2CBase.cpp

PCA9548A.o: $(PIXDIR)PCA9548A.h $(I2CDIR)I2CBase.h
	$(CC) $(CFLAGS) -c $(PIXDIR)PCA9548A.cpp

PVision.o: $(PIXDIR)PVision.h
	$(CC) $(CFLAGS) -c $(PIXDIR)PVision.cpp

vec.o: $(PIXDIR)vec.h
	$(CC) $(CFLAGS) -c $(PIXDIR)vec.cpp

BlobCompare.o: $(PIXDIR)BlobCompare.h
	$(CC) $(CFLAGS) -c $(PIXDIR)BlobCompare.cpp

IRRim.o: $(I2C_HEADER) $(BLACK_LIB_HEADER) $(SERVO_DIR)BlackServo.h $(PIXDIR)BlobCompare.h
	$(CC) $(CFLAGS) -c $(PIXDIR)IRRim.cpp

clean:
	$(RM) $(TARGET) *.o
