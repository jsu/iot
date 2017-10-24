CC=gcc
CFLAGS=-ansi -std=c11 -pedantic -Wall -D_POSIX_C_SOURCE=200809L -lmosquitto -lpthread -lwiringPi -lbcm2835

prog: main.c lcd.c menu.c
	$(CC) $^ -o $@ $(CFLAGS)

test: test.c lcd.c
	$(CC) $^ -o $@ $(CFLAGS)
