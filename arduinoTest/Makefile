# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -std=gnu++0x -lrt

# the build target executable:
TARGET = arduino.out

OBJDIR = ../BlackLib/
STEPPER_DIR = ../BlackStepper/
SERVO_DIR = ../BlackServo/
PIXDIR = ../PVision/
I2CDIR = ../I2CBase/
PIDDIR = ../PID/
ARDUINO_DIR = ../arduinoConnector/

BLACK_LIB_OBJ = $(OBJDIR)BlackCore.o $(OBJDIR)BlackSPI.o $(OBJDIR)BlackGPIO.o
BLACK_LIB_HEADER = $(OBJDIR)BlackLib.h $(OBJDIR)BlackSPI.h $(OBJDIR)BlackGPIO.h


# MOTOR_OBJ = BlackServo.o BlackStepper.o DualStepperMotor.o
# MOTOR_HEADER = $(STEPPER_DIR)BlackStepper.h $(SERVO_DIR)BlackServo.h $(STEPPER_DIR)DualStepperMotor.h

# PID_OBJ = PID.o
# PID_HEADER = $(PIDDIR)PID.h

# I2C_OBJ = I2CBase.o PCA9548A.o PVision.o IRRim.o vec.o BlobCompare.o
# I2C_HEADER = $(I2CDIR)I2CBase.h $(PIXDIR)PCA9548A.h $(PIXDIR)PVision.h $(PIXDIR)IRRim.h $(PIXDIR)vec.h $(PIXDIR)BlobCompare.h

all: $(TARGET)

$(TARGET): $(BLACK_LIB_OBJ) $(MOTOR_OBJ) $(BLACK_LIB_HEADER) $(MOTOR_HEADER) $(I2C_HEADER) $(I2C_OBJ) $(PID_HEADER) $(PID_OBJ) arduinoConnector.o
	$(CC) $(CFLAGS) -o $(TARGET) main.cpp $(MOTOR_OBJ) $(BLACK_LIB_OBJ) $(I2C_OBJ) $(PID_OBJ) arduinoConnector.o

arduinoConnector.o: $(ARDUINO_DIR)arduinoConnector.h $(OBJDIR)BlackSPI.h
	$(CC) $(CFLAGS) -c $(ARDUINO_DIR)arduinoConnector.cpp

clean:
	$(RM) $(TARGET) *.o
