# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -std=gnu++0x
# LFLAGS = -L /usr/local/lib/libblackLib.a

# the build target executable:
TARGET = pixartTest

PIXDIR = ../PVision/

all: $(TARGET)

$(TARGET): PVision.o $(PIXDIR)PVision.h
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) main.cpp Pvision.o

PVision.o: $(PIXDIR)PVision.h
	$(CC) $(CFLAGS) -c $(PIXDIR)PVision.cpp

clean:
	$(RM) $(TARGET) *.o