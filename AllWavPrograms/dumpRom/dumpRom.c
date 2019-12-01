/* Author */
/* Fernando Candelario Herrero */

#include "dumpRom.h"


/*Global Variables*/
static FILE *fd;

/* Prototipes */

// Functions called by main 									 
static bool setup(int argc, char const *argv[]);
static void run_parser();


// Functions to manage errors, the program can finish because the use of these functions (process will end, use of exit()).
static void manageReadWriteErrors(FILE *localFd);


/* Functions to manage errors */
static void manageReadWriteErrors(FILE *localFd){
	if(ferror(localFd) != 0 ){
		printf("Programs fails while read/writing, closing\n");
		perror("-- >>  ERROR");
		fclose(fd); 
		exit(1);
	}
}



/* Functions called by main */
static void run_parser(){
	unsigned char c[BYTES_PER_SAMPLE];
	bool goOut = false;
	int  cont;

	cont = 0;

	while(!goOut){
		if(fread(c, sizeof(unsigned char),BYTES_PER_SAMPLE, fd) != BYTES_PER_SAMPLE){
			manageReadWriteErrors(fd);
			goOut = (feof(fd) != 0);
		}

		printf("%i=>X$",cont);

		for (int i = BYTES_PER_SAMPLE-1; i > -1; --i){
			if (c[i] == 0x00)
				printf("0" );
			
			printf("%x",c[i] );
		}

		printf("$, ");

		cont++;

		if(cont % NUM_COLS==0)
			printf("\n");

	}


	printf("Num Samples: %i\n", cont);
}


static bool setup(int argc, char const *argv[]){
	int a;
	char *fileName;
	const char *v = "F:O:v";
	bool ok1;

	fileName = NULL;
	ok1 = false;
	
	while( (a = getopt(argc,(char* *const)argv,v)) > 0){
	
		switch( (char)a ){
			case 'F':
				fileName = optarg;
				ok1 = true;
				break;

			case'v': 
				printf("This program remove the header of a .wav file \n");
				break;
		
			default:
				perror("USAGE: ./exec -F <dir bin file> \n");
				return false;
		}
	}

	if(!ok1 ){
		printf("-- >> ERROR\n");
		return false;
	}

	//Open the file
	if ( (fd = fopen(fileName,"rb")) == NULL ){
		printf("-- >> FAIL DURING OPEN");
		return false;
	}

	printf("Reading %s\n\n", fileName);

	return true;
}


/****************** MAIN *****************/

int main(int argc, char const *argv[]){
	
	if( setup(argc, argv) ){
		run_parser();
		fclose(fd); 
	} 
	else
		printf("-- >> SETUP FAILS\n ");

	exit(0);
}
