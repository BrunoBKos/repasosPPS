#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct map_s {
    void* table[1024];
    unsigned long(*hash)(void* key);
    int(*equals)(void* one,void* two);
}; typedef struct map_s map_t;

struct lista_s {
    int tam;
    unsigned long* hss;
    void** elems;
}; typedef struct lista_s lista_t;

/*newmap*/
map_t* newmap(unsigned long(*hash)(void* key), int(*equals)(void* one,void* two));

/*put*/
int put(map_t* map, void* key, void* val);

/*get*/
void* get(map_t* map, void* key);

/*rm devuelve 1 si no se ha podido eliminar y 0 en caso contrario*/
int rm(map_t* map, void* key);

/*binarysearch para buscar un elemento en la lista*/
int binarysearch(lista_t* list, unsigned long elem);

int eq(void* one,void* two);

unsigned long hs(void* key);