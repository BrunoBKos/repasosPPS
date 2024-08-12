#include "compr.h"
/*funciones principales*/

/*funcion para iniciar la compresion*/
void comprime(FILE* in, FILE* out) {
    cmprsor_t cmpr; /*struct compresor*/
    cola_t cola; /*cola para los longrep*/
    int e[2] = {0,0}; /*len, dist*/
    unsigned int tam = 0; /*contador de los caracteres leidos del fichero*/
    int hs[257];
    int map[37888];
    /*variables adicionales*/
    int aux;
    unsigned int alert;
    /*inicializacion de las variables*/
    cmpr.pos_cmpr = 0;
    cmpr.pos_lns = 0;
    alert = 0; 
    cola.tam = -4;
    tam += fread(cmpr.lineas,1,1024,in);
    tam += fread(&((cmpr.lineas)[tam]),1,1024,in);
    memset(hs,-1,1024);
    /*bucle principal*/
    while(cmpr.pos_lns < tam) {
        /*cuando se pasa un bloque de caracteres*/
        if(alert != ((cmpr.pos_lns % 37888) >> 10)) {
            /*cargado del siguiente bloque de caracteres*/
            tam += fread(&((cmpr.lineas)[((((cmpr.pos_lns>>10)+1)<<10)%37888)]),1,1024,in); 
            alert = ((cmpr.pos_lns % 37888) >> 10);
        }
        /*se busca una coincidencia en las lineas anteriores*/
        e[0] = buscaMax(cmpr.lineas,(cmpr.pos_lns%37888),(tam-cmpr.pos_lns),&(e[1]),hs,map);
        /*se comprueba si es una posicion repetida*/
        aux = compara(&cola,e[1]);
        if(e[0] < 2) {
            if(aux != 4 || e[0] != 1) {
                aux = 2;
            }
            aux--;
            e[0] = 1;
        }
        if(aux == 2) {
            enqueue(&cola,e[1]);
        }
        /*funcion de compresion*/
        metedentroconlosbits(e,out,&cmpr,aux,hs,map);
        cmpr.pos_lns += e[0];
        e[0] = 0;
    }
    /*cargado de los ultimos caracteres comprimidos*/
    aux = (cmpr.pos_cmpr >> 3)%2048; 
    if(cmpr.pos_cmpr % 1024) 
        fwrite(&(cmpr.text_comp)[(aux>>10)<<10],1,(aux%1024)+1,out);
}
/*funcion para iniciar la descompresion*/
void descomprime(FILE* in, FILE* out) {
    cmprsor_t cmpr; /*compresor*/
    cola_t cola; /*cola para los longrep*/
    /*variables*/
    unsigned int tam = 0;
    int e[2] = {0,0};
    int aux;
    int alert;
    /*inicializacion de las variables*/
    cmpr.pos_cmpr = 0;
    cmpr.pos_lns = 0;
    alert = 0; 
    cola.tam = -4;
    /*primera lectura del fichero*/
    tam += fread(cmpr.text_comp,1,1024,in);
    tam += fread(&((cmpr.text_comp)[tam]),1,1024,in);
    alert = 0;
    while((cmpr.pos_cmpr>>3) < tam) {
        /*cargado de caracteres en el fichero en caso de haberse llenado un segmento*/
        if((unsigned int) alert != ((cmpr.pos_lns % 37888) >> 10)) {
            fwrite(&((cmpr.lineas)[(((((cmpr.pos_lns%37888)>>10)+36)<<10)%37888)]),1,1024,out); 
            alert = ((cmpr.pos_lns % 37888) >> 10);
        }
        aux = (cmpr.pos_cmpr >> 13); /*divido entre 8*1024*/
        sacafueraconlosbits(&cmpr,&cola);/*inicio la descompresion*/
        /*cargado de caracteres en el buffer en caso de haber pasado de segmento*/
        if((unsigned long) aux != (cmpr.pos_cmpr >> 13)) { 
            tam += fread(&((cmpr.text_comp)[((((cmpr.pos_cmpr >> 13)+1)%2)<<10)]),1,1024,in);
        }
    }
    aux = (cmpr.pos_lns)%37888; 
    /*ultimo cargado de caracteres en el fichero en caso de no 
    haberse podido realizar con anterioridad*/
    if(!(cmpr.pos_lns % 1024 == 0))
        fwrite(&(cmpr.lineas)[((aux>>10)<<10)],1,(aux%1024)+1,out);
}

