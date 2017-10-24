CC=gcc
CFLAGS=-ansi -std=c11 -pedantic -Wall -D_POSIX_C_SOURCE=199309L -lmosquitto -lpthread

simulator: simulator.c
	$(CC) simulator.c -o $@ $(CFLAGS)
