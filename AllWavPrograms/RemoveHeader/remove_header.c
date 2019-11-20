/* Author */
/* Fernando Candelario Herrero */

#include "remove_header.h"


/*Global Variables*/
static FILE *fd;
static FILE *fd_out;	
static wav_header_t file;

/* Const variables */
static const char *id_0Mark = "RIFF";
static const char *id_1Mark = "WAVE";
static const char *id_2Mark = "fmt ";
static const char *id_3Mark = "data";

static const char *outpuFileMark = "_no_header.bin";
/* Prototipes */

// Functions called by main 									 
static bool setup(int argc, char const *argv[]);
static void run_parser();

// Functionality functions
static bool readHeader(unsigned int *n);
static void writeOutFile(unsigned int headerBytesReaded);

// Aux functions
static int getNBytesNum(int n);
static bool searchId_3Mark(unsigned int *numBytesReaded);
static unsigned char getNextByte();

static void closeFds();


// Functions to manage errors, the program can finish because the use of these functions (process will end, use of exit()).
static void manageReadWriteErrors(FILE *localFd);

/* Aux functions */
static bool searchId_3Mark(unsigned int *numBytesReaded){
	unsigned char s[3],c;
	bool found = false;
	unsigned int n;

	n = file.fileSize - *(numBytesReaded);

	while (n > 0 && !found) {
		c = getNextByte();
		*(numBytesReaded)+=1;
		n--;

		if(c == id_3Mark[0]){
			
		for (short int i = 0; i < 3; i++){	
			s[i] = getNextByte();
		}
			found = ( s[0] == id_3Mark[1] && s[1] == id_3Mark[2] && s[2] == id_3Mark[3] ); 
			
			*(numBytesReaded)+=3;
			n-=3;
		}
		
	}

	return found;
}

static bool checkCharacters(const char *ch){
	unsigned char s[4];
	short int i = 0;

	while(i < 4){
		s[i++] = getNextByte();
	}

	return s[0] == ch[0] && s[1] == ch[1] && 
			s[2] == ch[2] && s[3] == ch[3]; 
}

static unsigned char getNextByte(){
	unsigned char c;

	if(fread(&c,1,1,fd) != 1)
		manageReadWriteErrors(fd);
	
	return c;
}

static int getNBytesNum(int n){
	int aux;

	if(fread(&aux,1,n,fd) != n)
		manageReadWriteErrors(fd);

	return aux;
}

static void closeFds(){
	fclose(fd); 
	fclose(fd_out);
}

/* Functions to manage errors */
static void manageReadWriteErrors(FILE *localFd){
	if(ferror(localFd) != 0 ){
		printf("Programs fails while read/writing, closing\n");
		perror("-- >>  ERROR");
		closeFds();
		exit(1);
	}
}


/* Functionality functions */
static bool readHeader(unsigned int *n){

	printf("Reading Header \n\n");
	if( !checkCharacters(id_0Mark) ){
		printf("-- >> ERROR NOT VALID HEADER MARK 0\n");
		return false;
	}


	file.fileSize = (unsigned)getNBytesNum(4);

	if( !checkCharacters(id_1Mark) ){
		printf("-- >> ERROR NOT VALID HEADER MARK 1\n");
		return false;
	}

	if( !checkCharacters(id_2Mark) ){
		printf("-- >> ERROR NOT VALID HEADER MARK 2\n");
		return false;
	}

	file.sizeUntilNow = (unsigned)getNBytesNum(4);
	file.format = (short unsigned)getNBytesNum(2);
	file.channels = (short unsigned)getNBytesNum(2);

	if(file.channels > MAX_CHANNELS || file.channels < 1){
		printf("-- >> ERROR Nº OF CHANNELS EXCEDDS THE MAX VALUE : %i, CURRENT Nº OF CHANNELSIN IN THE .WAV FILE IS: %i\n",MAX_CHANNELS,file.channels);
		return false;
	}

	file.sampleFreq = (unsigned)getNBytesNum(4);
	file.averageNumBytesPerSec = (unsigned)getNBytesNum(4);

	file.alin = (short unsigned)getNBytesNum(2);
	file.numBitsPerSample = (short unsigned)getNBytesNum(2);
	
	if( file.numBitsPerSample != 8 && file.numBitsPerSample != 16 && file.numBitsPerSample != 24 && file.numBitsPerSample != 32 ){
		printf("-- >> ERROR NOT VALID BIT RESOLUTION PER SAMPLE, RESOLUTION RESOLUTION IS: %i\n",file.numBitsPerSample);
		return false;
	}

	*(n) = file.sizeUntilNow + (2*4)+(4*3);

	if( !searchId_3Mark(n) ){
		printf("-- >> ERROR NOT VALID HEADER MARK 3\n");
		return false;
	}

	file.numBytesForSampleData = (unsigned)getNBytesNum(4);

	*(n)+=4;

	return true;

}

