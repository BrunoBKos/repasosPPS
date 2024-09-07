#include "compr.h"
/*funciones principales*/

/*funcion para iniciar la compresion*/
void comprime(FILE* in, FILE* out) {
    char cmpr[2048];
    char txt[3072];
    unsigned int pos_txt;
    unsigned long pos_cmpr;
    unsigned int tam = 0; /*contador de los caracteres leidos del fichero*/
    int lsfr[4368]; /*lista para las frecuencias de caracteres*/
    int lsdr[4367]; /*lista para las direciones asociadas a las frecuencias*/ 
    int map[65536]; /*mapa de posiciones de los caracteres*/
    /*variables adicionales*/
    int aux;
    unsigned int alert;
    char e[3];
    /*inicializacion de las variables*/
    pos_cmpr = 0;
    pos_txt = 0;
    alert = 0;
    tam += fread(txt,1,1024,in);
    tam += fread(&(txt[tam]),1,1024,in);
    /*de no haber caracteres salimos*/
    if(!tam) {
        return;
    }
    /*inicializacion de las lista a -1*/
    memset(map,-1,65536*sizeof(int));
    lsfr[4367] = 0; /*la ultima posicion de la lista es el tamaño de la misma*/ 
    /*metemos el primer caracter como literal*/
    pos_cmpr += metedentroconlosbits(out,cmpr,txt[0],pos_cmpr,-1);
    pos_txt++;
    /*bucle principal*/
    tam--; /*para que no entre cuando solo queda un caracter*/
    while(pos_txt < tam) {
        /*analisis de la compresion */
        e[0] = txt[(pos_txt-1)%3072];
        e[1] = txt[pos_txt%3072]; 
        e[2] = txt[(pos_txt+1)%3072];
        aux = analisis(map,lsfr,lsdr,(unsigned char*) e);
        /*funcion de compresion*/
        pos_cmpr += metedentroconlosbits(out,cmpr,txt[(pos_txt%3072)],pos_cmpr,aux);
        /*incremento el contador*/
        pos_txt++;
        pos_txt += (aux >= 0);
        /*cuando se pasa un bloque de caracteres*/
        if(alert != (pos_txt >> 10)) {
            /*cargado del siguiente bloque de caracteres*/
            /*lo que puedo hacer es restarle uno por si falla en los casos en que vale 0 o modulo de 1024*/
            tam += fread(&(txt[((((pos_txt>>10)+1)<<10)%3072)]),1,1024,in); 
            alert = (pos_txt >> 10);
        }
    }
    /*añado el ultimo caracter en caso de ser necesario*/
    if(!(pos_txt - tam)){ /*cuando la diferencia es de cero es necesario añadir el ultimo caracter*/
        pos_cmpr += metedentroconlosbits(out,cmpr,txt[(pos_txt%3072)],pos_cmpr,-1);    
    }
    /*mirar el tema del finalizador*/
    pos_cmpr += metedentroconlosbits(out,cmpr,'0',pos_cmpr,4367);
    /*cargado de los ultimos caracteres comprimidos en caso de ser necesario*/
    aux = (pos_cmpr >> 3)%2048;
    if(pos_cmpr % 1024) 
        fwrite(&(cmpr)[(aux>>10)<<10],1,(aux%1024)+1,out);
}
/*ya la retocare; me falta un poco de tiempo todavia jeje*/
/*funcion para iniciar la descompresion*/
void descomprime(FILE* in, FILE* out) {
    /*variables estructura*/
    char cmpr[3072];
    char txt[2048];
    unsigned int pos_txt;
    unsigned long pos_cmpr;
    unsigned int tam = 0; /*contador de los caracteres leidos del fichero*/
    int lsfr[4368]; /*lista para las frecuencias de caracteres*/
    int lsdr[4367]; /*lista para las direciones asociadas a las frecuencias*/ 
    int map[65536]; /*mapa de posiciones de los caracteres*/
    /*variables auxiliares*/
    int aux;
    int aux2;
    unsigned int alert;
    /*inicializacion de las variables*/
    pos_cmpr = 0;
    pos_txt = 0;
    alert = 0; 
    /*primera lectura del fichero*/
    tam += fread(cmpr,1,1024,in);
    tam += fread(&(cmpr[tam]),1,1024,in);
    /*primer caracter tiene por necesidad que ser un literal*/
    if(tam < 2) { /*no hay al menos un caracter, atributo incorrecto*/
        return;
    }
    /*cargado del primer caracter comprimido*/
    txt[0] = 0;
    txt[0] += ((cmpr[1] & 128) != 0);
    txt[0] |= ((cmpr[0] & 127) << 1);
    pos_txt++;
    pos_cmpr += 9;
    while((pos_cmpr>>3) < tam) {
        /*cargado de caracteres en el fichero en caso de haberse llenado un segmento*/
        if(alert != (pos_txt >> 10)) {
            fwrite(&(txt[((((pos_txt >> 10) + 1) % 2) << 10)]),1,1024,out); 
            alert = (pos_txt >> 10);
        }
        aux = (pos_cmpr >> 13); /*divido entre 8*1024*/
        aux2 = sacafueraconlosbits(cmpr,txt,pos_cmpr,pos_txt,map,lsfr,lsdr);
        /*aumento de contadores*/
        pos_cmpr += aux2;
        pos_txt++;
        pos_txt += (aux2 != 9);
        /*cargado de caracteres en el buffer en caso de haber pasado de segmento*/
        if((unsigned int) aux != (pos_cmpr >> 13)) { 
            tam += fread(&(cmpr[((((pos_cmpr >> 13)+1)%3)<<10)]),1,1024,in);
        }
    }
    aux = (pos_txt)%2048; 
    /*ultimo cargado de caracteres en el fichero en caso de no 
    haberse podido realizar con anterioridad*/
    if(pos_txt & 1023)
        fwrite(&(txt[((aux>>10)<<10)]),1,(aux%1024),out);
}

