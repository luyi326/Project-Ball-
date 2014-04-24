# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -std=gnu++0x -lblackLib
# LFLAGS = -L /usr/local/lib/libblackLib.a

# the build target executable:
TARGET = test

OBJDIR = ../BlackLib/

all: $(TARGET)

$(TARGET): controller_halo.o $(OBJDIR)BlackCore.o $(OBJDIR)BlackUART.o $(OBJDIR)BlackLib.h
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) basicUART4.cpp $(OBJDIR)BlackCore.o $(OBJDIR)BlackUART.o controller_halo.o

controller_halo.o: ./controller_halo/controller_halo.h $(OBJDIR)BlackLib.h
	$(CC) $(CFLAGS) -c ./controller_halo/controller_halo.cpp

clean:
	$(RM) $(TARGET) controller_halo.o
