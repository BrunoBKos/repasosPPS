#include <stdlib.h>
#include <string.h>

struct node_s {
    void* elem;
    struct node_s* next;
    char type;
};
typedef struct node_s node_t;

/*constructor de un nodo*/
node_t* node(void*, char type);

/*devuelve el elemnto de un nodo*/
void* elem(node_t* nodo);

/*libera la memoria ocupada por un nodo*/
void liberar(node_t* nodo);

/*devuelve el siguiente elemento al nodo*/
node_t* getnext(node_t* nodo);

/*cambia el siguiente elemento del nodo*/
void setnext(node_t* nodo,node_t* nx);