/*funciones auxiliares*/


/*descripcion de los atributos :
    out -> fichero de salida  "sin cambios"
    cmpr -> buffer de caracteres comprimidos
    e -> caracter, necesario en caso de tener que añadirlo como literal 
    pos_cmpr -> posicion con respecto al texto comprimido; nivel de bit
  posibles casos: 
    cs < 0 -> titeral
    0 <= cs <= 15 -> T1 "4 bits"
    16 <= cs <= 271 -> T2 "8 bits"
    272 <= cs <= 4366 -> T3 "12 bits"
    cs = 4367 -> terinador "12 bits"
    */
int metedentroconlosbits(FILE* out,char cmpr[2048],unsigned char e,unsigned long pos_cmpr, int cs) {
    char comprbytes[6];
    unsigned int mask; 
    unsigned long i, j, aux;
    j = pos_cmpr;
    i = j%8;
    j = (j >> 3) % 2048;
    comprbytes[0] = (cmpr[j]) & ((char) ((255 >> (8 - i)) << (8 - i)));
    memset(&comprbytes[1],0,5);
    /*codigo comprimido*/
    aux = 0; /*caso literal*/
    aux += (cs >= 0); /*caso T1*/
    aux += (cs > 15); /*caso T2*/
    aux += (cs > 271); /*caso T3 incluyendo el terminador*/
    switch(cs) {
        case 0: /*literal: 0 + byte*/
            mask = 128;
            for(i += 1; mask > 0 ;i++) {
                comprbytes[i>>3] += (((mask & e) != 0) << (7 - (i%8)));
                mask = mask >> 1;
            }
            break;
        case 1: /*10 + 4 bits; {0,15}*/
            comprbytes[i>>3] += (128 >> (i%8));
            i += 2;
            mask = 8;
            aux = cs; 
            break;
        case 2: /*110 + 8 bits; {16,271}*/
            comprbytes[i>>3] += (128 >> (i%8));
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            i += 3;
            mask = 128;
            aux = cs - 16;
            break;
        case 3: /*111 + 12 bits; {272,4367}*/
            comprbytes[i>>3] += (128 >> (i%8));
            comprbytes[(i+1)>>3] += (128 >> ((i+1)%8));
            comprbytes[(i+2)>>3] += (128 >> ((i+2)%8));
            i += 3;
            mask = 2048;
            aux = cs - 272;
            break;
    }
    while(mask > 0) {
        comprbytes[i>>3] += (((mask & aux) != 0) << (7 - (i%8)));
        mask = mask >> 1;
        i++;
    }
    /*cargado de los bits comprimidos en el buffer*/
    for(mask = 0; mask < ((i>>3)+1); mask++) {
        cmpr[(j+mask)%2048] = comprbytes[mask];
    }
    /*cargardo de los bits comprimidos en el 
    fichero cuando se ha llenado el buffer*/
    if((j < 1024) != (((j+(i>>3))%2048) < 1024)){
        fwrite(&(cmpr[((j>>10)<<10)]),1,1024,out);
    }
    /*actualizacion del contador de bits comprimidos*/
    return (i - (j%8));
}

/*descripcion de los atributos : 
    cmpr -> buffer de caracteres comprimidos
    txt -> buffer de caracteres descomprimidos
    pos_cmpr -> puntero al buffer de caracteres comprimidos
    pos_txt -> puntero al buffer de caracteres descomprimidos
    map -> mapa de frecuencias y direcciones
    lsfr -> lista de las mas frecuentes
    lsdr -> lista de las direcciones asociadas a las mayores frecuencias*/
