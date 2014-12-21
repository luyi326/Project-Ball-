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
ACCELDIR = ../AccelStepper/

all: $(TARGET)

$(TARGET): $(OBJDIR)BlackCore.o $(OBJDIR)BlackPWM.o $(OBJDIR)BlackLib.h
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) main.cpp $(OBJDIR)BlackCore.o $(OBJDIR)BlackPWM.o

clean:
	$(RM) $(TARGET)
