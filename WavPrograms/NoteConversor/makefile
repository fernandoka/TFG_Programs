CC=gcc
#SRC=$(wildcard *.c)
#OBJS=$(SRC:.c=.o)
#TARGETS=$(SRC:.c=.elf)
CFLAGS=-Wall 

make : wav_parser.elf

all: wav_parser.elf wav_parser_op2.elf wav_parser_op3.elf wav_parser_debug.elf

op1: wav_parser_op1.elf

op2: wav_parser_op2.elf

op3: wav_parser_op3.elf

gdb: wav_parser_debug.elf

#Without optimization

wav_parser.elf: wav_parser.o
	$(CC) $(CFLAGS) -o $@ wav_parser.o

wav_parser.o:	
	$(CC) $(CFLAGS) -c -o $@ wav_parser.c


#Optimization 1
wav_parser_op1.elf: wav_parser_op1.o
	$(CC) $(CFLAGS) -O1 -o $@ wav_parser_op1.o

wav_parser_op1.o:	
	$(CC) $(CFLAGS) -O1 -c -o $@ wav_parser.c


#Optimization 2
wav_parser_op2.elf: wav_parser_op2.o
	$(CC) $(CFLAGS) -O2 -o $@ wav_parser_op2.o

wav_parser_op2.o:	
	$(CC) $(CFLAGS) -O2 -c -o $@ wav_parser.c


#Optimization 3
wav_parser_op3.elf: wav_parser_op3.o
	$(CC) $(CFLAGS) -O3 -o $@ wav_parser_op3.o

wav_parser_op3.o:	
	$(CC) $(CFLAGS) -O3 -c -o $@ wav_parser.c

#To use gdb
wav_parser_debug.elf: wav_parser_debug.o
	$(CC) $(CFLAGS) -g -o $@ wav_parser_debug.o 

wav_parser_debug.o: 	
	$(CC) $(CFLAGS) -g -c -o $@ wav_parser.c 

clean:
	-rm -f *.o
	-rm -f *.elf

