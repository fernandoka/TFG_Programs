
Para ejecutar el programa se debe dar como argumentos la ubicacion del archivo .wav base ( argumento -F ), la direccion en donde se generara el nuevo archivo ( argumento -O, el archivo de salida tendra el mismo nombre que el archivo de entrada salvo por una terminacion "_cool" antes del formato. Si la ruta no termina marcando una dirección, los caracteres sobrantes se concadenaran al nombre del archivo de salida ). 

Se debe añadir también la frecuencia objetivo ( argumento -T ) y la frecuencia base ( argumento -B ), ambas se definiran como notas siguiendo la codificación americana ( Do:C, Re:D, Mi:E, Fa:F, Sol:G, La:A, Si:B ). La aplicacion contine las frecuencias de todas las notas del piano ( 88 notas ), comenzando por A0 y terminando en C8 ( el numero indica la octava ). La frecuencia base es la frecuencia que esta almacenada en el archivo .wav de lectura, el programa no detecta la nota leyendo el archivo .wav debe indicarse, si se indica mal la frecuencia base, no se obtnedra el resultado esperado :) 


Comandos ejemplo:

	# argumentos utiles para debugear con gdb:
	
	r -F ../../SalamanderGrandPianoV3_48khz24bit/48khz24bit/C4v8.wav -O oopio -T C#4 -B C4
	
	
	
	Notas de estrellita:
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A4v8-2.wav -O AllNotes/A4_ -T A4 -B A4
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/F#4v8-2.wav -O AllNotes/G4_ -T G4 -B F#4
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/D#4v8-2.wav -O AllNotes/F4_ -T F4 -B D#4
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/D#4v8-2.wav -O AllNotes/E4_ -T E4 -B D#4
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/C4v8-2.wav -O AllNotes/D4_ -T D4 -B C4
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/C4v8-2.wav -O AllNotes/C4_ -T C4 -B C4
	
	
	
	Todas las notas:
	//La
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A0v8-2.wav -O AllNotes/A#0_ -T A#0 -B A0
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A0v8-2.wav -O AllNotes/B0_ -T B0 -B A0
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A1v8-2.wav -O AllNotes/A#1_ -T A#1 -B A1
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A1v8-2.wav -O AllNotes/B1_ -T B1 -B A1
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A2v8-2.wav -O AllNotes/A#2_ -T A#2 -B A2
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A2v8-2.wav -O AllNotes/B2_ -T B2 -B A2
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A3v8-2.wav -O AllNotes/A#3_ -T A#3 -B A3
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A3v8-2.wav -O AllNotes/B3_ -T B3 -B A3
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A4v8-2.wav -O AllNotes/A#4_ -T A#4 -B A4
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A4v8-2.wav -O AllNotes/B4_ -T B4 -B A4
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A5v8-2.wav -O AllNotes/A#5_ -T A#5 -B A5
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A5v8-2.wav -O AllNotes/B5_ -T B5 -B A5
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A6v8-2.wav -O AllNotes/A#6_ -T A#6 -B A6
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A6v8-2.wav -O AllNotes/B6_ -T B6 -B A6
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A7v8-2.wav -O AllNotes/A#7_ -T A#7 -B A7
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/A7v8-2.wav -O AllNotes/B7_ -T B7 -B A7
	
	
	//Do
	./wav_parser_op3.elf -F ../SameSize-1.06MB/C1v7-2.wav -O AllNotes/C#1_ -T C#1 -B C1
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/C1v7-2.wav -O AllNotes/D1_ -T D1 -B C1
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/C2v8-2.wav -O AllNotes/C#2_ -T C#2 -B C2
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/C2v8-2.wav -O AllNotes/D2_ -T D2 -B C2
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/C3v8-2.wav -O AllNotes/C#3_ -T C#3 -B C3
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/C3v8-2.wav -O AllNotes/D3_ -T D3 -B C3
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/C4v8-2.wav -O AllNotes/C#4_ -T C#4 -B C4
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/C4v8-2.wav -O AllNotes/D4_ -T D4 -B C4
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/C5v8-2.wav -O AllNotes/C#5_ -T C#5 -B C5
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/C5v8-2.wav -O AllNotes/D5_ -T D5 -B C5
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/C6v8-2.wav -O AllNotes/C#6_ -T C#6 -B C6
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/C6v8-2.wav -O AllNotes/D6_ -T D6 -B C6
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/C7v8-2.wav -O AllNotes/C#7_ -T C#7 -B C7
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/C7v8-2.wav -O AllNotes/D7_ -T D7 -B C7
	
	
	//Re#
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/D#7v8-2.wav -O AllNotes/E7_ -T E7 -B D#7
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/D#7v8-2.wav -O AllNotes/F7_ -T F7 -B D#7
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/D#6v8-2.wav -O AllNotes/E6_ -T E6 -B D#6
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/D#6v8-2.wav -O AllNotes/F6_ -T F6 -B D#6
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/D#5v8-2.wav -O AllNotes/E5_ -T E5 -B D#5
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/D#5v8-2.wav -O AllNotes/F5_ -T F5 -B D#5
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/D#4v8-2.wav -O AllNotes/E4_ -T E4 -B D#4
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/D#4v8-2.wav -O AllNotes/F4_ -T F4 -B D#4
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/D#3v8-2.wav -O AllNotes/E3_ -T E3 -B D#3
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/D#3v8-2.wav -O AllNotes/F3_ -T F3 -B D#3
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/D#2v8-2.wav -O AllNotes/E2_ -T E2 -B D#2
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/D#2v8-2.wav -O AllNotes/F2_ -T F2 -B D#2
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/D#1v8-2.wav -O AllNotes/E1_ -T E1 -B D#1
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/D#1v8-2.wav -O AllNotes/F1_ -T F1 -B D#1
	
	
	//Fa#
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/F#1v8-2.wav -O AllNotes/G1_ -T G1 -B F#1
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/F#1v8-2.wav -O AllNotes/G#1_ -T G#1 -B F#1
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/F#2v8-2.wav -O AllNotes/G2_ -T G2 -B F#2
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/F#2v8-2.wav -O AllNotes/G#2_ -T G#2 -B F#2
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/F#3v8-2.wav -O AllNotes/G3_ -T G3 -B F#3
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/F#3v8-2.wav -O AllNotes/G#3_ -T G#3 -B F#3
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/F#4v8-2.wav -O AllNotes/G4_ -T G4 -B F#4
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/F#4v8-2.wav -O AllNotes/G#4_ -T G#4 -B F#4
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/F#3v8-2.wav -O AllNotes/G3_ -T G3 -B F#3
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/F#3v8-2.wav -O AllNotes/G#3_ -T G#3 -B F#3
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/F#2v8-2.wav -O AllNotes/G2_ -T G2 -B F#2
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/F#2v8-2.wav -O AllNotes/G#2_ -T G#2 -B F#2
	
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/F#1v8-2.wav -O AllNotes/G1_ -T G1 -B F#1
	
	./wav_parser_op3.elf -F ../SameSize-1.06MB/F#1v8-2.wav -O AllNotes/G#1_ -T G#1 -B F#1
	
