#include "compr.h"

int descomprime();
/*struct cmprsor_s {
    unsigned int pos_lns;
    char lineas[4096];
    unsigned long pos_cmpr;
    char text_comp[2048]; 
}; typedef struct cmprsor_s cmprsor_t;*/
int comprime(FILE* in, FILE* out) {
    cmprsor_t cmpr;
    cola_t cola;
    int e[2] = {0,0}; /*len, dist*/
    unsigned int tam = 0;
    int aux;
    int alert;
    /*inicializacion de las variables*/
    cmpr.pos_cmpr = 0;
    cmpr.pos_lns = 0;
    alert = 0; 
    cola.elems = -4;
    tam += fread(cmpr.lineas,1,1024,in);
    tam += fread(&((cmpr.lineas)[tam]),1,1024,in);
    while(cmpr.pos_lns < tam) {
        if((unsigned int) alert != ((cmpr.pos_lns % 4096) >> 10)) {
            tam += fread(&((cmpr.lineas)[(((((cmpr.pos_lns%4096)>>10)+1)<<10)%4096)]),1,1024,in); 
            alert = ((cmpr.pos_lns % 4096) >> 10);
        }
        e[0] = buscaMax(cmpr.lineas,cmpr.pos_lns,tam,&(e[1]));
        aux = compara(&cola,e);
        if(e[0] < 1) {
            aux = 0;
        }
        metedentroconlosbits(e,out,&cmpr,aux);
        cmpr.pos_lns += e[0];
        enqueue(&cola,e);
    }
    return 0;
}
int compara(cola_t* cola, int e[2]) {
    int i, n, j;
    i = ((*cola).elems + 4) % 4;
    n = ((*cola).elems < 0 ? ((*cola).elems + 4) : 4);
    for(j = 0; j < n; j++) {
        if((((*cola).dists)[(i+j)%4]) == e[1]) {
            j += 4;
            n = 5;
            break;
        }
    }
    if(!j && (e[0] == 1)) {
        j -= 1;
    }
    return n != 5 ? 2 : j;
}

void enqueue(cola_t* cola, int e[2]) {
    int i;
    i = ((*cola).elems + 4) % 4;
    ((*cola).lnths)[i] = e[0];
    ((*cola).dists)[i] = e[1];
    (*cola).elems += 1;
    (*cola).elems = (*cola).elems == 4 ? 0 : (*cola).elems;
}

