# the compiler: gcc
CC = gcc

# compiler flags:
#  -g    	adds debugging information to the executable file
#  -Wall 	turns on most, but not all, compiler warnings
#  -O3		tells the compiler to utilize optimization level 3
#  -s			strips unused from the binary
  
SRC = $(shell ls ./src/*.c)
HDR = $(shell ls ./src/*h)
CFLAGS = -O3 -lcrypto

# the build target executable:
TARGET = server.out

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) $(HDR) -o $(TARGET) -s $(CFLAGS)

debug:
	$(CC) $(SRC) $(HDR) -o $(TARGET) -g -Wall -pedantic $(CFLAGS)

clean:
	$(RM) $(TARGET)
