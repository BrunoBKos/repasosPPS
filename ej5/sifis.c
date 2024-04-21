#include "sifis.h"

int main (int argc, char** argv) {
    FILE* fp;
    FILE* err;
    pag_t* tp;
    int ok;
    if(argc < 4) {
        return 1;
    }
    fp = fopen(argv[2],"r");
    if(fp == NULL) {
        return 1;
    }
    if(strlen(argv[1]) < 2 || argv[1][1] == 'f') {
        if(argc < 5) {
            fclose(fp);
            return 1;
        }
        err = fopen(argv[4],"w");
    }
    else if(argv[1][1] == 's') {
        err = stdout;
    }
    else {
        err = NULL;
    }
    if(strlen(argv[1]) == 3 && argv[1][2] == 's') {
        params[1] = 's';
    }
    else {
        params[1] = 'n';
    }
    params[0] = 'a';
    tp = cargar(fp,err);
    fclose(fp);
    if(tp == NULL){
        if(err != NULL) {
            fclose(err);
        }
        return 1;
    }
    if(argv[1][0] == 'g') {
        fp = fopen(argv[3],"r"); 
        params[0] = 'g';
        modifica(fp,err,tp);
    }
    else {
        if(argv[1][0] == 'm') {
            fp = fopen(argv[3],"w");    
        }
        else {
            fp = fopen(argv[3],"r");
            if (argv[1][0] == 'r') {
                params[0] = 'r';
            }
            modifica(fp,err,tp);
            fclose(fp);
            fp = fopen(argv[2],"w");
        }
        ok = push(tp,fp);
    }
    if(err != NULL) {
        fclose(err);
    }
    liberar(tp);
    fclose(fp);
    return ok;
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

pag_t* cargar(FILE* fp, FILE* out) {
    pag_t* res;
    char linea[2048];
    int ok = 0; 
    res = new_pag();
    if(res == NULL) {
        ok = 1;
    }
    while ((ok == 0) && fgets(linea,2048,fp)) {
        ok = meter(res,linea,out);
    }
    if(ok == 1) {
        return NULL;
    }
    return res;
}

int modifica(FILE* fp,FILE* out, pag_t* tp) {
    char linea[2049];
    while(fgets(linea,2048,fp) != NULL) {
        meter(tp,linea, out);
    }
    return 0;
}

int meter(pag_t* res,char* linea, FILE* out) {
    int i, j, k;
    char* aux; 
    char not[10];
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
    not[0] = linea[0];
    not[1] = linea[1];
    not[2] = linea[2];
    strncpy(&not[3]," null\n",7);
    if((*res).ent[i] == NULL) {
        if(params[0] == 'r' || params[0] == 'g') {
            fallo(out,not);
            return 0;
        }
        (*res).ent[i] = new_pag();
        if((*res).ent[i] == NULL) {
            return 1;
        }
    }
    if((*((pag_t*) (*res).ent[i])).ent[j] == NULL) {
        if(params[0] == 'r' || params[0] == 'g') {
            fallo(out,not);
            return 0;
        }
        (*((pag_t*) (*res).ent[i])).ent[j] = new_pag();
        if((*((pag_t*) (*res).ent[i])).ent[j] == NULL) {
            return 1;
        }
    }
    if( ((*(pag_t*) ((*((pag_t*) (*res).ent[i])).ent[j]))).ent[k] == NULL) {
        if(params[0] == 'r' || params[0] == 'g') {
            fallo(out,not);
            return 0;
        }
        ((*((pag_t*) ((*((pag_t*) (*res).ent[i])).ent[j]))).ent[k]) 
                                = (char*) malloc(sizeof(char)*(strlen(linea)+1));
        if((*((pag_t*) ((*((pag_t*) (*res).ent[i])).ent[j]))).ent[k] == NULL) {
            return 1;
        }
        strcpy((char*) ((*((pag_t*) ((*((pag_t*) (*res).ent[i])).ent[j]))).ent[k]),linea);
        return 0;
    }
    if(params[0] == 'a') {
        if(params[1] == 's') {  
            aux = (char*) ((*(pag_t*) ((*((pag_t*) (*res).ent[i])).ent[j]))).ent[k];
            fallo(out,aux);
            free(aux);
            aux = (char*) malloc(sizeof(char)*(strlen(linea)+1));
            strcpy(aux,linea);
            ((*((pag_t*) ((*((pag_t*) (*res).ent[i])).ent[j]))).ent[k]) = aux;
        }
        else {
            fallo(out,linea);
        }
        return 0;
    }
    if(params[0] == 'r' || params[0] == 'g') {
        aux = (char*) ((*(pag_t*) ((*((pag_t*) (*res).ent[i])).ent[j]))).ent[k];
        fallo(out, aux);
        if(params[0] == 'r') {
            ((*(pag_t*) ((*((pag_t*) (*res).ent[i])).ent[j]))).ent[k] = NULL;
            free(aux);
        }
    }
    return 0;
}

void fallo(FILE* out,char* linea) {
    if(out != NULL) {
        fprintf(out,"%s\n",linea);
    }
    return;
}

void sacar(char* linea, FILE* fp) {
    fprintf(fp,"%s\n",linea);
    return;
}

int push(pag_t* pag,FILE* fp) {
    int i,j,k;
    if(fp == NULL) {
        return 1;
    }
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