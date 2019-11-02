
Para ejecutar el programa se debe dar como argumentos la ubicacion del archivo .wav base ( argumento -F ), la direccion en donde se generara el nuevo archivo ( argumento -O, el archivo de salida tendra el mismo nombre que el archivo de entrada salvo por una terminacion "_copy" antes del formato. Si la ruta no termina marcando una dirección, los caracteres sobrantes se concadenaran al inicio del nombre del archivo de salida ). 

Este programa solo copia los datos de un archivo .wav a otro con distinto nombre. Para copiarlo realiza la conversión correcta de pasar el valor de una muestra a entero mediante el uso de funciones.

Comando Ejemplo:

./copy_wav_op3.elf -F ../../../Documentos/SalamanderGrandPianoV3_48khz24bit/48khz24bit/C4v8.wav -O aa
