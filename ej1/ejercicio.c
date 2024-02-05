#include "ejercicio.h"


int main(int argc, char** argv) {
    FILE *fp;
    nodo_t* lklist;
    int rp = 1;
    if(argc < 3) {
        return 1;
    }

    fp = fopen(argv[1],"r");
    if(fp == NULL) {
        return rp;
    }
    lklist = cargar(fp);
    fclose(fp);
    if(lklist == NULL) {
        return rp;
    }
    fp = fopen(argv[2],"w");
    if(fp != NULL) {
        rp = pasar(lklist,fp);
        fclose(fp);
    }

    vaciar(lklist);

    return rp;
}

nodo_t* cargar(FILE* fp) {
    nodo_t  *nodo, *aux;
    char linea[TAM + 1];

    nodo = (nodo_t*) malloc(sizeof(nodo_t));
    if(nodo == NULL) {
        return nodo;
    }
    aux = nodo;
    if(fgets(linea, TAM + 1, fp) == NULL) {
        return NULL;
    }
    strncpy((*aux).linea, linea, TAM);
    while(fgets(linea, TAM + 1, fp) != NULL) {
        (*aux).next = (nodo_t*) malloc(sizeof(nodo_t));
        aux = (*aux).next;
        if(aux == NULL) {
            break;
        }
        strncpy((*aux).linea, linea, TAM);
    }
    if(aux != NULL) {
        (*aux).next = NULL;
        return nodo;
    }
    return NULL;
}

int pasar(nodo_t* nodo, FILE* fp) {
    nodo_t* aux;
    aux = nodo;
    while(aux != NULL) {
        fprintf(fp, "%s",(*aux).linea);
        aux = (*aux).next;
    }
    return 0;
}

void vaciar(nodo_t* nodo) {
    if(nodo == NULL) {
        return;
    }
    vaciar((*nodo).next);
    free(nodo);
    return;
}