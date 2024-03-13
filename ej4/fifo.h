#include <stdio.h>
#include "node.h"

struct fifo_s {
    node_t* first;
    node_t* last;
    char type;
};
typedef struct fifo_s fifo_t;

/*costructor de la fifo*/
fifo_t* fifo(char type);

/*mete el elemento en la cola*/
int enqueue(fifo_t* list, void* elem);

/*saca el elemnto de la cola y lo imprime por salida estandar*/
int dequeue(fifo_t*);

/*liberar la memoria de la cola*/
void libercola(fifo_t*);