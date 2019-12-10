/* Author */
/* Fernando Candelario Herrero */

#include "appendFiles.h"


/*Global Variables*/
static FILE *fd,*fd_out;

/* Prototipes */

// Functions called by main 									 
static bool setup(int argc, char const *argv[]);
static void run_append();


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
static void run_append(){
	unsigned char c;
	int  contBytes,contSamples;

	contSamples = contBytes = 0;

	while(feof(fd)==0){
		
		if(fread(&c, 1,1, fd) != 1)
			manageReadWriteErrors(fd);
			
		if(fwrite(&c,1,1,fd_out)!=1)
			manageReadWriteErrors(fd_out);		

		contBytes++;
		if(contBytes%BYTES_PER_SAMPLE == 0)
			contSamples++;
	}


	printf("Num Samples: %i\n", contSamples);
}


static bool setup(int argc, char const *argv[]){
	int a;
	char *fileName,*fileToWrite;
	const char *v = "F:O:v";
	bool ok1,ok2;

	fileToWrite=fileName = NULL;
	ok1 = ok2= false;
	
	while( (a = getopt(argc,(char* *const)argv,v)) > 0){
	
		switch( (char)a ){
			case 'F':
				fileName = optarg;
				ok1 = true;
				break;
			
			case 'O':
				fileToWrite = optarg;
				ok2=true;
				break;

			case'v': 
				printf("This program remove the header of a .wav file \n");
				break;
		
			default:
				perror("USAGE: ./exec -F <dir bin file> \n");
				return false;
		}
	}

	if(!ok1 && !ok2){
		printf("-- >> ERROR\n");
		return false;
	}

	//Open the file
	if ( (fd = fopen(fileName,"rb")) == NULL ){
		printf("-- >> FAIL DURING OPEN");
		return false;
	}

	//Open the file append mode
	if ( (fd_out = fopen(fileToWrite,"ab")) == NULL ){
		printf("-- >> FAIL DURING OPEN");
		return false;
	}


	printf("Reading %s\n\n", fileName);

	return true;
}


/****************** MAIN *****************/

int main(int argc, char const *argv[]){
	
	if( setup(argc, argv) ){
		run_append();
		fclose(fd); fclose(fd_out);
	} 
	else
		printf("-- >> SETUP FAILS\n ");

	exit(0);
}
