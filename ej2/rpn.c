#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void procesar_operaciones(FILE *);

int main(int argc, char** argv) {
    int ok = 0;
    FILE* fp;
    if(argc > 1) {
        fp = fopen(argv[1],"r");
        if(fp == NULL) {
            ok = 1;
            printf("No se puede abrir el archivo: %s\n",argv[1]);
        }
        else {
            printf("Se ha abierto el archo: %s\n",argv[1]);
            procesar_operaciones(fp);
            printf("Se ha cerrado el archivo: %s\n",argv[1]);
            fclose(fp);
        }
    }
    else {
        procesar_operaciones(stdin);
    }
    return ok;
}

int leer_numero_u_operacion(FILE* f, double *num, char *operacion) {
    if(fscanf(f,"%lf",num) ==  1) {
        return 1;
    }
    else if(fscanf(f,"%31s",operacion) == 1) {
        return 2;
    }
    return 0;
}

typedef struct {
    double *array;
    unsigned tam;
    unsigned len;
} rpn_t; 

int inicializar(rpn_t* ops) { 
    (*ops).tam = 8;
    (*ops).len = 0;
    (*ops).array = (double*) malloc(sizeof(double)*((*ops).tam));
    if((*ops).array == NULL) {
        printf("No hay espacio");
        return 0;
    }
    return 1;
}

int guardar_numero(double num, rpn_t *ops) { 
    double* aux = NULL;
    if((*ops).len == ((*ops).tam)) {
        (*ops).tam = (*ops).tam*2;
        aux = (double*) realloc((*ops).array, sizeof(double)*(*ops).tam);
        if(aux == NULL) {
            printf("Falta memoria");
            return 0;
        }
        (*ops).array = aux;
    }
    (*ops).array[(*ops).len] = num;
    (*ops).len += 1;
    return 1;
}

double suma_todos(rpn_t *ops) { 
    double res = 0;
    unsigned int i;
    for(i = 0;i < (*ops).len;i++) {
        res += (*ops).array[i];
    }
    (*ops).len = 0;
    return res;
}

void mostrar(rpn_t* ops) {
    unsigned int i;
    fprintf(stderr,"[ ");
    for ( i = 0; i < ops->len; i++ ) { 
        fprintf(stderr, "%.2f ", ops->array[i]); 
    }
    fprintf(stderr,"]\n");
}

double ultimo(rpn_t* ops) {
    return ops->array[ops->len - 1];
}

void realizar_operacion(char *operacion, rpn_t *ops) {
    double x = 0;
    mostrar(ops);
    printf("Haciendo: %s => ", operacion);
    x = (*ops).array[((*ops).len-2)];
    if(strcmp(operacion,"+") == 0) {
        x = x + ops->array[(*ops).len-1];
        ops->array[((*ops).len-2)] = x;
    }
    if(strcmp(operacion,"-") == 0) {
        x = x - (*ops).array[(*ops).len-1];
        ops->array[((*ops).len-2)] = x;
    }
    if(strcmp(operacion,"*") == 0) {
        x = x * ops->array[(*ops).len-1];
        ops->array[(*ops).len-2] = x;
    }
    if(strcmp(operacion,"/") == 0) {
        x = x / ops->array[(*ops).len-1];
        (*ops).array[(*ops).len-2] = x;
    }
    (*ops).len = (*ops).len - 1;
    if(strcmp(operacion,"suma_todos") == 0){
        (*ops).len = (*ops).len + 1;
        (*ops).array[0] = suma_todos(ops);
        (*ops).len = 1;
    }
    printf("Resultado %.2f\n", ultimo(ops));
}

void liberar(rpn_t *ops) {
    if (ops->array)
    free(ops->array);
}

void procesar_operaciones(FILE *f) {
    double num = 0;
    int aux;
    char operacion[32];
    rpn_t ops;
    aux = inicializar(&ops);
    while(aux) {
        aux = leer_numero_u_operacion(f, &num, operacion);
        if(aux == 1) {
            aux = guardar_numero(num, &ops);
        }
        else if(aux == 2) {
            realizar_operacion(operacion, &ops);
        }
    }
    liberar(&ops);
}