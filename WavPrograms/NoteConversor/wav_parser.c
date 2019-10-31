/* Author */
/* Fernando Candelario Herrero */

#include "wav_parser.h"


/*Global Variables*/
static FILE *fd;
static FILE *fd_out;	
static wav_header_t file;
static double step; // Num steps of distance between samples
static bool verbose = false;

/* Const variables */
static const char *id_0Mark = "RIFF";
static const char *id_1Mark = "WAVE";
static const char *id_2Mark = "fmt ";
static const char *id_3Mark = "data";

static const char *outpuFileMark = "_cool.wav";


static const double notesFreqMiddle[NUM_NOTES_PER_OCTAVE*7+4] = {	

										27.5000f,29.1353f,30.8677f,

									   	32.7032f,34.6479f,36.7081f,38.8909f,41.2035f,43.6536f,46.2493f,48.9995f,51.9130f,55.0000f,58.2705f,61.7354f,   
									
									   	32.7032f*2,34.6479f*2,36.7081f*2,38.8909f*2,41.2035f*2,43.6536f*2,46.2493f*2,48.9995f*2,51.9130f*2,55.0000f*2,58.2705f*2,61.7354f*2,   
									
									   	32.7032f*4,34.6479f*4,36.7081f*4,38.8909f*4,41.4035f*4,43.6536f*4,46.4493f*4,48.9995f*4,51.9130f*4,55.0000f*4,58.4705f*4,61.7354f*4,    
									   	// Octava Central
										32.7032f*8,34.6479f*8,36.7081f*8,38.8909f*8,41.8035f*8,43.6536f*8,46.8493f*8,48.9995f*8,51.9130f*8,55.0000f*8,58.8705f*8,61.7354f*8,   

										32.7032f*16,34.6479f*16,36.7081f*16,38.8909f*16,41.2035f*16,43.6536f*16,46.2493f*16,48.9995f*16,51.9130f*16,55.0000f*16,58.2705f*16,61.7354f*16,    
									 	
									 	32.7032f*32,34.6479f*32,36.7081f*32,38.8909f*32,41.2035f*32,43.6536f*32,46.2493f*32,48.9995f*32,51.9130f*32,55.0000f*32,58.2705f*32,61.7354f*32,    
									 	
									 	32.7032f*64,34.6479f*64,36.7081f*64,38.8909f*64,41.2035f*64,43.6536f*64,46.2493f*64,48.9995f*64,51.9130f*64,55.0000f*64,58.2705f*64,61.7354f*64,    

									 	32.7032f*128
									 };


/* Prototipes */

// Functions called by main 									 
static bool setup(int argc, char const *argv[]);
static void run_parser();

// Functionality functions
static bool readHeader();
static bool writeOutFile(int headerBytesReaded);

// Aux functions
static bool checkCharacters(const char *ch);
static unsigned int get4BytesNum();
static short unsigned int get2BytesNum();
static bool searchId_3Mark(int *numBytesReaded);

static unsigned char getNextByte();
static void write4BytesNum(unsigned int n);
static void closeFds();
static bool getFreq(const char *s, int l, double *r);

static bool isInteger(double r, int *n, double *d);
	
