CC = gcc
CFLAGS = -Wall -Wextra -pedantic-errors -O2
LIBS = -lwiringPi -lpthread

all: server

server: utils.c server.c sender.c gpio.c
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@
