#include "fifo.h"

fifo_t* fifo(char type) {
    fifo_t* res = (fifo_t*) malloc(sizeof(fifo_t));
    if(res == NULL) {
        return NULL;
    }
    (*res).type = (const char) type;
    return res;
}

int enqueue(fifo_t* list, void* elem) {
    node_t* aux;
    if((list == NULL) | (elem == NULL)) {
        return 1;
    }
    aux = node(elem,(*list).type);
    if(aux == NULL) {
        return 1;
    }
    if((*list).first == NULL) {
        (*list).first = aux;
    }
    else {
        setnext((*list).last,aux);
    }
    (*list).last = aux;
    return 0;
}

int dequeue(fifo_t* list) {
    node_t* aux;
    if(list == NULL) {
        return 1;
    }
    if((*list).first == NULL) {
        return 1;
    }
    aux = (*list).first;
    (*list).first = getnext(aux);
    if((*list).type == 'c' ) {
        fprintf(stdout,"%c",*((char*) elem(aux)));
    }
    else if((*list).type == 'i' ) {
        fprintf(stdout,"%d",*((int*) elem(aux)));
    }
    else if((*list).type == 'f' ) {
        fprintf(stdout,"%f",*((float*) elem(aux)));
    }
    else if((*list).type == 'd' ) {
        fprintf(stdout,"%e",*((double*) elem(aux)));
    }
    else if((*list).type == 's' ) {
        fprintf(stdout,"%s",*((char**) elem(aux)));
    }
    else {
        return 1;
    }
    liberar(aux);
    return 0;
}

void libercola(fifo_t* list) {
    node_t* aux;
    while((*list).first != NULL) {
        aux = (*list).first;
        (*list).first = getnext((*list).first);
        liberar(aux);
    }
    free(list);
}