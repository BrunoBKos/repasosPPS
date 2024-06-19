#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct half_s {
    unsigned char val[2];
}; typedef struct half_s half_t;

/* comprime */

int comprime(char* src, char* dest);

/* descomprime */

int descomprime(char* src, char* dest);

/*coinciden caracteres de dos cadenas*/
int coinCar(char* str,char* str2, int to);

int buscaMax(char* str, int from, int to, int* pos);