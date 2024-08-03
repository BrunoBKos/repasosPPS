#include "compr.h"

int descomprime(FILE* in, FILE* out) {
    cmprsor_t cmpr;
    cola_t cola;
    unsigned int tam = 0;
    int aux;
    int e[2] = {0,0};
    int alert;
    /*inicializacion de las variables*/
    cmpr.pos_cmpr = 0;
    cmpr.pos_lns = 0;
    alert = 0; 
    cola.elems = -4;

    tam += fread(cmpr.text_comp,1,1024,in);
    tam += fread(&((cmpr.text_comp)[tam]),1,1024,in);
    /*for(alert = 0; alert < 20; alert++) {
        printf("%x ",(unsigned char) ((cmpr.text_comp)[alert]));
    }*/
    alert = 0;
    enqueue(&cola,e);
    while((cmpr.pos_cmpr>>3) < tam) {
        if((unsigned int) alert != ((cmpr.pos_lns % 37888) >> 10)) {
            fwrite(&((cmpr.lineas)[(((((cmpr.pos_lns%37888)>>10)+36)<<10)%37888)]),1,1024,out); 
            alert = ((cmpr.pos_lns % 37888) >> 10);
        }
        aux = (cmpr.pos_cmpr >> 3) / 1024;
        sacafueraconlosbits(&cmpr,&cola);
        /*printf("_j:%ld",cmpr.pos_cmpr);*/
        if((unsigned long) aux != ((cmpr.pos_cmpr >> 3) / 1024)) {
            tam += fread(&((cmpr.text_comp)[((((cmpr.pos_cmpr >> 13)+1)%2)<<10)]),1,1024,in);
        }
    }
    /*fprintf(stderr, "bits:%ld",cmpr.pos_cmpr);*/
    aux = (cmpr.pos_lns)%37888; 
    if(!(cmpr.pos_lns % 1024 == 0))
        fwrite(&(cmpr.lineas)[((aux>>10)<<10)],1,(aux%1024)+1,out);
    return 0;
}
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
    enqueue(&cola,e);
    while(cmpr.pos_lns < tam) {
        if((unsigned int) alert != ((cmpr.pos_lns % 37888) >> 10)) {
            tam += fread(&((cmpr.lineas)[(((((cmpr.pos_lns%37888)>>10)+1)<<10)%37888)]),1,1024,in); 
            alert = ((cmpr.pos_lns % 37888) >> 10);
        }
        e[0] = buscaMax(cmpr.lineas,cmpr.pos_lns,tam,&(e[1]));
        aux = compara(&cola,e);
        if(e[0] < 2) {
            if(aux != 3) {
                aux = 1;
            }
            e[0] = 1;
        }
        if(aux == 2) {
            enqueue(&cola,e);
            /*if(4400 > cmpr.pos_lns)  {
            printf("_%d_",e[1]);
            } */  
        }
        metedentroconlosbits(e,out,&cmpr,aux);
        cmpr.pos_lns += e[0];
        e[0] = 0;
        e[1] = 0;
    }
    /*fprintf(stderr, "bits:%ld",cmpr.pos_cmpr);*/
    aux = (cmpr.pos_cmpr >> 3)%2048; 
    if(!(cmpr.pos_cmpr % 1024 == 0))
        fwrite(&(cmpr.text_comp)[(aux>>10)<<10],1,(aux%1024)+1,out);
    return 0;
}
int compara(cola_t* cola, int e[2]) {
    int i, n, j;
    i = ((*cola).elems + 7) % 4;
    n = ((*cola).elems < 0 ? ((*cola).elems + 4) : 4);
    for(j = 0; j < n; j++) {
        if((((*cola).dists)[((i-j)+4)%4]) == e[1]) {
            j += 4;
            n = 5;
            break;
        }
    }
    if((j==4)&&(e[0]==1)) {
        j = 3;
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
    unsigned int mask; 
    unsigned long i, j, aux;
    int cas[2];
    cas[0] = 0; /*lenth*/
    cas[1] = 6; /*distance*/
    j = (*cmpr).pos_cmpr;
    i = j%8;
    j = (j >> 3) % 2048;
    /*if(4234 == (*cmpr).pos_lns) {
        printf("esta es la linea problematica: %ld ", (*cmpr).pos_cmpr);
        for(mask = 8; mask > 0; mask--) {
            printf("%c",((*cmpr).lineas)[(*cmpr).pos_lns + 8 - mask]);
        }
    }*/
    comprbytes[0] = (((*cmpr).text_comp)[j]) & ((char) ((255 >> (8 - i)) << (8 - i)));
    memset(&comprbytes[1],0,5);
    /*codigo comprimido*/
    switch(cs) {
        case 1: /*literal: 0 + byte*/
            baux = ((*cmpr).lineas)[((*cmpr).pos_lns)%37888];
            mask = 128;
            for(i += 1; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & baux) != 0) << (7 - (i%8)));
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
            /*printf("[0] l:%d ", (*cmpr).pos_lns);*/
            comprbytes[i>>3] += (128 >> (i%8));
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            comprbytes[(i+3)>>3] += (128 >> ((i+3)%8));
            i += 4;
            break;
        case 5: /*longrep[1]: 1110 + len*/
            /*printf("[1] l:%d ", (*cmpr).pos_lns);*/
            comprbytes[i>>3] += (128 >> (i%8));
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            comprbytes[(i+2)>>3] += (128 >> ((i+2)%8));
            i += 4;
            break;
        case 6: /*longrep[2]: 11110 + len*/
            /*printf("[2] l:%d ", (*cmpr).pos_lns);*/
            comprbytes[i>>3] += (128 >> (i%8));
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            comprbytes[(i+2)>>3] += (128 >> ((i+2)%8));
            comprbytes[(i+3)>>3] += (128 >> ((i+3)%8));
            i += 5;
            break;
        case 7: /*longrep[3]: 11111 + len*/
            /*printf("[3] l:%d ", (*cmpr).pos_lns);*/
            comprbytes[i>>3] += (128 >> (i%8));
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            comprbytes[(i+2)>>3] += (128 >> ((i+2)%8));
            comprbytes[(i+3)>>3] += (128 >> ((i+3)%8));
            comprbytes[(i+4)>>3] += (128 >> ((i+4)%8));
            i += 5;
            break;
    }
    
    /*leng*/
    cas[0] += (e[0] > 9);
    cas[0] += (e[0] > 17);
    cas[0] += (e[0] > 273);

    switch(cas[0]) {
        case 0: /*[2,9]: 0 + 3bits*/
            mask = 4;
            aux = e[0] - 2;
            for(i += 1; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & aux) != 0) << (7 - (i%8)));
                mask = mask >> 1;
            }
            break;
        case 1: /*[10,17]: 10 + 3 bits*/
            mask = 4;
            aux = e[0] - 10;
            comprbytes[i>>3] += (128 >> (i%8));
            for(i += 2; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & aux) != 0) << (7 - (i%8)));
                mask = mask >> 1;
            }
            break;
        case 2: /*[18,273]: 11 + 8 bits*/
            mask = 128;
            aux = e[0] - 18;
            comprbytes[i>>3] += (128 >> (i%8));
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            for(i += 2; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & aux) != 0) << (7 - (i%8)));
                mask = mask >> 1;
            }
            break;
        default: /*no hay length*/
            break;
    }
    /*dist*/
    cas[1] += (e[1] > 7);
    cas[1] += (e[1] > 39);
    cas[1] += (e[1] > 295);
    cas[1] += (e[1] > 4392);

    switch(cas[1]) {
        case 0: /*[0,7]*/ /*00 3bits*/
            mask = 4;
            aux = e[1];
            for(i += 2; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & aux) != 0) << (7 - (i%8)));
                mask = mask >> 1;
            }
            break;
        case 1: /*[8,39]*/ /*01 5 bits*/
            mask = 16;
            aux = e[1] - 8;
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            for(i += 2; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & aux) != 0) << (7 - (i%8)));
                mask = mask >> 1;
            }
            break;
        case 2: /*[40,295]*/ /*10 8 bits*/
            mask = 128;
            aux = e[1] - 40;
            comprbytes[i>>3] += (128 >> (i%8));
            for(i += 2; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & aux) != 0) << (7 - (i%8)));
                mask = mask >> 1;
            }
            break;
        case 3: /*296,4392*/ /*110 12 bits*/
            mask = 2048;
            aux = e[1] - 296;
            comprbytes[i>>3] += (128 >> (i%8));
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            for(i += 3; mask > 0 ;i++) {
                baux = (unsigned char) (((mask & aux) != 0)*(128>>(i%8)));
                comprbytes[i>>3] += (unsigned char) baux;
                mask = mask >> 1;
            }
            break;
        case 4: /*[4393,35840]*/ /*111 15 bits*/
            mask = 16384;
            aux = e[1] - 4393;
            comprbytes[i>>3] += (128 >> (i%8));
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            comprbytes[(i+2)>>3] += (128 >> ((i+2)%8));
            for(i += 3; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & aux) != 0) << (7 - (i%8)));
                mask = mask >> 1;
            }
            break;
        default: /*no hay length*/
            break;
    }
    if (37379 == (*cmpr).pos_lns ) {
        printf("cs:%d;bit:%ld;length:%d;dist:%d;car:%d;",cs,(*cmpr).pos_cmpr,e[0],e[1],(*cmpr).pos_lns);
    }
    for(mask = 0; mask < ((i>>3)+1); mask++) {
        ((*cmpr).text_comp)[(j+mask)%2048] = comprbytes[mask];
    }
    /*memcpy(&(((*cmpr).text_comp)[j]),comprbytes,(i>>3)+1); */
    if((j < 1024) != (((j+(i>>3))%2048) < 1024)){
        fwrite(&(((*cmpr).text_comp)[((j>>10)<<10)]),1,1024,out);
    }
    /*if(i > 200) {
        fprintf(stderr,"aqui estan pasando cosas; i:%ld,aux:%d,ln:%d,dist:%d",i,cs,e[0],e[1]);
    }*/
    if (37379 == (*cmpr).pos_lns ) {
        printf("i:%ld;bit:%ld;",i,(*cmpr).pos_cmpr);
    }
    (*cmpr).pos_cmpr = (((((*cmpr).pos_cmpr)/8)*8) + i);
    if (37379 == (*cmpr).pos_lns ) {
        printf("bit de fallo:%ld;",(*cmpr).pos_cmpr);
        printf("-%c-%c-%c-%c",((*cmpr).lineas)[577],((*cmpr).lineas)[578],((*cmpr).lineas)[579],((*cmpr).lineas)[580]);
    }
    /*if(e[1] == 341) {
        printf("los bits que hay que mirar son: %ld ",(*cmpr).pos_cmpr);
    }*/
}

