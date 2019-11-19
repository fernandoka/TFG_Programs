/* Author */
/* Fernando Candelario Herrero */

#include "wav_parser.h"


/*Global Variables*/
static FILE *fd;
static FILE *fd_out;	
static wav_header_t file;
static double step; // Num steps of distance between samples
static bool verbose = false;
static int QM;

/* Const variables */
static const char *id_0Mark = "RIFF";
static const char *id_1Mark = "WAVE";
static const char *id_2Mark = "fmt ";
static const char *id_3Mark = "data";

static const char *outpuFileMark = "_cool.wav";


static const double notesFreqMiddle[NUM_NOTES_PER_OCTAVE*7+4] = {	

										27.5000f,29.1353f,30.8677f,

									   	32.7032f,34.6479f,36.7081f,38.8909f,41.2035f,43.6536f,46.2493f,48.9995f,51.9130f,55.0000f,58.2705f,61.7354f,   
									
									   	65.4064f,69.2957f,73.4162f,77.7817f,82.4069f,87.3071f,92.4986f,97.9989f,103.826f,110.000f,116.541f,123.471f,   
									
									   	130.813f,138.591f,146.832f,155.563f,164.814f,174.614f,184.997f,195.998f,207.652f,220.000f,233.082f,246.942f,    
									   	// Octava Central
										261.626f,277.183f,293.665f,311.127f,329.628f,349.228f,369.994f,391.995f,415.305f,440.000f,466.164f,493.883f,   

										523.251f,554.365f,587.330f,622.254f,659.255f,698.456f,739.989f,783.991f,830.609f,880.000f,932.328f,987.767f,    
									 	
									 	1046.50f,1108.73f,1174.66f,1244.51f,1318.51f,1396.91f,1479.98f,1567.98f,1661.22f,1760.00f,1864.66f,1975.53f,    
									 	
									 	2093.00f,2217.46f,2349.32f,2489.02f,2637.02f,2793.83f,2959.96f,3135.96f,3322.44f,3520.00f,3729.31f,3951.07f,    

									 	4186.01f
									 };


/* Prototipes */

// Functions called by main 									 
static bool setup(int argc, char const *argv[]);
static void run_parser();

// Functionality functions
static bool readHeader();
static void writeOutFile(int headerBytesReaded);

// Aux functions
static bool checkCharacters(const char *ch);
static int getNBytesNum(int n);
static bool searchId_3Mark(int *numBytesReaded);
static unsigned char getNextByte();

static void closeFds();
static bool getFreq(const char *s, int l, double *r);
static int rawDataToInt(unsigned char *c, int index, int size);
static void intToRawData(unsigned char *c, int n, int index, int size);

// Complex functions
static unsigned int interpolateSamples(int height, int width, int *samples, int *outSamples);
static unsigned int writeSamples();
static int roundCheckOverUnderFlow(long long int n);

// Functions to manage errors, the program can finish because the use of these functions (process will end, use of exit()).
static void manageReadWriteErrors(FILE *localFd);
static void manageFseekErrors();

