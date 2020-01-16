/* Author */
/* Fernando Candelario Herrero */

#include "dumpConstants.h"


/*Global Variables*/
unsigned int max = 0;

/* Prototipes */

// Functions called by main 									 
static bool setup(int argc, char const *argv[]);
static void run();
static void toHex(int aux, unsigned char * c);
static char toHexDigit(int n);

static char toHexDigit(int n){
	unsigned char c;

	switch(n){
		case 1:
			c='1';
		 break;
		
		case 2:
			c='2';
		 break;
		
		case 3:
			c='3';
		 break;
		
		case 4:
			c='4';
		 break;
			 
		case 5:
			c='5';
		 break;
		
		case 6:
			c='6';
		 break;
		
		case 7:
			c='7';
		 break;
		
		case 8:
			c='8';
		 break;

		case 9:
			c='9';
		 break;
		
		case 10:
			c='A';
		 break;
		
		case 11:
			c='B';
		 break;
		
		case 12:
			c='C';
		 break;
			 
		case 13:
			c='D';
		 break;
		
		case 14:
			c='E';
		 break;
		
		case 15:
			c='F';
		 break;	 

		default:
			c='0';

	}

	return c;
}

void toHex(int aux, unsigned char * c){
	int co;
	int i = NUM_HEX_DIGTS-1;

	while( i > -1  && (co=aux/16)!=0 ){
		c[i--] = toHexDigit(aux%16);
		aux = co;	
	}

	c[i--] = toHexDigit(aux%16);

	while(i > -1)
		c[i--] ='0';
}


/* Functions called by main */
static void run(){
	unsigned char c[NUM_HEX_DIGTS];
	int cont = 0;
	double n = 1;
	double aux, pow = (double)POW_OF_TWO;

	while(n <= max){
		printf("%i=>X$",cont++);
		aux = ((1/n)*pow)+0.5f;
		toHex((int)aux, c);
		printf("%s$, ", c);
		n++;
	}

 
}


static bool setup(int argc, char const *argv[]){
	int a;
	const char *v = "N:v";
	bool ok1;

	ok1 = false;
	
	while( (a = getopt(argc,(char* *const)argv,v)) > 0){
	
		switch( (char)a ){
			case 'N':
				max = atoi(optarg);
				ok1 = true;
				break;

			case'v': 
				printf("This program do things \n");
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

	return true;
}


/****************** MAIN *****************/

int main(int argc, char const *argv[]){
	
	if( setup(argc, argv) ){
		run();
	} 
	else
		printf("-- >> SETUP FAILS\n ");

	exit(0);
}
