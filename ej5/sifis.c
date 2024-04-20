#include "sifis.h"

int main (int argc, char** argv) {
    FILE* fp;
    FILE* salida;
    pag_t* tp;
    if(argc < 3) {
        return 1;
    }
    if(argv[1][0] != 'm') {
        return 1;
    }
    if(argc < 4) {
        return 1;
    }
    fp = fopen(argv[2],"r");
    if(fp == NULL) {
        return 1;
    }
    salida = fopen(argv[3],"w");
    if(salida == NULL) {
        return 1;
    }
    tp = cargar(fp);
    if(tp == NULL){
        fclose(fp);
        fclose(salida);
        return 1;
    }
    if(1 == push(tp,salida)) {
        liberar(tp);
        fclose(salida);
        fclose(fp);
        return 1;
    }
    liberar(tp);
    fclose(fp);
    fclose(salida);
    return 0;
}

pag_t* new_pag() {
    pag_t* res;
    int i;
    res = (pag_t*) malloc(sizeof(pag_t));
    if(res == NULL) {
        return NULL;
    }
    for(i = 0; i < 26; i++) {
        (*res).ent[i] = NULL;
    }
    return res;
} 


pag_t* cargar(FILE* fp) {
    pag_t* res;
    char linea[2048];
    int ok = 0; 
    res = new_pag();
    if(res == NULL) {
        ok = 1;
    }
    while ((ok == 0) && fgets(linea,2048,fp)) {
        ok = meter(res,linea);
    }
    if(ok == 1) {
        return NULL;
    }
    return res;
}

int meter(pag_t* res,char* linea) {
    int i, j, k;
    i = linea[0] - 'A';
    if((i < 0) || (i > 25)) {
        return 1;   
    } 
    j = linea[1] - 'A';
    if((j < 0) || (j > 25)) {
        return 1;   
    } 
    k = linea[2] - 'A';
    if((k < 0) || (k > 25)) {
        return 1;   
    } 
    if((*res).ent[i] == NULL) {
        (*res).ent[i] = new_pag();
        if((*res).ent[i] == NULL) {
            return 1;
        }
    }
    if((*((pag_t*) (*res).ent[i])).ent[j] == NULL) {
        (*((pag_t*) (*res).ent[i])).ent[j] = new_pag();
        if((*((pag_t*) (*res).ent[i])).ent[j] == NULL) {
            return 1;
        }
    }
    if( ((*(pag_t*) ((*((pag_t*) (*res).ent[i])).ent[j]))).ent[k] == NULL) {
        ((*((pag_t*) ((*((pag_t*) (*res).ent[i])).ent[j]))).ent[k]) 
                                = (char*) malloc(sizeof(char)*(strlen(linea)+1));
        if((*((pag_t*) ((*((pag_t*) (*res).ent[i])).ent[j]))).ent[k] == NULL) {
            return 1;
        }
        strcpy((char*) ((*((pag_t*) ((*((pag_t*) (*res).ent[i])).ent[j]))).ent[k]),linea);
        return 0;
    }
    fallo(res,i,j,k,linea);
    return 0;
}

void fallo(pag_t* pag,int n1, int n2, int s,char* linea) {
    int i = n1 + n2 + s;
    fprintf(stdout,"%s\n",linea);
    if(pag == NULL || i < 0) {
        return;
    }
    return;
}

int push(pag_t* pag,FILE* fp) {
    int i,j,k;
    for(i = 0; i < 26; i++) {
        if((*pag).ent[i] != NULL) {
            for(j = 0; j < 26; j++) {
                if((*((pag_t*) ((*pag).ent[i]))).ent[j] != NULL) {
                    for(k = 0; k < 26; k++) {
                        if((*((pag_t*) ((*((pag_t*) ((*pag).ent[i]))).ent[j]))).ent[k] != NULL) {
                            fprintf(fp,"%s",(char*) ((*((pag_t*) ((*((pag_t*) ((*pag).ent[i]))).ent[j]))).ent[k]));
                        }
                    }
                }
            }
        }
    }
    return 0;
}

void liberar(pag_t* pag) {
    int i,j,k;
    for(i = 0; i < 26; i++) {
        if((*pag).ent[i] != NULL) {
            for(j = 0; j < 26; j++) {
                if((*((pag_t*) ((*pag).ent[i]))).ent[j] != NULL) {
                    for(k = 0; k < 26; k++) {
                        if((*((pag_t*) ((*((pag_t*) ((*pag).ent[i]))).ent[j]))).ent[k] != NULL) {
                            free((*((pag_t*) ((*((pag_t*) ((*pag).ent[i]))).ent[j]))).ent[k]);
                        }
                    }
                    free((*((pag_t*) ((*pag).ent[i]))).ent[j]);
                }
            }
            free((*pag).ent[i]);
        }
    }
    free(pag);
    return;
}