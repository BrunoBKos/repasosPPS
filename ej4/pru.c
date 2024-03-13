#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fifo.h"

int main () {
    fifo_t* cola;
    char* car = "Hola, me llamo Pablo";
    cola = fifo('s');

    enqueue(cola,&car);

    enqueue(cola,&car);
    dequeue(cola);
    libercola(cola);


    return 0;
}