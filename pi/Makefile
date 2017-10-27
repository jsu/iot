CC=gcc
CFLAGS=-ansi -std=c11 -pedantic -Wall -D_POSIX_C_SOURCE=200809L -D_BSD_SOURCE -D_DEFAULT_SOURCE -lmosquitto -lpthread -lbcm2835

prog: main.c lcd.c menu.c
	$(CC) $^ -o $@ $(CFLAGS)

lcd: lcd.c
	$(CC) $^ -o $@ $(CFLAGS)
