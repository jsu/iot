CC		:=gcc
HEAD	:= bcm2835.h lcd.h menu.h
SRC		:= bcm2835.c lcd.c menu.c main.c
OBJS	:= bcm2835.o lcd.o menu.o main.o
PROG	:= prog

Files	:= $(wildcard ./*)

INC		= .
CFLAGS	=-g -ansi -std=c11 -pedantic -Wall -D_POSIX_C_SOURCE=200809L -D_BSD_SOURCE -D_DEFAULT_SOURCE 
LDFLAGS	= -Wall -g -lmosquitto -lpthread

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

libpurifier.so: bcm2835.o lcd.o menu.o
	$(CC) -shared $(LDFLAGS) bcm2835.o lcd.o menu.o -o $@

main.o: main.c bcm2835.h lcd.h menu.h
	$(CC) $(CFLAGS) -c $< -o $@

bcm2835.o: bcm2835.c bcm2835.h
	$(CC) $(CFLAGS) -c $< -o $@

lcd.o: lcd.c lcd.h
	$(CC) $(CFLAGS) -c $< -o $@

menu.o: menu.c menu.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean file_list

file_list:
	@echo "<<<<< Files in this folder >>>>>"
	@file $(Files)

clean:
	@echo "----- Clean -----"
	rm -f *.o
	rm -f $(PROG)
	rm -f libpurifier.so
