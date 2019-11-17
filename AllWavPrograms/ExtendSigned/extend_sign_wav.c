/* Author */
/* Fernando Candelario Herrero */

#include "extend_sign_wav.h"


/*Global Variables*/
static FILE *fd;
static FILE *fd_out;	
static wav_header_t file;
static bool verbose = false;

/* Const variables */
static const char *id_0Mark = "RIFF";
static const char *id_1Mark = "WAVE";
static const char *id_2Mark = "fmt ";
static const char *id_3Mark = "data";

static const char *outpuFileMark = "_extend.wav";
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
static int rawDataToInt(unsigned char *c, int index, int size);

// Complex functions
static unsigned int writeSamples();

// Functions to manage errors, the program can finish because the use of these functions (process will end, use of exit()).
static void manageReadWriteErrors(FILE *localFd);
static void manageFseekErrors();

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
	

static int rawDataToInt(unsigned char *c, int index, int size){
	int n,j,i;
	
	j = n = 0;
	for (i = index; i < index+size-1; i++){
		 n =  c[i]<< 8*j | n; 
		 j++;
	}
		
	n |=  ((signed char)c[i]<< 8*j);

	return n;	
}

static void intToRawData(unsigned char *c, int n, int index, int size){
	int i;

	for(i = index; i < index+size;i++){
		c[i] = (unsigned char)(n & 0xFF);
		n >>= 8;
	}

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


static void manageFseekErrors(){
	printf("Programs fails while seek the fd, closing\n");
	perror("-- >>  ERROR");
	closeFds();
	exit(1);
}

/* Complex functions */
static unsigned int writeSamples(){

	unsigned int bytesPerSample = file.numBitsPerSample/8;
	int totalSamplesInFile = file.numBytesForSampleData/bytesPerSample;
	int height = file.channels; 			// How many channels are
	int width = totalSamplesInFile/height;  // How many samples per channel

	unsigned char *wavData;
	unsigned char *wavDataOut;
	int *samples;
	int wavDataIndex;

	// Read all samples
	wavData = (unsigned char*) malloc(file.numBytesForSampleData);
	if( fread( (void *)wavData, 1, file.numBytesForSampleData, fd ) != file.numBytesForSampleData )
		manageReadWriteErrors(fd);

	// The sample are seen like if there are stored in a matrix
	samples = (int*) malloc(sizeof(int)*totalSamplesInFile);	

	wavDataIndex = 0;
	for(int j = 0; j < width; j++)
		for(int i = 0; i < height; i++){
			samples[i*width+j] = rawDataToInt(wavData,wavDataIndex,bytesPerSample);
			wavDataIndex += bytesPerSample;
		}

	if( verbose ){
		printf( "\nInterpolation (First channel)\n");
		printf( "--------------------------------------------------------------------\n");
		printf( "       index    intValue 	HexValue								 \n");
		printf( "--------------------------------------------------------------------\n");
	}

	wavDataOut = (unsigned char*) malloc(totalSamplesInFile*(bytesPerSample+1)); // Reserve memory for the extendSign
	wavDataIndex = 0;
	for(int j = 0; j < width; j++)
		for(int i = 0; i < height; i++){
			intToRawData(wavDataOut,samples[i*width+j],wavDataIndex,bytesPerSample+1);
			wavDataIndex += bytesPerSample+1;

		if( verbose )
			printf( "%8i%10i%17x\n", i*width+j,samples[i*width+j], samples[i*width+j] );

		}// For


	if(fwrite( (void *)wavDataOut, 1,totalSamplesInFile*(bytesPerSample+1),fd_out) != totalSamplesInFile*(bytesPerSample+1)) // Riff
		manageReadWriteErrors(fd_out);
	
	//Free memory
	free(wavData); // Free the previous memory
	free(samples);
	free(wavDataOut);

	return (unsigned int) totalSamplesInFile*(bytesPerSample+1);
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
	//Variables	
	unsigned char *c = malloc(headerBytesReaded);
	unsigned int newNumBytesForSample;
	unsigned int TotalBytes_out;
	short unsigned int newNumBitsPerSample = file.numBitsPerSample+8;
	
	//Copy Header
	rewind(fd);
	if(fread(c, sizeof(unsigned char),headerBytesReaded, fd) != headerBytesReaded)
		manageReadWriteErrors(fd);

	if(fwrite(c, sizeof(unsigned char), headerBytesReaded,fd_out) != headerBytesReaded)
		manageReadWriteErrors(fd_out);

	newNumBytesForSample = writeSamples();
	
	//Update Header
	if(fseek(fd_out,headerBytesReaded-4,SEEK_SET) == -1)
		manageFseekErrors();
	
	if(fwrite(&newNumBytesForSample, sizeof(unsigned char), sizeof(unsigned int),fd_out) != sizeof(unsigned int)) // bytesForSampleData
		manageReadWriteErrors(fd_out);
	
	if(fseek(fd_out,4,SEEK_SET) == -1)
		manageFseekErrors();
	
	TotalBytes_out = newNumBytesForSample+headerBytesReaded;

	if(fwrite(&TotalBytes_out, sizeof(unsigned char), sizeof(unsigned int),fd_out) != sizeof(unsigned int)) // totalBytesInFile
		manageReadWriteErrors(fd_out);
	

	if(fseek(fd_out,file.sizeUntilNow + (2*3)+(4*3)	,SEEK_SET) == -1)
		manageFseekErrors();
	
	if(fwrite(&newNumBitsPerSample, sizeof(unsigned char), sizeof(short unsigned int),fd_out) != sizeof(short unsigned int)) // totalBytesInFile
		manageReadWriteErrors(fd_out);

	
	printf("\nWriting new wav file\n\n");
	printf(" 	File Out Size: %i\n",TotalBytes_out);
	printf("	Header Bytes Readed: %i\n",headerBytesReaded);
	printf("	Total Num Bytes For Samples: %i\n",newNumBytesForSample);
	printf("	New bits per sample: %i\n\n",newNumBitsPerSample);

 	free(c);
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
	const char *v = "F:O:S:T:B:v";
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
				verbose = true;
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
