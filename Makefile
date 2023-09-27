# the compiler: gcc for C program, define as g++ for C++
CC = gcc

# compiler flags:
#  -g    	adds debugging information to the executable file
#  -Wall 	turns on most, but not all, compiler warnings
#  -O3		tells the compiler to utilize optimization level 3

SRC = $(shell ls ./src/*.c)
CFLAGS = -Wall -O3 -lcrypto

# the build target executable:
TARGET = server.out

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) -s $(CFLAGS)

debug:
	$(CC) $(SRC) -o $(TARGET) -g $(CFLAGS)

clean:
	$(RM) $(TARGET)