/* Aux functions */
static bool searchId_3Mark(int *numBytesReaded){
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

static bool getFreq(const char *s, int l, double *r){
	int i=0, j = 1;
	unsigned int octave;

	if( (s[1] != '#' || l !=3)  &&  ( s[1] < 48 || s[1] > 56 || l !=2 ) ){
		printf("-- >> NOT VALID CHARACTER %c \n",s[1]);
		printf("-- >> NOT VALID PATTERN, DONT'T LEAVE SPACES BETWEEN THE CHARACTERS \n");					
		return false;
	}

	if(s[1] =='#'){
		i++;
		j = 2;
	}	

	
	octave = (unsigned char)s[j]-48;

	if ( (octave == 0 && (s[0] != 'A'&& s[0] != 'B')) || (octave == 8 && (s[0] != 'C')) ) {
		printf("-- >> NOT VALID CHARACTER %s. THE LAST NOTES ARE A0,A#0,B0 AND C0 \n",s);		
		return false;
	}

	switch(s[0]){

		case 'C':
			i+=0;
			break;	

		case 'D':
			i+=2;
			break;	

		case 'E':
			i+=4;
			break;	

		case 'F':
			i+=5;
			break;

		case 'G':
			i+=7;
			break;	

		case 'A':
			i += (octave == 0) ? 0 : 9;
			break;

		case 'B':
			i += (octave == 0) ? 2 : 11;
			break;

		default:
			printf("-- >> NOT VALID LETTER %c \n",s[0]);
			return false;
	}

	
	*(r) = (octave != 0) ? notesFreqMiddle[ ((octave-1)*NUM_NOTES_PER_OCTAVE) + 3 + i] : notesFreqMiddle[i];
	
	return true;
		
}

static int roundCheckOverUnderFlow(long long int n){
	int aux;

	n += (long long int)1<<(QM_ARITH-1); 
	aux = (int)(n >> (QM_ARITH));

	if (aux > (int)INT24_MAX)
		aux = (int)INT24_MAX;
	else if(aux < (int)INT24_MIN)
		aux = (int)INT24_MIN;

	return aux;
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

// wtout[j] = wtint[j] + getDecimalPart(ci)*(wtint[j+1]-wtint[j])
// Returns the num of interpolated samples (using the formula and not)
// The fix arithmetic is done in Q 32.32
static unsigned int interpolateSamples(int height, int width, int *samples, int *outSamples){
	long long int fix_step = DOUBLE_TO_FIX(long long int, step, QM_ARITH);
	long long int ci = 0;
	unsigned int integerPart = 0;
	unsigned int decimalPart = 0;
	unsigned int j = 0;

	if( verbose ){
		printf( "\nInterpolation (First channel)\n");
		printf( "--------------------------------------------------------------------\n");
		printf( "       j    wtoutL           ci       j     j+1  wtinL[j] wtinL[j+1]\n");
		printf( "--------------------------------------------------------------------\n");
	}

	do{

		if( decimalPart == 0 ){

			for (int i = 0; i < height; ++i)
				outSamples[i*width+j] = samples[i*width+integerPart];
		}
		else{

			for (int i = 0; i < height; ++i){
				outSamples[i*width+j] = roundCheckOverUnderFlow( ( ((long long int )samples[i*width+integerPart])<<QM_ARITH ) + 
					((long long int)decimalPart*(long long int)(samples[i*width+integerPart+1]-samples[i*width+integerPart]))  );
			}
		}

		if( verbose )
			printf( "%8i%10i%13.5f%8i%8i%10i%11i\n", j, outSamples[j],((double)ci)/4294967296 , integerPart, integerPart+1, samples[integerPart], samples[integerPart+1] );
		

		ci += fix_step;
		integerPart = (unsigned int)(ci>>QM_ARITH); // Take the integer part
		decimalPart = (unsigned int)(ci & 0xFFFFFFFF); // Take the decimal part
		j++;

	}while( integerPart+1 < width ); // Until I can do the interpolation. Width is the number of samples per channel

	return j;
}

static unsigned int writeSamples(){

	unsigned int bytesPerSample = file.numBitsPerSample/8;
	int totalSamplesInFile = file.numBytesForSampleData/bytesPerSample;
	int height = file.channels; 			// How many channels are
	int width = totalSamplesInFile/height;  // How many samples per channel

	unsigned int numOfInterpolatedSamplesPerChannel;
	unsigned char *wavData;
	int *samples, *outSamples;
	int wavDataIndex;
	unsigned int numBytesForInterpolatedSample;
	
	// Read all samples
	wavData = (unsigned char*) malloc(file.numBytesForSampleData);
	if( fread( (void *)wavData, 1, file.numBytesForSampleData, fd ) != file.numBytesForSampleData )
		manageReadWriteErrors(fd);

	// The sample are seen like if there are stored in a matrix
	samples = (int*) malloc(sizeof(int)*totalSamplesInFile);	
	outSamples = (int*) malloc(totalSamplesInFile*sizeof(int));

	wavDataIndex = 0;
	for(int j = 0; j < width; j++)
		for(int i = 0; i < height; i++){
			samples[i*width+j] = rawDataToInt(wavData,wavDataIndex,bytesPerSample);
			wavDataIndex += bytesPerSample;
		}


	//Interpolate samples
	numOfInterpolatedSamplesPerChannel = interpolateSamples(height,width,samples,outSamples);	
	
	wavDataIndex = 0;
	for(int j = 0; j < width; j++)
		for(int i = 0; i < height; i++){
			intToRawData(wavData,outSamples[i*width+j],wavDataIndex,bytesPerSample);
			wavDataIndex += bytesPerSample;
		}

	numBytesForInterpolatedSample = (unsigned int) numOfInterpolatedSamplesPerChannel*bytesPerSample*file.channels;
	if(fwrite( (void *)wavData, 1,numBytesForInterpolatedSample,fd_out) != numBytesForInterpolatedSample) // Riff
		manageReadWriteErrors(fd_out);

	//Free memory
	free(wavData); free(samples); free(outSamples);

	return numBytesForInterpolatedSample;
}

/* Functionality functions */
static bool readHeader(int *n){

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

	QM = file.numBitsPerSample - QN;

	*(n) = file.sizeUntilNow + (2*4)+(4*3);

	if( !searchId_3Mark(n) ){
		printf("-- >> ERROR NOT VALID HEADER MARK 3\n");
		return false;
	}

	file.numBytesForSampleData = (unsigned)getNBytesNum(4);

	*(n)+=4;

	return true;

}

static void writeOutFile(int headerBytesReaded){
	//Variables	
	unsigned char *c = (unsigned char*) malloc (headerBytesReaded);
	unsigned int numBytesForInterpolatedSample;
	unsigned int TotalBytes_out;


	//Coypy HEADER
	rewind(fd);
	if(fread(c, sizeof(unsigned char),headerBytesReaded, fd) != headerBytesReaded)
		manageReadWriteErrors(fd);

	if(fwrite(c, sizeof(unsigned char), headerBytesReaded,fd_out) != headerBytesReaded) // Riff
		manageReadWriteErrors(fd_out);

	// Write samples
	numBytesForInterpolatedSample = writeSamples();

	//Update Header
	if(fseek(fd_out,headerBytesReaded-4,SEEK_SET) == -1)
		manageFseekErrors();
	
	if(fwrite(&numBytesForInterpolatedSample, sizeof(unsigned char), sizeof(unsigned int),fd_out) != sizeof(unsigned int)) // bytesForSampleData
		manageReadWriteErrors(fd_out);
	
	if(fseek(fd_out,4,SEEK_SET) == -1)
		manageFseekErrors();
	
	TotalBytes_out = numBytesForInterpolatedSample+headerBytesReaded;

	if(fwrite(&TotalBytes_out, sizeof(unsigned char), sizeof(unsigned int),fd_out) != sizeof(unsigned int)) // totalBytesInFile
		manageReadWriteErrors(fd_out);


	printf("\nWriting new wav file\n\n");
	printf(" 	File Out Size: %i\n",TotalBytes_out);
	printf("	Header Bytes Readed: %i\n",headerBytesReaded);
	printf("	Total Num Bytes For Samples: %i\n\n",file.numBytesForSampleData);

	printf("	Bytes PerSample: %i\n",file.numBitsPerSample/8);
	printf("	Num Bytes For Interpolated Samples: %i\n",numBytesForInterpolatedSample);
	printf("	Num Interpolated Samples: %i\n\n",numBytesForInterpolatedSample/(file.numBitsPerSample/8));

 	printf("	Num Bytes For Samples Lost: %i\n",file.numBytesForSampleData-numBytesForInterpolatedSample);	
 	printf("	Num Samples Lost: %i\n",(file.numBytesForSampleData-numBytesForInterpolatedSample)/(file.numBitsPerSample/8));

 	free(c);
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
	const char *v = "F:O:S:T:B:v";
	bool ok1,ok2,ok3,ok4,goOut,startCount;
	int lenOfFileName,posOfForwardSlash,cont;
	char *note;
	double targetFreq,baseFreq;

	fileName = dirOutFile = Fname = NULL;
	startCount = ok1 = ok2 = ok3 = ok4 = goOut = false;
	
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
			case 'T':
				note = optarg;
				aux1 = (int)strlen(note);

				if(aux1 > 3 || aux1 < 2){
					printf("-T argument should be a note like (C4, C#4, B4,...)\n");
					perror("-- >>  ERROR");
					return false;
				}

				if( !getFreq(note,aux1,&targetFreq) ){
					printf("Fails getting targetFreq, must be a note like (C4, C#4, B4,....,C0)\n");
					perror("-- >>  ERROR");
					return false;
				}

				ok3 = true;
				break;

			case 'B':
				note = optarg;
				aux1 = (int)strlen(note);

				if(aux1 > 3 || aux1 < 2){
					printf("-B argument should be a note like (C4, C#4, B4,...)\n");
					perror("-- >>  ERROR");
					return false;
				}

				if( !getFreq(note,aux1,&baseFreq) ){
					printf("Fails getting baseFreq, must be a note like (C4, C#4, B4,....,C0)\n");
					perror("-- >>  ERROR");
					return false;
				}

				ok4 = true;
				break;

			case'v': 
				verbose = true;
				break;
		
			default:
				perror("USAGE: ./exec -F <dir Wav file> -O <dir of out file, should end with / -T target frequency -B base frequency> \nUSE -v to show the value of the file out samples");
				return false;
		}
	}

	if(!ok1 || !ok2 || !ok3 || !ok4){
		printf("-- >> YOU HAVE TO SPECIFY THE OUT ADDRESS WITH -O, THE BASE FILE TO READ WITH -F \nTHE BASE FREQ THAT IS SAVED IN THE BASE FILE WITH -B, AND THE TARGET FREQ TO GET\n");
		return false;
	}

	if( targetFreq < baseFreq ){
		printf("-- >> THE TARGET FREQ ( %f ) MUST BE BIGGER THAN BASE FREQ ( %f )\n", targetFreq,baseFreq);
		return false;
	}

	// The steps to do
	step = targetFreq/baseFreq;

	printf("\nTarget Freq: %f  baseFreq: %f\n\n",targetFreq,baseFreq );
	printf("Steps: %f\n\n", step);

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
