CC      = gcc
CFLAGS  = -g -Wall
RM      = rm -f

default: all

all: newshell

newshell: main.c
	$(CC) $(CFLAGS) -o newshell main.c utilities.c builtin.c programs.c

clean veryclean:
	$(RM) newshell
