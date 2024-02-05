#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM 2048
struct nodo_s {
    char linea[2048];
    struct nodo_s* next;
};
typedef struct nodo_s nodo_t;

/*cargado de las lineas del primer fichero en una linked list*/
nodo_t* cargar(FILE* fp);

/*volcado de lineas en el segundo fichero*/
int pasar(nodo_t* nodo, FILE* fp);

/*vaciado de la linked list*/
void vaciar(nodo_t* nodo);

int main(int argc, char** argv);
