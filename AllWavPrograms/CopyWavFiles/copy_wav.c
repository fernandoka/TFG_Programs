/* Author */
/* Fernando Candelario Herrero */

#include "copy_wav.h"


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

static const char *outpuFileMark = "_copy.wav";


/* Prototipes */
static bool setup(int argc, char const *argv[]);
static void run_parser();
static unsigned char getNextByte();
static bool readHeader();

static bool checkCharacters(const char *ch);
static unsigned int get4BytesNum();
static short unsigned int get2BytesNum();
static bool searchId_3Mark(int *numBytesReaded);

static bool writeOutFile(int headerBytesReaded);
static void write4BytesNum(unsigned int n);
static void closeFds();

static int stringToInt(unsigned char *c, int size);
static void writeInt(int n, int bytesToWrite);

static bool copySamples(int bytesReaded, unsigned int *n, int *sampleInIndex, int * numBytesForSampleData_Out);
static unsigned int writeSamples();

// Functions to manage errors, the program can finish because the use of these functions (process will end, use of exit()).
static void manageReadWriteErrors(FILE *localFd);
static void manageSeekErrors();


/* Aux functions */
static bool searchId_3Mark(int *numBytesReaded){
	unsigned char s[3],c;
	short int i;
	bool found = false;
	unsigned int n;

	n = file.fileSize - *(numBytesReaded);

	while (n > 0 && !found) {
		c = getNextByte();
		*(numBytesReaded)+=1;
		n--;

		if(c == id_3Mark[0]){
			
			i = 0;	
			while(i < 3){
				s[i++] = getNextByte();
				*(numBytesReaded)+=1;
			}
		
			found = ( s[0] == id_3Mark[1] && s[1] == id_3Mark[2] && s[2] == id_3Mark[3] ); 
			
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

static unsigned int get4BytesNum(){
	unsigned int aux;
	aux = 0;

	for (int i = 0; i < 4; i++)
		aux = getNextByte() << 8*i | aux; // Desplazo a la izquierda
	
	

	return aux;
}

static short unsigned int get2BytesNum(){
	unsigned int aux;
	aux = 0;
	
	for (int i = 0; i < 2; i++)
		aux = getNextByte() << 8*i | aux; // Desplazo a la izquierda


	return aux;
}

static void write4BytesNum(unsigned int n){
	unsigned char c[4];
	
	for (int i = 0; i < 4; i++){	
		c[i] = (unsigned char)(n & 0x000000FF);
		n >>= 8;
	}	

	if(fwrite(c, sizeof(unsigned char), 4,fd_out) != 4)
		manageReadWriteErrors(fd_out); 
}

static void closeFds(){
	fclose(fd); 
	fclose(fd_out);
}
	

static int stringToInt(unsigned char *c, int size){
	int n = 0;

	for (int i = 0; i < size; i++)
		 n =  c[i]<< 8*i | n; 
	
	return n;	
}

static void writeInt(int n, int bytesToWrite){
	unsigned char *s = (unsigned char*) malloc( sizeof(unsigned char)*bytesToWrite );


	for(int i = 0; i<bytesToWrite;i++){
		s[i] = (unsigned char)(n & 0xFF);
		n >>= 8;
	}

	if(fwrite(s, sizeof(unsigned char), bytesToWrite,fd_out) != bytesToWrite)
		manageReadWriteErrors(fd_out); 

	free(s);
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

static void manageSeekErrors(){
	printf("Programs fails while fseek(), closing\n");
	perror("-- >>  ERROR");
	closeFds();
	exit(1);
}

static bool copySamples(int bytesPerSample, unsigned int *n, int *sampleInIndex, int * numBytesForSampleData_Out){
	
	unsigned char * sample = (unsigned char*) malloc(sizeof(unsigned char)*bytesPerSample);
	bool goOut;
	int integer = 0;

	goOut = ( *(n) < bytesPerSample );
	*(n) -= bytesPerSample;	

	//Copy one sample			
	if(fread(sample, sizeof(unsigned char),bytesPerSample, fd) != bytesPerSample)
		manageReadWriteErrors(fd);

	integer = stringToInt(sample,bytesPerSample);
	
	if(verbose)
		printf("Sample: %i, val: %i\n",*(sampleInIndex),integer);

	writeInt(integer,bytesPerSample);

	*(numBytesForSampleData_Out) += bytesPerSample;
	*(sampleInIndex)+=1;

	free(sample);

	return goOut;
}


static unsigned int writeSamples(){

	unsigned int n = file.numBytesForSampleData;
	int bytesPerSample = file.numBitsPerSample/8;
	int numBytesForSampleData_Out = 0;
	
	int sampleInIndex = 0;

	while( !copySamples(bytesPerSample, &n, &sampleInIndex, &numBytesForSampleData_Out) );
		
	return numBytesForSampleData_Out;
}

/* Functionality functions */
static bool readHeader(int *n){

	printf("Reading Header \n");
	if( !checkCharacters(id_0Mark) ){
		printf("-- >> ERROR NOT VALID HEADER MARK 0\n");
		return false;
	}


	file.fileSize = get4BytesNum();

	if( !checkCharacters(id_1Mark) ){
		printf("-- >> ERROR NOT VALID HEADER MARK 1\n");
		return false;
	}

	if( !checkCharacters(id_2Mark) ){
		printf("-- >> ERROR NOT VALID HEADER MARK 2\n");
		return false;
	}

	file.sizeUntilNow = get4BytesNum();
	file.format = get2BytesNum();
	file.channels = get2BytesNum();

	if(file.channels > MAX_CHANNELS || file.channels < 1){
		printf("-- >> ERROR Nº OF CHANNELS EXCEDDS THE MAX VALUE : %i, CURRENT Nº OF CHANNELSIN IN THE .WAV FILE IS: %i\n",MAX_CHANNELS,file.channels);
		return false;
	}

	file.sampleFreq = get4BytesNum();
	file.averageNumBytesPerSec = get4BytesNum();

	file.alin = get2BytesNum();
	file.numBitsPerSample = get2BytesNum();
	
	if( file.numBitsPerSample != 8 && file.numBitsPerSample != 16 && file.numBitsPerSample != 24 && file.numBitsPerSample != 32 ){
		printf("-- >> ERROR NOT VALID BIT RESOLUTION PER SAMPLE, RESOLUTION IS: %i\n",file.numBitsPerSample);
		return false;
	}

	*(n) = file.sizeUntilNow + (2*4)+(4*3);

	if( !searchId_3Mark(n) ){
		printf("-- >> ERROR NOT VALID HEADER MARK 3\n");
		return false;
	}

	file.numBytesForSampleData = get4BytesNum();

	*(n)+=4;

	return true;

}

static bool writeOutFile(int headerBytesReaded){
	//Variables	
	unsigned char *c = (unsigned char*) malloc (headerBytesReaded-12);
	unsigned int numBytesForSampleData_Out;
	unsigned int fileOutSize;

	// Move file descriptor
	rewind(fd);
	if(fseek(fd,headerBytesReaded,SEEK_CUR) == -1)
		manageSeekErrors(); 
	
	if(fseek(fd_out,headerBytesReaded,SEEK_CUR) == -1) 
		manageSeekErrors();

	// Write samples
	numBytesForSampleData_Out = writeSamples();

	//HEADER
	rewind(fd_out);
	if(fwrite(id_0Mark, sizeof(char), 4,fd_out) != 4) // Riff
		manageReadWriteErrors(fd_out);
	
	fileOutSize = numBytesForSampleData_Out+headerBytesReaded;
	write4BytesNum(fileOutSize);
		
	rewind(fd);
	if(fseek(fd,8,SEEK_CUR) == -1)
		manageSeekErrors();
	
	if(fread(c, sizeof(char),headerBytesReaded-12, fd) != headerBytesReaded-12)
		manageReadWriteErrors(fd);

	if(fwrite(c, sizeof(char), headerBytesReaded-12,fd_out) != headerBytesReaded-12)
		manageReadWriteErrors(fd_out);

 	write4BytesNum(numBytesForSampleData_Out); //numBytesForSampleData_Out
	
	printf("\nWriting new wav file\n");
	printf(" 	fileOutSize: %i\n",fileOutSize);
	printf("	headerBytesReaded: %i\n",headerBytesReaded);
	printf("	numBytesForSampleData_Out: %i\n",numBytesForSampleData_Out);
	printf("	bytesPerSample: %i\n",file.numBitsPerSample/8);
	printf("	numSamples: %i\n",numBytesForSampleData_Out/(file.numBitsPerSample/8));
	
 	free(c);

 	return true;
}


/* Functions called by main */
static void run_parser(){
	int bytesReaded = 0;

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
				verbose = true;	
				break;
		
			default:
				perror("USAGE: ./exec -F <dir Wav file> -O <dir of out file, should end with / ");
				return false;
		}
	}

	if(!ok1 || !ok2 ){
		printf("-- >> YOU HAVE TO SPECIFY THE OUT ADDRESS WITH -O, THE BASE FILE TO READ WITH -F \n\n");
		return false;
	}

	//Open the file
	if ( (fd = fopen(fileName,"r")) == NULL ){
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
	if ( (fd_out = fopen(OutfileName,"w+")) == NULL ){
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
