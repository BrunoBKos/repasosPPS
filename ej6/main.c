#include "map.c"

int main( void ) {
    int ok,i;
    int* aux;
    int keys[1024];
    int vals[1024];
    map_t* mapa;
    i = 0;
    ok = 0;
    for(i = 0; i < 1024; i++) {
        vals[i] = i;
    }
    i = 0;
    mapa = newmap(NULL,NULL);
    fprintf(stdout,"%ld ",sizeof(int));
    while(ok == 0 && i < 1024) {
        put(mapa,&keys[i],&vals[i]);
        i++;
    }
    if(ok == 1) {
        fprintf(stdout,"%d",i);
    }
    i = 0;
    while(ok == 0 && i < 1024) {
        aux = (int*) get(mapa,&keys[i]);
        
        if(aux == NULL) {
            fprintf(stdout,"mal");
        }
        else {
            fprintf(stdout,"%d ",*aux);
        }
        i++;
    }
    free(mapa);
    return ok;
}