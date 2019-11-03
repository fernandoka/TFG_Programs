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
#define QN 1		// Integer part

// Constant
#define NUM_NOTES_PER_OCTAVE 12
#define MAX_CHANNELS 20
#endif