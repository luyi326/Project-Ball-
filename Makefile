# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -std=gnu++0x -lblackLib
# LFLAGS = -L /usr/local/lib/libblackLib.a

# the build target executable:
TARGET = test

OBJDIR = BlackLib/

all: $(TARGET)

$(TARGET): Timing.o $(OBJDIR)BlackCore.o $(OBJDIR)BlackPWM.o Timing.h $(OBJDIR)BlackLib.h
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) main.cpp Timing.o $(OBJDIR)BlackCore.o $(OBJDIR)BlackPWM.o

Timing.o: Timing.h
	$(CC) $(CFLAGS) -c  Timing.cpp

clean:
	$(RM) $(TARGET)