void metedentroconlosbits(int e[2], FILE* out, cmprsor_t* cmpr, int cs) {
    char comprbytes[6];
    char baux;
    int mask; 
    unsigned long i, j, aux;
    int cas[2];
    cas[0] = 0; /*lenth*/
    cas[1] = 5; /*distance*/
    j = (*cmpr).pos_cmpr;
    i = j%8;
    j = (j >> 3) % 2048;
    comprbytes[0] = (((*cmpr).text_comp)[j]) & ((char) ((255 >> (8 - i)) << (8 - i)));
    memset(&comprbytes[1],0,5);
    /*codigo comprimido*/
    switch(cs) {
        case 1: /*literal: 0 + byte*/
            baux = ((*cmpr).lineas)[((*cmpr).pos_lns)%4096];
            mask = 128;
            for(i += 1; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & baux) != 0) << (8 - (i%8)));
                mask = mask >> 1;
            }
            cas[0] = 5;
            break;
        case 2: /*match: 10 + len + dist*/
            comprbytes[i>>3] += (128 >> (i%8));
            cas[1] = 0;
            i += 2;
            break;
        case 3: /*shortrep: 1100 */
            comprbytes[i>>3] += (128 >> (i%8));
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            cas[0] = 5;
            i += 4;
            break;
        case 4: /*longrep[0]: 1101 + len*/
            comprbytes[i>>3] += (128 >> (i%8));
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            comprbytes[(i+3)>>3] += (128 >> ((i+3)%8));
            i += 4;
            break;
        case 5: /*longrep[1]: 1110 + len*/
            comprbytes[i>>3] += (128 >> (i%8));
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            comprbytes[(i+2)>>3] += (128 >> ((i+2)%8));
            i += 4;
            break;
        case 6: /*longrep[2]: 11110 + len*/
            comprbytes[i>>3] += (128 >> (i%8));
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            comprbytes[(i+2)>>3] += (128 >> ((i+2)%8));
            comprbytes[(i+3)>>3] += (128 >> ((i+3)%8));
            i += 5;
            break;
        case 7: /*longrep[3]: 11111 + len*/
            comprbytes[i>>3] += (128 >> (i%8));
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            comprbytes[(i+2)>>3] += (128 >> ((i+2)%8));
            comprbytes[(i+3)>>3] += (128 >> ((i+3)%8));
            comprbytes[(i+4)>>3] += (128 >> ((i+4)%8));
            i += 5;
            break;
    }
    
    /*leng*/
    cas[0] += (e[0] < 10);
    cas[0] += (e[0] < 18);
    cas[0] += (e[0] < 274);

    switch(cas[0]) {
        case 1: /*[2,9]: 0 + 3bits*/
            mask = 4;
            aux = e[0] - 2;
            for(i += 1; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & aux) != 0) << (8 - (i%8)));
                mask = mask >> 1;
            }
            break;
        case 2: /*[10,17]: 10 + 3 bits*/
            mask = 4;
            aux = e[0] - 10;
            comprbytes[i>>3] += (128 >> (i%8));
            for(i += 2; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & aux) != 0) << (8 - (i%8)));
                mask = mask >> 1;
            }
            break;
        case 3: /*[18,273]: 11 + 8 bits*/
            mask = 128;
            aux = e[0] - 18;
            comprbytes[i>>3] += (128 >> (i%8));
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            for(i += 2; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & aux) != 0) << (8 - (i%8)));
                mask = mask >> 1;
            }
            break;
        default: /*no hay length*/
            break;
    }
    /*dist*/
    cas[1] += (e[1] < 10);
    cas[1] += (e[1] < 42);
    cas[1] += (e[1] < 297);
    cas[1] += (e[1] < 2049);

    switch(cas[0]) {
        case 1: /*[2,9]*/ /*00 3bits*/
            mask = 4;
            aux = e[0] - 2;
            for(i += 2; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & aux) != 0) << (8 - (i%8)));
                mask = mask >> 1;
            }
            break;
        case 2: /*[10,41]*/ /*01 5 bits*/
            mask = 16;
            aux = e[0] - 10;
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            for(i += 2; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & aux) != 0) << (8 - (i%8)));
                mask = mask >> 1;
            }
            break;
        case 3: /*[42,296]*/ /*10 8 bits*/
            mask = 128;
            aux = e[0] - 42;
            comprbytes[i>>3] += (128 >> (i%8));
            for(i += 2; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & aux) != 0) << (8 - (i%8)));
                mask = mask >> 1;
            }
            break;
        case 4: /*297,2048*/ /*11 12 bits*/
            mask = 2048;
            aux = e[0] - 297;
            comprbytes[i>>3] += (128 >> (i%8));
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            for(i += 2; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & aux) != 0) << (8 - (i%8)));
                mask = mask >> 1;
            }
            break;
        default: /*no hay length*/
            break;
    }
    memcpy(&(((*cmpr).text_comp)[j]),comprbytes,(i>>3)+1);
    if((j < 1024) != (((j+(i>>3))%2048) < 1024)){
        fwrite(&(((*cmpr).text_comp)[((j>>10)<<10)]),1,1024,out);
    }
    (*cmpr).pos_cmpr += ((((*cmpr).pos_cmpr)>>3)<<3) + i;
}

int coinCar(char* str,char* str2, int to) {
    int res;
    res = 0;
    while(res < to && res < 273) {
        if(str[(res%4096)] != str2[(res%4096)]) {
            break;
        }
        res++;
    }
    return res;
}

int buscaMax(char* str, int from, int to, int* pos) {
    int i,aux;
    int res[2];
    res[0] = 0;
    i = 0;
    aux = ((from - 2048) < 0) ? from : 2048;
    while(i < aux) {
        if(str[(from % 4096)] == str[((from - i) % 4096)]) {
            aux = coinCar(&str[(from % 4096)], &str[(from - i) % 4096],to - from);
            if(res[0] < aux) {
                res[0] = aux;
                res[1] = i;
            }
            if(aux == 273) {
                break;
            }
        }
        i++;
    }
    *pos = res[1];
    return res[0];
}