// Complex functions
static bool seekUntilNSamples(int totalSamplesInFile, int fin, int bytesPerSample, int *ini, int *samplesToRead);
static bool copySamples(int bytesPerSample, int *samplesToRead, int *setIndex, unsigned int *numBytesForSampleData_Out);
static bool linearInterpolation(double decimalPart, int bytesPerSample, int *samplesToRead, unsigned int *numBytesForSampleData_Out, int *setIndex);
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

	if ( (octave == 0 && (s[0] != 'A'|| s[0] != 'B')) || (octave == 8 && (s[0] != 'C')) ) {
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

static bool isInteger(double r, int *n, double *d){
	double x;
	*(d) = modf(r,&x);
	
	*(n) = (int)x;

	return *(d) == 0;
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

/* Complex Functions */
/**
	-- GENERAL NOTES FOR THE FOLLOWING FUNCTIONS --

	@param  bytesPerSample :in
		How many bytes are per sample

	@param samplesToRead :in/out
		How many samples we can read in the file we are reading

	@param setIndex :in/out
		Represents the index of the Sets of samples who have file.channels of lenght

	@param numBytesForSampleData :in/out
		How many bytes the program have just written in the out file
*/

// This function set the value of ini to the same value of fin, and seek the corresponding distance of each index in the in file.
static bool seekUntilNSamples(int totalSamplesInFile, int fin, int bytesPerSample, int *ini, int *samplesToRead){
	
	bool goOut = false;
	int offset;
	int iniFinDiff = ( fin - *(ini) );
	int samplesToShift = file.channels*iniFinDiff;

	if (iniFinDiff < 0)	
		goto goto_mark1;

	else if (iniFinDiff == 0)
		goOut = ( totalSamplesInFile < (-1)*samplesToShift + *(samplesToRead) );
	
	//Case iniFinDiff > 0
	else
		goOut = ( *(samplesToRead) < samplesToShift );


	if(!goOut){

		offset = bytesPerSample*samplesToShift;

		if (fseek(fd,offset,SEEK_CUR) == -1)
			manageSeekErrors();

		*(ini) = fin;
		*(samplesToRead) +=	(-1)*samplesToShift;		 
	}

	goto_mark1:
	return goOut;	
}


static bool copySamples(int bytesPerSample, int *samplesToRead, int *setIndex, unsigned int *numBytesForSampleData_Out){
	
	unsigned char * sample;
	bool goOut;
	int bytesToRW;

	goOut = ( *(samplesToRead) < file.channels );
	
	if(!goOut){

		bytesToRW = bytesPerSample * file.channels;
		sample = (unsigned char*) malloc(sizeof(unsigned char)*bytesToRW);

		//Copy one samplePerChannel			
		if(fread(sample, sizeof(unsigned char),bytesToRW, fd) != bytesToRW)
			manageReadWriteErrors(fd);
			
		if(fwrite(sample, sizeof(unsigned char), bytesToRW,fd_out) != bytesToRW)
			manageReadWriteErrors(fd_out);
	
		*(numBytesForSampleData_Out) += bytesToRW;
		*(setIndex) +=1;
		*(samplesToRead) -= file.channels;

		free(sample);
	}

	return goOut;
}

// Interpolacion
// wtout[i] = wtin[j]+(ci)*(wtin[j+1]-wtin[j])
// j = parte etera de i, ci = i-j (parte decimal de i) 
static bool linearInterpolation(double decimalPart, int bytesPerSample, int *samplesToRead, unsigned int *numBytesForSampleData_Out, int *setIndex){
	
	//unsigned char **sample;
	bool goOut;
	int *wtinJPlus1, *wtinJ;
	int wtoutI;
	int totalSamplesToBeReaded = file.channels*2;
	double wtinJPlus1_f, wtinJ_f, wtoutI_f;

	goOut = ( *(samplesToRead) < totalSamplesToBeReaded );
	

	if( !goOut ){
			
		//sample = (unsigned char**) malloc(sizeof(unsigned char*)*totalSamplesToBeReaded);
		wtinJ = (int*) malloc(sizeof(int)*totalSamplesToBeReaded);
		wtinJPlus1 = (int*) malloc(sizeof(int)*totalSamplesToBeReaded);

		for (int i = 0; i < file.channels; i++)
			if(fread( &(wtinJ[i]), sizeof(char),bytesPerSample, fd) != bytesPerSample)
				manageReadWriteErrors(fd);		
		

		
		for (int i = file.channels; i < totalSamplesToBeReaded; i++)
			if(fread( &(wtinJPlus1[i-file.channels]), sizeof(char),bytesPerSample, fd) != bytesPerSample)
				manageReadWriteErrors(fd);				
		

		// Interpolation double arithmetic
		for (int i = 0; i < file.channels; i++){
			
			//Fix to double conversion
			wtinJ_f = ((double)wtinJ[i])/(1<<23);
			wtinJPlus1_f = ((double)wtinJPlus1[i])/(1<<23);
			
			//double interpolation
			wtoutI_f = wtinJ_f + decimalPart*(wtinJPlus1_f-wtinJ_f);

			if(verbose)
				printf("Interpolated sample value:%f\n", wtoutI_f);
			
			//double to fix conversion
			wtoutI = ((int)wtoutI_f)*(1<<23);

			if(fwrite( &wtoutI, sizeof(char), bytesPerSample,fd_out) != bytesPerSample)
				manageReadWriteErrors(fd_out);		
		}
		
		*(samplesToRead) -= totalSamplesToBeReaded;
		*(setIndex) += 2; 
		*(numBytesForSampleData_Out) += bytesPerSample*file.channels;
		
		// Free memory
		/*for (int i = 0; i < totalSamplesToBeReaded; ++i)
			free(sample[i]);
		*/

		//free(sample);
		free(wtinJ);
		free(wtinJPlus1);
	}

	return goOut;
}

static unsigned int writeSamples(){

	bool goOut = false;
	int bytesPerSample = file.numBitsPerSample/8;
	int totalSamplesInFile = file.numBytesForSampleData/bytesPerSample;
	int samplesToRead = totalSamplesInFile;

	unsigned int numBytesForSampleData_Out = 0;
	double setOutIndex = 0;
	int setIndex = 0;
	int integerPart;

	double decimalPart;

	while( !goOut ){
		
		if ( isInteger(setOutIndex,&integerPart,&decimalPart) ){
			
			goOut = seekUntilNSamples(totalSamplesInFile, setOutIndex, bytesPerSample, &setIndex, &samplesToRead);
			
			if( !goOut )
				goOut = copySamples(bytesPerSample, &samplesToRead, &setIndex, &numBytesForSampleData_Out);
		}
		else{
						
			goOut = seekUntilNSamples(totalSamplesInFile, integerPart, bytesPerSample, &setIndex, &samplesToRead);
		
			if( !goOut )
				goOut = linearInterpolation(decimalPart, bytesPerSample, &samplesToRead, &numBytesForSampleData_Out, &setIndex);

		}

		// Prepare next index
		setOutIndex += step;

		// This is only necesary in this version, when the interpolation will be done this wont be necessary.
		//goOut = goOut || (numBytesForSampleData_Out >= file.numBytesForSampleData);

		
	}//While

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

 	write4BytesNum(numBytesForSampleData_Out);//numBytesForSampleData_Out
	
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
