#include "node.h"

node_t* node(void* e, char type) {
    node_t *res;
    res = (node_t*) malloc(sizeof(node_t));
    if(res == NULL) {
        return NULL;
    } 
    (*res).type = type;
    if(type == 'c') {
        (*res).elem = (char*) malloc(sizeof(char));
        if((*res).elem == NULL) {
            return NULL;
        }
        *((char*) (*res).elem) = *((char*) e);
    }
    else if(type == 'i') {
        (*res).elem = (int*) malloc(sizeof(int));
        if((*res).elem == NULL) {
            return NULL;
        }
        *((int*) (*res).elem) = *((int*) e);
    }
    else if(type == 'd') {
        (*res).elem = (double*) malloc(sizeof(double));
        if((*res).elem == NULL) {
            return NULL;
        }
        *((double*) (*res).elem) = *((double*) e);
    }
    else if(type == 'f') {
        (*res).elem = (float*) malloc(sizeof(float));
        if((*res).elem == NULL) {
            return NULL;
        }
        *((float*) (*res).elem) = *((float*) e);
    }
    else if(type == 's') {
        (*res).elem = (char**) malloc(sizeof(char*));
        if((*res).elem == NULL) {
            return NULL;
        }
        *((char**) (*res).elem) = (char*) malloc(sizeof(char)*(strlen((char*) e)+1));
        if(*((char**) (*res).elem) == NULL) {
            return NULL;
        }
        strcpy(*((char**) (*res).elem),(char*) e);
    }
    else {
        return NULL;
    }
    return res;
}


void* elem(node_t* nodo) {
    if(nodo == NULL) {
        return NULL;
    }
    if((*nodo).type == 's') {
        return *((void**) (*nodo).elem);
    }
    else {
        return (*nodo).elem;
    }
}

void liberar(node_t* nodo) {
    if(nodo == NULL) {
        return;
    }
    if((*nodo).type == 's') {
        free(*((char**) (*nodo).elem));
    }
    free((*nodo).elem);
    free(nodo);
}

node_t* getnext(node_t* nodo) {
    if(nodo == NULL) {
        return NULL;
    }
    return (*nodo).next;
}
void setnext(node_t* nodo,node_t* nx) {
    if(nodo == NULL) {
        return;
    }
    (*nodo).next = nx;
}