int sacafueraconlosbits(cmprsor_t* cmpr, cola_t* cola) {
    unsigned int i;
    int mask,aux;
    unsigned long j;
    unsigned int aux2,aux3;
    int cas[2] = {0,0}; /*{len, dist}*/
    unsigned char baux, baux2;
    j = (*cmpr).pos_cmpr;
    aux = ((j >> 3) % 2048);
    j = j % 8;
    i = j;
    if((*cmpr).pos_lns == 9955) {
        printf("estado del bit err:%ld;",(*cmpr).pos_cmpr);
        for(mask = 0; mask < 8; mask++) {
            printf("_%X",((*cmpr).text_comp)[(aux + mask)%2048]);
        }
        printf(";");
    }
    /*if(4234 == (*cmpr).pos_lns) {
        printf("esta es la linea problematica: %ld ", (*cmpr).pos_cmpr);
    }*/
    while((((*cmpr).text_comp)[(aux + (j>>3))%2048] & (128 >> (j%8)))) {
        j++;
        if((j-i) == 5) {
            break;
        } 
    }
    j++;
    switch(j - i - 1) {
        case 0: /*literal*/
            mask = 128;
            baux = 0;
            while(mask > 0) {
                aux2 = (128 >> (j%8));
                baux2 = ((*cmpr).text_comp)[(aux + (j >> 3))%2048];
                aux3 = ((baux2 & aux2) != 0);
                baux |= (mask*aux3);
                j++;
                mask >>= 1;
            }
            (*cmpr).pos_cmpr = ((((*cmpr).pos_cmpr >> 3) << 3) + j);
            ((*cmpr).lineas)[((*cmpr).pos_lns)%37888] = baux;
            (*cmpr).pos_lns += 1;
            return 0;
        case 1: /*match*/
            break;
        case 2: /*shortrep + longrep[0]*/
            cas[1] = 1;
            if(!((((*cmpr).text_comp)[aux + (j>>3)] & (128 >> (j%8))))) {
                cas[0] = 1;
            }
            else {
                /*printf("[0] l:%d ", (*cmpr).pos_lns);*/
            }
            j++;
            break;
        case 3: /*longrep[1] */
            /*printf("[1] l:%d ", (*cmpr).pos_lns);*/
            cas[1] = 2;
            break;
        case 4: /*longrep[2]*/
            /*printf("[2] l:%d ", (*cmpr).pos_lns);*/
            cas[1] = 3;
            break;
        case 5: /*longrep[3]*/
            /*printf("[3] l:%d ", (*cmpr).pos_lns);*/
            j--;
            cas[1] = 4;
            break;
    }
    if((*cmpr).pos_lns == 9955) {
        printf("estado 2 del bit:%ld;",(*cmpr).pos_cmpr);
    }
    /*length*/
    if(cas[0] == 0) {
        i = j;
        while((((*cmpr).text_comp)[(aux + (j>>3))%2048] & (128 >> (j%8)))) {
            j++;
            if((j-i) == 2) {
                break;
            } 
        }
        j++;
        switch(j - i -1) {
            case 0:
                mask = 4;
                cas[0] = 2;
                break;
            case 1:
                mask = 4;
                cas[0] = 10;
                break;
            case 2:
                mask = 128;
                cas[0] = 18;
                j--;
                break;
        }
        while(mask > 0) {
            cas[0] += (mask * (0 != ((((*cmpr).text_comp)[(aux + (j >> 3))%2048]) & (128 >> (j%8)))));
            mask >>= 1;
            j++;
        }
    }
    /*if((*cmpr).pos_lns == 4234) {
        printf("caracteres:%ld",(*cmpr).pos_cmpr);
    }*/
    /*dist*/

    if(cas[1] == 0) {
        i = j;
        while((((*cmpr).text_comp)[(aux + (j>>3))%2048] & (128 >> (j%8)))) {
            j++;
            if((j-i) == 3) {
                break;
            } 
        }
        j++;
        switch(j - i -1) {
            case 0:
                if((((*cmpr).text_comp)[(aux + (j>>3))%2048] & (128 >> (j%8)))) {
                    mask = 16;
                    cas[1] = 8;
                }
                else {
                    mask = 4;
                }
                j++;
                break;
            case 1:
                mask = 128;
                cas[1] = 40;
                break;
            case 2:
                mask = 2048;
                cas[1] = 296;
                break;
            case 3:
                cas[1] = 4393;
                mask = 16384;
                j--;
                break;
        }
        while(mask > 0) {
            cas[1] += (mask * (0 != ((((*cmpr).text_comp)[(aux + (j >> 3))%2048]) & (128 >> (j%8)))));
            mask >>= 1;
            j++;
        }
        enqueue(cola,cas);
        /*if(4400 > (*cmpr).pos_lns)  {
            printf("_%d_",cas[1]);
        }*/
    }
    else {
        cas[1] = sacacola(cola,cas[1]);
    }
    if((*cmpr).pos_lns == 36858/* || (*cmpr).pos_lns == 4243 || (*cmpr).pos_lns == 4241 */) {
        printf("length:%d;dist:%d;",cas[0],cas[1]);
    } 
    (*cmpr).pos_cmpr = ((((*cmpr).pos_cmpr >> 3) << 3) + j);
    if((*cmpr).pos_lns == 9955) {
        printf("estado 3 del bit:%ld;",(*cmpr).pos_cmpr);
    }
    for(i = 0; ((int) i) < cas[0]; i++) {
        ((*cmpr).lineas)[((*cmpr).pos_lns + i)%37888] = ((*cmpr).lineas)[((*cmpr).pos_lns + i - cas[1])%37888];
        if((*cmpr).pos_lns == 36858) {
            printf("-%c-",((*cmpr).lineas)[((*cmpr).pos_lns + i - cas[1])%37888]);
        }
    }
    (*cmpr).pos_lns += i;
    return 0;
}

int coinCar(char* str,int st1, int st2, int to) {
    int res;
    res = 0;
    while(res < to && res < 273) {
        if(str[(res+st1)%37888] != str[(res+st2)%37888]) {
            break;
        }
        res++;
    }
    return res;
}

int buscaMax(char* str, int from, int to, int* pos) {
    int i,aux,aux2;
    int res[2];
    res[0] = 0;
    i = 1;
    aux2 = (from < 35840) ? from : 35840;
    while(i < aux2) {
        if(str[(from % 37888)] == str[((from - i) % 37888)]) {
            aux = coinCar(str,from,(from - i + 37888),to - from);
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

int sacacola(cola_t* cola,int e) {  
    return ((*cola).dists)[(((*cola).elems - e)+8)%4];
}