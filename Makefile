# the compiler: gcc for C program, define as g++ for C++
CC = gcc

# compiler flags:
#  -g    	adds debugging information to the executable file
#  -Wall 	turns on most, but not all, compiler warnings
#  -O3		tells the compiler to utilize optimization level 3

SRC = $(shell ls ./src/*.c)
HDR = $(shell ls ./src/*.h)
#TSTSRC = $(shell ls ./src/test/*.c)
CFLAGS = -O3 -lcrypto -lpthread

# the build target executable:
TARGET = server.out
#TESTTARGET = tests.out
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(HDR) $(SRC) -o $(TARGET) -s $(CFLAGS)

debug:
	$(CC) $(SRC) $(HDR) -o $(TARGET) -g -Wall -pedantic $(CFLAGS)

clean:
	$(RM) $(TARGET)

#test: $(TSTSRC)
#	$(CC) $(TSTSRC) -o $(TESTTARGET) -s -g -Wall -pedantic $(CFLAGS)