int sacafueraconlosbits(char cmpr[3072], char txt[2048], unsigned long pos_cmpr, unsigned int pos_txt,
                        int map[65536],int lsfr[4368],int lsdr[4367]) {
    unsigned int i;
    int mask,aux;
    unsigned long j;
    int cas;
    unsigned char baux;
    /*inicializacion de las variables*/
    cas = -1;
    j = pos_cmpr;
    aux = ((j >> 3) % 3072);
    j = j % 8;
    i = j;
    /*calculo del caso*/
    while((cmpr[(aux + (j>>3))%3072] & (128 >> (j%8)))) {
        j++;
        if((j-i) == 3) {
            break;
        } 
    }
    j++;
    switch(j - i - 1) {
        case 0: /*literal*/
            /*es necesario añadir la actualizacion de frecuencias*/
            mask = 128;
            baux = 0;
            while(mask > 0) {
                baux |= (mask*(((cmpr[(aux + (j >> 3))%3072]) & (128 >> (j%8))) != 0));
                j++;
                mask >>= 1;
            }
            txt[(pos_txt%2048)] = baux;
            break;
        case 1: /*10 + 4 bits; Rango: {0,15}*/
            cas = 0;
            mask = 8;
            break;
        case 2: /*110 + 8 bits; Rango: {16,271}*/
            cas = 16;
            mask = 128;
            break;
        case 3: /*111 + 12 bits; Rango: {272,4367}*/
            j--;
            cas = 272;
            mask = 2048;
            break;
    }
    while(mask > 0) {
        cas += (mask * (0 != ((cmpr[(aux + (j >> 3))%3072]) & (128 >> (j%8)))));
        mask >>= 1;
        j++;
    }
    if(cas == 4367) {
        return 16384;
    }
    if(cas >= 0) {
        aux = lsdr[cas];
        aumentaFr(map,lsfr,lsdr,aux);
        txt[pos_txt%2048] = (char) (aux >> 8);
        txt[(pos_txt+1)%2048] = (char) (aux & 255);
    }
    aux = ((txt[(pos_txt - 1)%2048] << 8 ) + txt[pos_txt%2048]); 
    aumentaFr(map,lsfr,lsdr,aux);
    return (j-i);
}

/*funcion de analisis, devuelve el caso asociado a la compresion que se va a realizar*/
/*
casos de retorno : 
    n < 0 : literal; "no hay coincidencias previas"
    n > 0 : posicion; "hay coincidencias previas, se puede comprimir"
atributos de la funcion: 
    map -> lista de posiciones para todos los posibles casos 
    lsfr -> lista de frecuencias para la ordenacion de las apariciones
    lsdr -> lista de direciones asociada a las frecuencias donde para acceder al map
    e -> los caracteres a analizar 
*/
int analisis(int map[65536],int lsfr[4368],int lsdr[4367], unsigned char e[3]) {
    int res; /*resultado*/
    int aux; /*auxiliar*/
    aux = ((e[1] << 8) + e[2]);
    res = map[aux];
    if(res > 0) { /*cuando se puede comprimir*/
        aumentaFr(map,lsfr,lsdr,aux);
    }
    aux = ((e[0] << 8) + e[1]);
    aumentaFr(map,lsfr,lsdr,aux);
    return res;
}

/*funcion auxiliar para el analisis, aumenta la frecuencia de aparicion en la
libreria para un par especifico

atributos de la funcion:
    map -> lista de posiciones para todos los posibles casos
    lsfr -> lista de frecuencias para la ordenacion de de las apariciones
    lsdr -> lista de direcciones asociada a las frecuencias donde acceder al map
    dr -> direccion de el par al que hay que actualizar en las listas de frecuencia*/
void aumentaFr(int map[65536],int lsfr[4368],int lsdr[4367], int dr) {
    int aux;
    int pos;
    int frec;
    aux = map[dr];
    if(aux < 0) { /*cuando no tiene una posicion asignada "caso dificil"*/
        map[dr] = (aux - 1); 
        frec = (aux*(-1));
        pos = lsfr[4367];
        /*cuando la lista esta completa*/
        if(pos == 4366) {
            if(frec < lsfr[4366]) {
                return;
            }
            /*bucle de actualizacion*/
            map[lsdr[4366]] = ((lsfr[4366]+1)*(-1));
            while(pos > 0) {
                aux = pos - 1;
                if(frec < lsfr[aux]) {
                    break;
                }
                lsfr[pos] = lsfr[aux];
                lsdr[pos] = lsdr[aux];
                aux = lsdr[pos];
                map[aux] = pos;
                pos--;
            }
            lsfr[pos] = frec;
            lsdr[pos] = dr;
            map[lsdr[pos]] = pos;
        }
        /*cuando la lista tiene espacios disponibles*/
        else {
            while(pos > 0) {
                aux = pos - 1;
                if(frec < lsfr[aux]) {
                    break;
                }
                lsfr[pos] = lsfr[aux];
                lsdr[pos] = lsdr[aux];
                aux = lsdr[pos];
                map[aux] = pos;
                pos--;
            }
            lsfr[pos] = frec; 
            lsfr[pos] = frec;
            lsdr[pos] = dr;
            map[dr] = pos;
            lsfr[4367] += 1;         
        }
    }
    else { /*cuando ya tiene una posicion asignada "caso facil"*/
        pos = aux;
        frec = lsfr[pos] + 1;
        while(pos > 0) {
            if(frec < lsfr[pos-1]) {
                break;
            }
            lsfr[pos] = lsfr[pos-1];
            lsdr[pos] = lsdr[pos-1];
            map[(lsdr[pos])] = pos;
            pos--;
        }
        lsfr[pos] = frec;
        if(aux != pos) {
            lsdr[pos] = dr;
            map[lsdr[pos]] = pos;
        }
    }
    return;
}