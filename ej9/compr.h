#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cola_s {
    int elems; 
    int lnths[4];
    int dists[4];
}; typedef struct cola_s cola_t;

struct cmprsor_s {
    unsigned int pos_lns;
    char lineas[37888];
    unsigned long pos_cmpr;
    char text_comp[2048]; 
}; typedef struct cmprsor_s cmprsor_t;

int comprime(FILE* in, FILE* out);

int descomprime(FILE* in, FILE* out);

void metedentroconlosbits(int e[2], FILE* out, cmprsor_t* cmpr, int cs);

int buscaMax(char* str, int from, int to, int* pos);

int coinCar(char* str,char* str2, int to);

int compara(cola_t* cola, int e[2]);

void enqueue(cola_t* cola, int e[2]);

int sacacola(cola_t* cola,int e);

int sacafueraconlosbits(cmprsor_t* cmpr, cola_t* cola);