#ifndef _WAV_PARSER_H
#define _WAV_PARSER_H

#include <stdio.h>
#include <stdlib.h>

//Biblioteca para ficheros (open)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//To use getopt
#include <unistd.h>

//To operate with strings
#include <string.h>

// To use modff() function
#include <math.h>

#include <stdbool.h>

typedef struct {
	//int id_0; // "Riff"
	unsigned int fileSize; // Size of the file, including header.
	//int id_1; // "WAVE"
	//int id_2; // "fmt "
	int sizeUntilNow;
	short unsigned int format; // 2 bytes
	short unsigned int channels; // 2 bytes
	unsigned int sampleFreq;
	unsigned int averageNumBytesPerSec;
	short unsigned int alin; //2 bytes
	short unsigned int	numBitsPerSample; //2 bytes
	//int id_3; //data
	unsigned int numBytesForSampleData;//4 bytes
}wav_header_t;

//To Fix stuff
#define QN 2    // No lo uso todavia
#define QM 22   // Con valor 23 da un sonido algo mas claro, pero falta...

#define INT24_MAX 0x007FFFFF 
#define INT24_MIN 0xFF800000

#define DOUBLE_TO_FIX(t,d,q) ( (t) ( (d)*(double)(1ULL<<(q)) ))

#define FMUL(a,b,q) ( ((a)*(b))>>(q) )

// Constant
#define NUM_NOTES_PER_OCTAVE 12
#define MAX_CHANNELS 20
#endif