/*funciones auxiliares*/

void metedentroconlosbits(int e[2], FILE* out, cmprsor_t* cmpr, int cs, int hs[256], int map[37888]) {
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
    comprbytes[0] = (((*cmpr).text_comp)[j]) & ((char) ((255 >> (8 - i)) << (8 - i)));
    memset(&comprbytes[1],0,5);
    /*codigo comprimido*/
    switch(cs) {
        case 1: /*literal: 0 + byte*/
            aux = ((*cmpr).pos_lns)%37888;
            baux = ((*cmpr).lineas)[aux];
            mask = 128;
            for(i += 1; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & baux) != 0) << (7 - (i%8)));
                mask = mask >> 1;
            }
            cas[0] = 5;
            map[aux] = hs[(unsigned int) baux];
            hs[(unsigned int) baux] = aux;
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
            aux = ((*cmpr).pos_lns)%37888;
            baux = ((*cmpr).lineas)[aux];
            map[aux] = hs[(unsigned int) baux];
            hs[(unsigned int) baux] = aux;
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
    
    /*length*/
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
    if (cas[0] < 3) {
        aux = ((*cmpr).pos_lns)%37888;
        mask = e[0];
        while(mask > 0) {
            baux = ((*cmpr).lineas)[aux];
            map[aux] = hs[(unsigned int) baux];
            hs[(unsigned int) baux] = aux;
            mask--;
            aux = (aux + 1)%37888;
        }
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
    /*cargado de los bits comprimidos en el buffer*/
    for(mask = 0; mask < ((i>>3)+1); mask++) {
        ((*cmpr).text_comp)[(j+mask)%2048] = comprbytes[mask];
    }
    /*cargardo de los bits comprimidos en el 
    fichero cuando se ha llenado el buffer*/
    if((j < 1024) != (((j+(i>>3))%2048) < 1024)){
        fwrite(&(((*cmpr).text_comp)[((j>>10)<<10)]),1,1024,out);
    }
    /*actualizacion del contador de bits comprimidos*/
    (*cmpr).pos_cmpr = (((((*cmpr).pos_cmpr)/8)*8) + i);
}

