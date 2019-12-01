CC=gcc
CFLAGS=-Wall 
NAME=dumpRom

make : $(NAME).elf

all: $(NAME).elf $(NAME)_op2.elf $(NAME)_op3.elf $(NAME)_debug.elf

op1: $(NAME)_op1.elf

op2: $(NAME)_op2.elf

op3: $(NAME)_op3.elf

gdb: $(NAME)_debug.elf

clean: cl rmWav

#Without optimization

$(NAME).elf: $(NAME).o
	$(CC) $(CFLAGS) -o $@ $(NAME).o 

$(NAME).o:	
	$(CC) $(CFLAGS) -c -o $@ $(NAME).c


#Optimization 1
$(NAME)_op1.elf: $(NAME)_op1.o
	$(CC) $(CFLAGS) -O1 -o $@ $(NAME)_op1.o

$(NAME)_op1.o:	
	$(CC) $(CFLAGS) -O1 -c -o $@ $(NAME).c
 

#Optimization 2
$(NAME)_op2.elf: $(NAME)_op2.o
	$(CC) $(CFLAGS) -O2 -o $@ $(NAME)_op2.o

$(NAME)_op2.o:	
	$(CC) $(CFLAGS) -O2 -c -o $@ $(NAME).c 


#Optimization 3
$(NAME)_op3.elf: $(NAME)_op3.o
	$(CC) $(CFLAGS) -O3 -o $@ $(NAME)_op3.o 

$(NAME)_op3.o:	
	$(CC) $(CFLAGS) -O3 -c -o $@ $(NAME).c 

#To use gdb
$(NAME)_debug.elf: $(NAME)_debug.o
	$(CC) $(CFLAGS) -g -o $@ $(NAME)_debug.o 

$(NAME)_debug.o: 	
	$(CC) $(CFLAGS) -g -c -o $@ $(NAME).c 

cl:
	-rm -f *.o
	-rm -f *.elf
	
rmWav:
	-rm -f *.wav
