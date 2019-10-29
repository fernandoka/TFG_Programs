CC=gcc
SRC=$(wildcard *.c)
OBJS=$(SRC:.c=.o)
TARGETS=$(SRC:.c=.elf)
CFLAGS=-Wall -g

midi_parser.elf: midi_parser.o
	$(CC) $(CFLAGS) -o $@ midi_parser.o

clean:
	-rm -f $(TARGETS)
	-rm -f $(OBJS)