void sacafueraconlosbits(cmprsor_t* cmpr, cola_t* cola) {
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
    /*calculo del caso*/
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
            return;
        case 1: /*match*/
            break;
        case 2: /*shortrep + longrep[0]*/
            cas[1] = 1;
            if(!((((*cmpr).text_comp)[aux + (j>>3)] & (128 >> (j%8))))) {
                cas[0] = 1;
            }
            j++;
            break;
        case 3: /*longrep[1] */
            cas[1] = 2;
            break;
        case 4: /*longrep[2]*/
            cas[1] = 3;
            break;
        case 5: /*longrep[3]*/
            j--;
            cas[1] = 4;
            break;
    }
    /*length*/
    if(cas[0] == 0) {
        i = j;
        /*calculo del caso*/
        while((((*cmpr).text_comp)[(aux + (j>>3))%2048] & (128 >> (j%8)))) {
            j++;
            if((j-i) == 2) {
                break;
            } 
        }
        j++;
        switch(j - i -1) {
            case 0: /*3 bits*/
                mask = 4;
                cas[0] = 2;
                break;
            case 1: /*3 bits*/
                mask = 4;
                cas[0] = 10;
                break;
            case 2: /*8 bits*/
                mask = 128;
                cas[0] = 18;
                j--;
                break;
        }
        /*calculo de la length*/
        while(mask > 0) {
            cas[0] += (mask * (0 != ((((*cmpr).text_comp)[(aux + (j >> 3))%2048]) & (128 >> (j%8)))));
            mask >>= 1;
            j++;
        }
    }
    /*dist*/
    if(cas[1] == 0) {
        i = j;
        /*calculo del caso*/
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
                    /*5 bits*/
                    mask = 16;
                    cas[1] = 8;
                }
                else { /*3 bits*/
                    mask = 4;
                }
                j++;
                break;
            case 1: /*8 bits*/
                mask = 128;
                cas[1] = 40;
                break;
            case 2:/*12 bits*/
                mask = 2048;
                cas[1] = 296;
                break;
            case 3:/*15 bits*/
                cas[1] = 4393;
                mask = 16384;
                j--;
                break;
        }
        /*calculo de la distancia*/
        while(mask > 0) {
            cas[1] += (mask * (0 != ((((*cmpr).text_comp)[(aux + (j >> 3))%2048]) & (128 >> (j%8)))));
            mask >>= 1;
            j++;
        }
        /*la nueva distancia se encola*/
        enqueue(cola,cas[1]);
    }
    else {
        /*la distancia no se tiene que calcular sino que se saca de cola*/
        cas[1] = sacacola(cola,cas[1]);
    }
    /*actualizacion del contador de bits comprimidos*/
    (*cmpr).pos_cmpr = ((((*cmpr).pos_cmpr >> 3) << 3) + j);
    /*cargado de los caracteres descomprimidos en el buffer*/
    for(i = 0; ((int) i) < cas[0]; i++) {
        ((*cmpr).lineas)[((*cmpr).pos_lns + i)%37888] = ((*cmpr).lineas)[((*cmpr).pos_lns + i - cas[1])%37888];
    }
    /*actualizacion del contador de caracteres descomprimidos*/
    (*cmpr).pos_lns += i;
}

int buscaMax(char* str, int from, int to,int* pos, int hs[256], int map[37888]) {
    int i, aux, aux2, aux3;
    int res[2];
    res[0] = 0;
    aux = ((to < 273) ? to : 273);
    aux3 = 0;
    i = hs[(unsigned int) (str[from])];
    aux2 = (((from - i) + 39936)%37888);
    if(i > 0 && aux2 <= 2048) {
        hs[(unsigned int) (str[from])] = -1;
        return 0;
    }
    while(i > 0 && aux2 > 2048) {
        aux2 = coinCar(str,from, i, aux);
        if(res[0] < aux2) {
            res[0] = aux2;
            res[1] = i;
            if(res[0] == 273) {
                break;
            }
        }
        else if(!aux2) {
            break;
        }
        i = map[i];
        aux3 = aux2;
        aux2 = (((from - i) + 39936)%37888);
        if(aux3 >= aux2) {
            break;
        }
    }
    *pos = (((from - res[1]) + 37888)%37888);
    return res[0];
}

int coinCar(char* str,int st1, int st2, int to) {
    int res;
    for(res = 0;res < to;res++) {
        if(str[(res+st1)%37888] != str[(res+st2)%37888]) {
            break;
        }
    }
    return res;
}

void enqueue(cola_t* cola, int e) {
    int i;
    i = ((*cola).tam + 4) % 4;
    ((*cola).elems)[i] = e;
    (*cola).tam += 1;
    (*cola).tam = (*cola).tam == 4 ? 0 : (*cola).tam;
}

int sacacola(cola_t* cola,int e) {  
    return ((*cola).elems)[(((*cola).tam - e)+8)%4];
}

int compara(cola_t* cola, int e) {
    int i, n, j;
    i = ((*cola).tam + 7) % 4;
    n = ((*cola).tam < 0 ? ((*cola).tam + 4) : 4);
    for(j = 0; j < n; j++) {
        if((((*cola).elems)[((i-j)+4)%4]) == e) {
            j += 4;
            n = 5;
            break;
        }
    }
    return n != 5 ? 2 : j;
}
