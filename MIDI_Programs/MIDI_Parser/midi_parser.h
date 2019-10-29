/* Author */
/* Fernando Candelario Herrero */


#ifndef _MIDI_PARSER_H
#define _MIDI_PARSER_H

#include <stdio.h>
#include <stdlib.h>

//Biblioteca para ficheros (open)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//My stdbool
#define TRUE 1
#define FALSE 0

//To use getopt
#include <unistd.h>

typedef short int bool_t;

typedef struct {
	int length; // Of the header
	short int format; // Format of the MIDI file
	short int ntrks; // Nº of Tracks Chunks
	short int division; // meaning of delta times
}header_chunk_t;


//Program change y Channel key pressure le acompaña solo un byte de datos por definicion
//El resto de midi channel messages tienen 2 bytes de datos
typedef struct {
	unsigned char byte1; // If IT's a note on/off message represents the key

	unsigned char byte2; // If IT's a note on/off message represents the vel
						 // If It's a control Change message represent controller value ( for sustian ≤63 off, ≥64 on)
}midi_event_t;

typedef struct {
	long int length; // VarLengthQuantity. It's needed to move the read-pointer of the file.
}sysex_event_t;

#define END_OF_TRACK 0x2f // In the file it's coded like 0x2f, always have 00 length
#define SET_TEMPO 0x51 // In the file it's coded like 0x51
#define TIME_SIGNATURE 0x58 // length of 4
#define KEY_SIGNATURE 0X59 // length of 2
typedef struct {
	unsigned char type; // END_OF_TRACK | SET_TEMPO
	long int length; // VarLengthQuantity
	long int timeUs; // Tempo in us/MIDI quarterNote. 500000 us = 120bpm
	unsigned char b1; // Time signature numerator | flats number
	unsigned char b2; // Time signature denominator power of 2 | sharps number
}meta_event_t;


#define MIDI_EVENT_NOTE_OFF 0x8 // Any MIDI channel message, the first byte
#define MIDI_EVENT_NOTE_ON 0X9 
#define MIDI_EVENT_PC 0xc // Program change, one data byte
#define MIDI_EVENT_CKP 0xd // Channel Key Pressure, one data byte
#define MIDI_EVENT_CC 0xb // Control Change
	#define CC_SUSTAIN 0X40 



#define SYSEX_EVENT_1 0xf0 // System exclusive message
#define SYSEX_EVENT_2 0xf7
#define META_EVENT 0xff // Meta data (Tempo,End of Track,Lyrics...)
typedef struct {
	long int deltaTime;
	unsigned char type; // MIDI_EVENT ( NOTE_ON | NOTE_OFF | PR | CKH | CC) | SYSEX_EVENT | META_EVENT
	midi_event_t mid;
	sysex_event_t sys;
	meta_event_t meta;
}mtrk_event_t;


#define MAX_EVENTS 10000
typedef struct {
	int length; // Of the track
	int numEvents;
	mtrk_event_t events[MAX_EVENTS];
}track_chunk_t;



#define MAX_TRACKS 50
typedef struct {
	int numTracks;
	header_chunk_t header;
	track_chunk_t track[MAX_TRACKS];
}midi_file_t;

#endif