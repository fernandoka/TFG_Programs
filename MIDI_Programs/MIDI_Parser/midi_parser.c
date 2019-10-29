/* Author */
/* Fernando Candelario Herrero */

#include "midi_parser.h"

static FILE *fd;
static midi_file_t file;

/* Examples files */
/* 
	midiFiles/Papermoon.mid
	midiFiles/La_La_Land_-_City_of_Stars.mid
	midiFiles/Etude_C#m_Op1_nº4_Chopin.mid
	midiFiles/Etude_No_33.mid
	midiFiles/Ed_Sheeran_Perfect_Piano_Cover.mid
	midiFiles/AlanWalker-Alone.mid
	midiFiles/LinkingPark-InTheEnd.mid
	midiFiles/Format_0_Example.mid

*/

/* Const variables */
//static const char *fileName = "midiFiles/La_La_Land_-_City_of_Stars.mid"; 
static const char *chOfHeaderChunk = "MThd";
static const char *chOfTrackChunk = "MTrk";

/*Debug variables*/
static const char notes[128][4] = {
	"C-1","C#-1","D-1","D#-1","E-1","F-1","F#-1","G-1","G#-1","A-1","A#-1","B-1",
	"C0","C#0","D0","D#0","E0","F0","F#0","G0","G#0","A0","A#0","B0",
	"C1","C#1","D1","D#1","E1","F1","F#1","G1","G#1","A1","A#1","B1",
	"C2","C#2","D2","D#2","E2","F2","F#2","G2","G#2","A2","A#2","B2",
	"C3","C#3","D3","D#3","E3","F3","F#3","G3","G#3","A3","A#3","B3",
	"C4","C#4","D4","D#4","E4","F4","F#4","G4","G#4","A4","A#4","B4",
	"C5","C#5","D5","D#5","E5","F5","F#5","G5","G#5","A5","A#5","B5",
	"C6","C#6","D6","D#6","E6","F6","F#6","G6","G#6","A6","A#6","B6",
	"C7","C#7","D7","D#7","E7","F7","F#7","G7","G#7","A7","A#7","B7",
	"C8","C#8","D8","D#8","E8","F8","F#8","G8","G#8","A8","A#8","B8",
	"C9","C#9","D9","D#9","E9","F9","F#9","G9"
};

static const char keySigMajor[14][10] = {  
	"Fa Major","Sib Major", "Mib Major","Lab Major", "Reb Major","Solb Major", "Dob Major",
	"Sol Major","Re Major", "La Major","Mi Major", "Si Major","Fa# Major", "Do# Major" 
};

static const char keySigMinor[14][10] = {  
	"Re Minor","Sol Minor", "Do Minor","Fa Minor", "Sib Minor","Mib Minor", "Lab Minor",
	"Mi Minor","Si Minor", "Fa# Minor","Do# Minor", "Sol# Minor","Re# Minor", "La# Minor" 
};

/* Prototipes */
static bool_t setup(int argc, char const *argv[]);
static void run_parser();
static unsigned char getNextByte();
static bool_t checkCharactersOfChunk(const char *ch);
static bool_t readHeaderChunk();
static bool_t readTrackChunk();
static int readLengthOfChunk();
static short int readTwoBytes();
static long int readVarLength(int *numBytesReaded);
static void seekNbytes(int n);
static bool_t readEvents(int length,mtrk_event_t *eventsList);
static void parseEvent(int *length, unsigned char status, mtrk_event_t *event);
static void readTracks();
static void parseMetaEvent(int *lentgh, meta_event_t * meta);
static void parseMidiEvent(int *lentgh, int status, midi_event_t * mid);
static int powerOf2(int n);
static const char* getKeySignature(signed char index,int MajorOrMinor);

/* Debug funcitons */
static void printEvent(mtrk_event_t event);


