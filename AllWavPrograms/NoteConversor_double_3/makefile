CC=gcc
LIBS=-lm
CFLAGS=-Wall 
NAME=wav_parser

make : $(NAME).elf

all: $(NAME).elf $(NAME)_op2.elf $(NAME)_op3.elf $(NAME)_debug.elf

op1: $(NAME)_op1.elf

op2: $(NAME)_op2.elf

op3: $(NAME)_op3.elf

gdb: $(NAME)_debug.elf

clean: cl rmWav

#Without optimization

$(NAME).elf: $(NAME).o
	$(CC) $(CFLAGS) -o $@ $(NAME).o $(LIBS)

$(NAME).o:	
	$(CC) $(CFLAGS) -c -o $@ $(NAME).c $(LIBS)


#Optimization 1
$(NAME)_op1.elf: $(NAME)_op1.o
	$(CC) $(CFLAGS) -O1 -o $@ $(NAME)_op1.o $(LIBS)

$(NAME)_op1.o:	
	$(CC) $(CFLAGS) -O1 -c -o $@ $(NAME).c $(LIBS)
 

#Optimization 2
$(NAME)_op2.elf: $(NAME)_op2.o
	$(CC) $(CFLAGS) -O2 -o $@ $(NAME)_op2.o $(LIBS)

$(NAME)_op2.o:	
	$(CC) $(CFLAGS) -O2 -c -o $@ $(NAME).c $(LIBS)


#Optimization 3
$(NAME)_op3.elf: $(NAME)_op3.o
	$(CC) $(CFLAGS) -O3 -o $@ $(NAME)_op3.o $(LIBS)

$(NAME)_op3.o:	
	$(CC) $(CFLAGS) -O3 -c -o $@ $(NAME).c $(LIBS)

#To use gdb
$(NAME)_debug.elf: $(NAME)_debug.o
	$(CC) $(CFLAGS) -g -o $@ $(NAME)_debug.o $(LIBS)

$(NAME)_debug.o: 	
	$(CC) $(CFLAGS) -g -c -o $@ $(NAME).c $(LIBS)

cl:
	-rm -f *.o
	-rm -f *.elf
	
rmWav:
	-rm -f *.wav
