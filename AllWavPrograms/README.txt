El archivo C4v8.bin si se abre con sublime Text u visual studio code, se muestra la cabecera del archvio C4v8.wav ( el original ), tal y como yo he interpretado el archivo para poder interpolar.
Las muestras se agrupan en conjuntos ( sets ), cada conjunto tiene tantas muestras como canales tenga el archivo. El programa lo que hace es leer dos conjuntos e interpolar las muestras correspondientes de cada canal.
En el archivo de salida se escribira un conjunto ( tantas muestras como canales tenga el archivo ) por cada 2 conjuntos del archivo de entrada. Habra conjuntos del archivo de entrada que se leeran más de una vez,
de ahí la utilidad de la función seekUntilNSamples()( línea 308 ). 