static void printEvent(mtrk_event_t event){
	
	printf("Delta Time value: %li \n Event type hex: %x \n",event.deltaTime,event.type);

	
		
	if (event.type == META_EVENT){
		
		printf("	Meta Event \n" );
		if( event.meta.type == END_OF_TRACK )
			printf("	End Of Track Event, length value: %li \n",event.meta.length);
		else if( event.meta.type == SET_TEMPO )
			printf("	Set Tempo Event, length value: %li , us: %li",event.meta.length,event.meta.timeUs);
		else if( event.meta.type == TIME_SIGNATURE )
			printf("	Time Signature Event, b1: %x b2:%x , time Signature is: %i/%i\n",event.meta.b1,event.meta.b2,event.meta.b1,powerOf2((int)event.meta.b2));
		else if( event.meta.type == KEY_SIGNATURE )
			printf("	Key Signature Event, b1: %x b2:%i , key signature is: %s\n",event.meta.b1,event.meta.b2, getKeySignature( (signed char)event.meta.b1,(int)event.meta.b2 ) );

	}

	else if (event.type >> 4 == MIDI_EVENT_PC){
		printf("	Program Change");
	}

	else if (event.type >> 4 == MIDI_EVENT_CKP){
		printf("	Channel Key Presurre event");
	}

	else{

		if(event.type >> 4 == MIDI_EVENT_NOTE_OFF || event.type >> 4 == MIDI_EVENT_NOTE_ON ){
					
			if(event.type >> 4 == MIDI_EVENT_NOTE_ON)
				printf("	Note ON event\n");
			else
				printf("	Note Off event\n");

			printf("	Key code %x , Key val %i , vel: %i note: %s\n", event.mid.byte1, event.mid.byte1,event.mid.byte2 ,notes[event.mid.byte1]);
		}
		else if(event.type >> 4 == MIDI_EVENT_CC){
			printf("	Control Change Event \n");
			
			if( event.mid.byte1 == CC_SUSTAIN )
				printf("	Sustain control\n");
			printf("	B1 code %x , B1 val %i , B2 code %x , B2 val %i\n", event.mid.byte1, event.mid.byte1,event.mid.byte2 ,event.mid.byte2);

		}
		else if (event.type == SYSEX_EVENT_1 ||event.type == SYSEX_EVENT_2)
			printf("	Sys_EX_event, length: %li \n",event.sys.length);
		else
			printf("	Not recognized event\n");
	}

	printf("\n\n");
}

/* Aux functions */
static bool_t checkCharactersOfChunk(const char *ch){
	unsigned char s[4];
	short int i = 0;

	while(i < 4)
		s[i++] = getNextByte();

	return s[0] == ch[0] && s[1] == ch[1] && 
			s[2] == ch[2] && s[3] == ch[3]; 
}

static int powerOf2(int n){
	int a = 1;

	for (int i = 0; i < n; ++i){
		a*=2;
	}

	return a;
}

static const char* getKeySignature(signed char index,int MajorOrMinor){
		
	if(index == 0)
		return  MajorOrMinor ? "Do Minor" : "Do Major";

	if(index > 0)
		return  MajorOrMinor ? keySigMinor[index+6] : keySigMajor[index+6];
	else
		return  MajorOrMinor ? keySigMinor[(index * -1) - 1] : keySigMajor[(index * -1) - 1];
	
}

static unsigned char getNextByte(){
	unsigned char c;
	fread(&c,1,1,fd);

	return c;
}


static int readLengthOfChunk(){
	int n,i;

	n = i = 0;
	while(i < 4){
		n = (n << 8) | getNextByte();
		i++;
	}

	return n;
}

static short int readTwoBytes(){
	short int n,i;
	
	n = i = 0;
	while(i < 2){
		n = (n << 8) | getNextByte();
		i++;
	}

	return n;
}

/* Function based on an example in  MIDI 1.0 Detailed Specification v 96.1 3rd Ed. pag 140 */
static long int readVarLength(int *numBytesReaded){
	long int val;
	unsigned char c;
	
	*(numBytesReaded) = 1;

	if ((val = getNextByte()) & 0x80){
		val &= 0x7f;
		do{
			val = (val << 7) + ((c = getNextByte()) & 0x7f);
			*(numBytesReaded)+=1;
		}while(c & 0x80);
	}

	return val;
}

