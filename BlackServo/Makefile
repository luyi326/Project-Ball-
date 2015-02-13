# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -std=gnu++0x
# LFLAGS = -L /usr/local/lib/libblackLib.a

# the build target executable:
TARGET = servoTest

OBJDIR = ../BlackLib/
SERVODIR = ./

all: $(TARGET)

$(TARGET): $(OBJDIR)BlackCore.o BlackServo.o $(OBJDIR)BlackLib.h $(OBJDIR)BlackPWM.h
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) main.cpp BlackServo.o $(OBJDIR)BlackCore.o $(OBJDIR)BlackPWM.o

BlackServo.o: $(SERVODIR)BlackServo.h
	$(CC) $(CFLAGS) -c $(SERVODIR)BlackServo.cpp

clean:
	$(RM) $(TARGET) *.o
