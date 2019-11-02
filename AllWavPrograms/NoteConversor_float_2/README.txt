
Para ejecutar el programa se debe dar como argumentos la ubicacion del archivo .wav base ( argumento -F ), la direccion en donde se generara el nuevo archivo ( argumento -O, el archivo de salida tendra el mismo nombre que el archivo de entrada salvo por una terminacion "_cool" antes del formato. Si la ruta no termina marcando una dirección, los caracteres sobrantes se concadenaran al nombre del archivo de salida ). 

Se debe añadir también la frecuencia objetivo ( argumento -T ) y la frecuencia base ( argumento -B ), ambas se definiran como notas siguiendo la codificación americana ( Do:C, Re:D, Mi:E, Fa:F, Sol:G, La:A, Si:B ). La aplicacion contine las frecuencias de todas las notas del piano ( 88 notas ), comenzando por A0 y terminando en C8 ( el numero indica la octava ). La frecuencia base es la frecuencia que esta almacenada en el archivo .wav de lectura, el programa no detecta la nota leyendo el archivo .wav debe indicarse, si se indica mal la frecuencia base, no se obtnedra el resultado esperado :)
 
******************** PARTICULARIDAD DE ESTA VERSIÓN *************************

Esta versión realiza la interpolación realizando aritmetica de punto floatante de precisión simple (float)
Los archivos .wav generados con esta aplicación son parecidos a los generados por la versión con aritmética de doble precisión.

*****************************************************************************

Comandos ejemplo:

El archivo C4v8_cool.wav se generado apartir del siguiente comando:
	
	./wav_parser_op3.elf -F notasBuenas/C4v8.wav -O notasFlama/ -T C#4 -B C4

El archivo A4v8_cool.wav se generado apartir del siguiente comando:

	./wav_parser_op3.elf -F notasBuenas/A4v8.wav -O notasFlama/ -T B4 -B A4

# argumentos utiles para debugear con gdb:

r -F ../../SalamanderGrandPianoV3_48khz24bit/48khz24bit/C4v8.wav -O oopio -T C#4 -B C4