static void seekNbytes(int n){
	fseek(fd,n,SEEK_CUR);
}

/*******************************************************************************************/

/* Parse Functions */
static bool_t readHeaderChunk(){
	bool_t res;

	printf("Reading Header Chunk\n");
	if( !checkCharactersOfChunk(chOfHeaderChunk) ){
		printf("-- >> ERROR NOT VALID CHARACTERS OF CHUNK \n");
		return FALSE;
	}

	//Read length
	file.header.length = readLengthOfChunk();

	//Read format
	file.header.format = readTwoBytes();

	if( file.header.format < 0 || file.header.format > 2 ){
		printf("-- >> ERROR NOT VALID FORMAT \n");
		
		if( file.header.format == 2 )
			printf("-- >> FORMAT 2 IT'S NOT SUPPORTED \n" );
		
		return FALSE;	
	}

	//Read ntrks
	file.header.ntrks = readTwoBytes();

	if( file.header.format == 0 && file.header.ntrks != 1){
		printf("-- >> ERROR HEADER IT'S NOT FOLLOWING THE SPECIFICATION, FORMAT IT'S 0 AND NTRKS: %i \n",file.header.ntrks);
		return FALSE;	
	}

	//Read division
	file.header.division = readTwoBytes();

	res = ( (file.header.division & ~(0x7fff) ) == 0 );

	if(!res)
		printf("Type of divison it's not supported \n");

	return res;
}

static void parseMidiEvent(int *length, int status, midi_event_t * mid){
		//Midi-Events
	if ( status == MIDI_EVENT_NOTE_OFF || status == MIDI_EVENT_NOTE_ON || 
	status == MIDI_EVENT_CC ){
		mid->byte1 = getNextByte();
		mid->byte2 = getNextByte();
		*(length)-=2;
	}

	//Only one data byte
	else if ( (status == MIDI_EVENT_PC) || (status == MIDI_EVENT_CKP) ){
		seekNbytes(1);
		*(length)-=1;
	}

	//The rest of MIDI CHANNEL MESSAGES two data bytes
	else{
		seekNbytes(2);
		*(length)-=2;
	}
}

static void parseMetaEvent(int *length, meta_event_t * meta){
	int bytesReaded,i;

	i = 0;

	meta->type = getNextByte();
	meta->length = readVarLength(&bytesReaded);
	*(length)-=bytesReaded+1;

	if(meta->type != END_OF_TRACK){
		
		if(meta->type == SET_TEMPO){
			
			meta->timeUs = i = 0;
			
			while( i < meta->length ){
				meta->timeUs = (meta->timeUs << 8) | getNextByte();
				i++;
				*(length)-=1; 
			}
		}
		else if(meta->type == KEY_SIGNATURE || meta->type == TIME_SIGNATURE){
			
			// TIME_SIGNATURE always have 4 data bytes
			meta->b1 = getNextByte();
			meta->b2 = getNextByte();
			*(length)-=2;
			
			if(meta->type == TIME_SIGNATURE){
				seekNbytes(2);
				*(length)-=2;
			}
		
		}else{
			seekNbytes(meta->length);
			*(length)-=meta->length;
		}						
			
	}//1º If
}

static void parseEvent(int *length, unsigned char status, mtrk_event_t *event){
	int bytesReaded;

	event->type = status;

	switch(status){

		// Meta-mssg
		case META_EVENT:
			parseMetaEvent( length, &(event->meta) );
			break;
		
		// System_EX
		case SYSEX_EVENT_1 || SYSEX_EVENT_2:
			
			event->sys.length = readVarLength(&bytesReaded);
			seekNbytes(event->sys.length);			
			*(length)-=bytesReaded+event->sys.length;

			break;

		default:
			parseMidiEvent( length, status >> 4, &(event->mid) );

			
	}// Switch
}


