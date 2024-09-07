#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*estructuras a usar*/
/*  char cmpr[2048];
    char txt[2048];
    int pos_cmpr,pos_txt;
    unsigned int tam = 0; contador de los caracteres leidos del fichero*/
/*  int lsfr[4368]; lista para las frecuencias*/
/*  char lsdr[4367]; lista para las direciones asociadas a las frecuencias*/ 
/*  int map[65536]; mapa de posiciones de los caracteres*/

/*funcines principales*/

/*funcion principal de compresion*/
void comprime(FILE* in, FILE* out);

/*funcion principal de descompresion*/
void descomprime(FILE* in, FILE* out);

/*funciones auxiliares*/

/*hacer comprobaciones*/
int metedentroconlosbits(FILE* out,char cmpr[2048],unsigned char e,unsigned long pos_cmpr, int cs);

/*retocar intenso*/
int sacafueraconlosbits(char cmpr[3072], char txt[2048], unsigned long pos_cmpr, unsigned int pos_txt,
                        int map[65536],int lsfr[4368],int lsdr[4367]);

/*funcion de analisis para la compresion*/
int analisis(int map[65536],int lsfr[4368],int lsdr[4367], unsigned char e[3]);

/*funcion auxiliar para aumentar actualizar la frecuencia de uan direccion*/
void aumentaFr(int map[65536],int lsfr[4368],int lsdr[4367], int dr);