static void writeOutFile(unsigned int headerBytesReaded){
	
	unsigned char *wavData;
	
	// Read all samples
	wavData = (unsigned char*) malloc(file.numBytesForSampleData);
	if( fread( (void *)wavData, 1, file.numBytesForSampleData, fd ) != file.numBytesForSampleData )
		manageReadWriteErrors(fd);


	if( fwrite( (void *)wavData, 1,file.numBytesForSampleData,fd_out) != file.numBytesForSampleData )
		manageReadWriteErrors(fd_out);
	
	//Free memory
	free(wavData); 

}


/* Functions called by main */
static void run_parser(){
	unsigned int bytesReaded = 0;

	if( !readHeader(&bytesReaded)){
		printf("-- >> FAIL READING HEADER ");
		return;
	}
	
	printf("	fileSize: %i\n",file.fileSize);
	printf("	sizeUntilNow: %i\n",file.sizeUntilNow);
	printf("	Format: %i\n",file.format);
	printf("	channels: %i\n",file.channels);
	printf("	sampleFreq: %i\n",file.sampleFreq);

	printf("	averageNumBytesPerSec: %i\n",file.averageNumBytesPerSec);
	printf("	alin: %i\n",file.alin);
	printf("	numBitsPerSample: %i\n",file.numBitsPerSample);
	printf("	numBytesForSampleData: %i\n",file.numBytesForSampleData);
	printf("	numSamples: %i\n",file.numBytesForSampleData/(file.numBitsPerSample/8));

	writeOutFile(bytesReaded);
}


static bool setup(int argc, char const *argv[]){
	int a,aux1,aux2,aux3,i,j;
	char *fileName,*dirOutFile,*Fname,*OutfileName;
	const char *v = "F:O:v";
	bool ok1,ok2,goOut,startCount;
	int lenOfFileName,posOfForwardSlash,cont;

	fileName = dirOutFile = Fname = NULL;
	startCount = ok1 = ok2 = goOut = false;
	
	while( (a = getopt(argc,(char* *const)argv,v)) > 0){
	
		switch( (char)a ){
			case 'F':
				fileName = optarg;
				ok2 = true;
				break;

			case 'O':
				dirOutFile = optarg;
				ok1 = true;
				break;	

			case'v': 
				printf("This program remove the header of a .wav file \n");
				break;
		
			default:
				perror("USAGE: ./exec -F <dir Wav file> -O <dir of out file, should end with / -T target frequency -B base frequency> \nUSE -v to show the value of the file out samples");
				return false;
		}
	}

	if(!ok1 || !ok2 ){
		printf("-- >> YOU HAVE TO SPECIFY THE OUT ADDRESS WITH -O, THE BASE FILE TO READ WITH -F \nTHE BASE FREQ THAT IS SAVED IN THE BASE FILE WITH -B, AND THE TARGET FREQ TO GET\n");
		return false;
	}

	//Open the file
	if ( (fd = fopen(fileName,"rb")) == NULL ){
		printf("-- >> FAIL DURING OPEN");
		return false;
	}

	printf("Reading %s\n\n", fileName);

	aux1 = (int)strlen(fileName);

	// Count length of the name of the files
	i = aux1;
	cont = -1;
	while( i >= 0 && !goOut){
		goOut = (fileName[i] == '/');
		startCount = startCount || (fileName[i] == '.');
		
		if(startCount)
			cont++;
			
		i--;
	}
		
	posOfForwardSlash = i+1;
	lenOfFileName = cont-1;
	
	aux2 = (int)strlen(outpuFileMark);
	aux3 = (int)strlen(dirOutFile); 
	OutfileName =(char*)  malloc ( sizeof(char)*(lenOfFileName+aux2+aux3+1) );
	
	strncpy(OutfileName, dirOutFile, aux3);
	
	// Copy only the name of the file not the full address
	i=posOfForwardSlash+1;
	Fname = (char*) malloc(sizeof(char)* lenOfFileName);
	j=0;
	while (i < aux1){
		Fname[j] = fileName[i];
		j++; i++;
	}
	
	strncat(OutfileName, Fname, lenOfFileName);
	strncat(OutfileName, outpuFileMark, aux2);

	printf("Name of out file: %s \n\n",OutfileName);

	//Open the file
	if ( (fd_out = fopen(OutfileName,"wb")) == NULL ){
		printf("-- >> FAIL CREATING THE FILE");
		return false;
	}

	free(OutfileName); 	free(Fname);

	return true;
}


/****************** MAIN *****************/

int main(int argc, char const *argv[]){
	
	if( setup(argc, argv) ){
		run_parser();
		closeFds();
	} 
	else
		printf("-- >> SETUP FAILS\n ");

	exit(0);
}