static bool_t readEvents(int length, mtrk_event_t *eventsList){
	
	int n,bytesReaded;
	unsigned char status,c;
	
	n = 0;

	while( length > 0 && n < MAX_EVENTS ){

		eventsList[n].deltaTime = readVarLength(&bytesReaded);
		c = getNextByte();
		length-=bytesReaded+1;

		if( c & 0x80 ){
			status = c;
		}
		else{
			seekNbytes(-1);
			length++;
		}

		parseEvent( &length,status,&(eventsList[n]) );
		
		printEvent(eventsList[n]);
		n++;

	}
	
	file.track[file.numTracks].numEvents = n; 

	return eventsList[n-1].type == META_EVENT && eventsList[n-1].meta.type == END_OF_TRACK && 
		eventsList[n-1].meta.length == 0 && n<= MAX_EVENTS;

}


static bool_t readTrackChunk(){

	printf("\n ----------------- Starting Reading Track Chunk ----------------- \n");
	if( !checkCharactersOfChunk(chOfTrackChunk) ){
		printf("-- >> ERROR NOT VALID CHARACTERS OF CHUNK  \n");
		return FALSE;
	}

	file.track[file.numTracks].length = readLengthOfChunk();
	printf("Length of Track Chunk %i\n\n", file.track[file.numTracks].length);

	if( !readEvents(file.track[file.numTracks].length, file.track[file.numTracks].events) )
		printf("-->> LAST EVENT READED IT'S NOT A END OF TRACK EVENT OR LIST OF EVENTS PER TRACK IS FULL\n");

	printf("Nº of events in Track Chunk: %i\n",file.track[file.numTracks].numEvents );
	printf(" ----------------- Track Chunk Readed ----------------- \n");
	file.numTracks++;

	return TRUE;
}


static void readTracks(){
	int i=0;

	if(file.header.ntrks <= MAX_TRACKS){
		while( i < file.header.ntrks){
			if(!readTrackChunk()){
				printf("-- >> ERROR READING TRACK CHUNK Nº: %i\n",i+1 );
				break;
			}
			i++;
		}
		printf("NumTracks: %i\n",file.numTracks);
	}
	else
		printf("-- >> TOO MANY TRACK CHUNK IN THE MIDI FILE, MAX TRACKS CHUNK SUPPORTED: %i\n", MAX_TRACKS);

}

/* Functions called by main */

static void run_parser(){
	
	
	file.numTracks = 0;
	
	if( !readHeaderChunk()){
		printf("-- >> FAIL READING HEADER CHUNK");
		return;
	}

	printf("	Length: %i\n",file.header.length);
	printf("	Format: %i\n",file.header.format);
	printf("	ntrks: %i\n",file.header.ntrks);
	printf("	division: %i\n",file.header.division);

	readTracks();
	
}

static bool_t setup(int argc, char const *argv[]){
	int a;
	char * fileName;
	const char *v = "F:v";

	while( (a = getopt(argc,(char* *const)argv,v)) > 0){
	
		switch( (char)a ){
			case 'F':
				fileName = optarg;
				break;
			case'v': 
				printf("Program that parse a midi file and show some commands by stdout.\n");
				printf("Use with > operator to dump the stdout data in a .txt file\n\n");
				break;
			
			default:
				perror("USAGE: ./exec -F dir Of Midi File\nUSE -v to show some notes");
				return FALSE;
		}
	}

	//Open the file, fd is a global variable
	if ( (fd = fopen(fileName,"r")) < 0 ){
		printf("-- >> FAIL DURING OPEN");
		return FALSE;
	} 

	printf("Reading %s\n", fileName);

	return TRUE;
}


/***********************************/

int main(int argc, char const *argv[]){
	
	if( setup(argc, argv) ) 
		run_parser();
	else
		printf("-- >> SETUP FAILS");
	
	return 0;
}