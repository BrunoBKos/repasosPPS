#include "88110.h"
#include "map.h"


void procesar_op(int op,ens_t* ens) {
    unsigned char oper, i;
    int res, rd, s1;
    int aux;
    unsigned int auxu;
    oper = (unsigned char) (((unsigned int) op ) >> 26);  
    i = oper >> 1;
    rd = ((unsigned int) op) << 6;
    rd = ((unsigned int) rd) >> 27;
    s1 = ((unsigned int) op) << 11;
    s1 = ((unsigned int) s1) >> 27;
    s1 = (*ens).rd[s1];
    if((oper % 64 ) == 1) {
        aux = op << 6;
        aux = aux >> 6;
        auxu = ((unsigned int) op) << 6;
        auxu = auxu >> 6;
    }
    else {
        auxu = ((unsigned int) op) << 16;
        auxu = auxu >> 27;
        aux = rd[auxu];
    }
    switch(i) {
        case 0b00001: /*ld*/
        break;
        case 0b00010: /*ld.b*/
        break;
        case 0b00011: /*ld.bu*/
        break;
        case 0b00100: /*st*/
        break;
        case 0b00101: /*st.b*/
        break;  
        case 0b00110: /*cmp*/
        break;
        case 0b00111: /*bb1*/
        break;
        case 0b01000: /*bb0*/
        break;
        case 0b01001: /*br*/
        break;
        case 0b01010: /*jmp*/
        break;
        case 0b01100: /*bsr*/
        break;
        case 0b01110: /*and*/
            
        break;
        case 0b01111: /*and.u*/

        break;
        case 0b10000: /*or*/
        break;  
        case 0b10001: /*or.u*/
        break;
        case 0b10010: /*xor*/
        break;
        case 0b10011: /*xor.u*/
        break;
        case 0b10100: /*add*/
            aux = aux + s1;
        break;
        case 0b10101: /*add.u*/
            auxu = auxu + s1;
        break;
        case 0b10110: /*sub*/
            aux = aux - s1;
        break;
        case 0b10111: /*sub.u*/
            auxu = auxu - s1;
        break;
        case 0b11000: /*mul*/
            aux = aux * s1;
        break;
        case 0b11001: /*mul.u*/
            auxu = auxu * s1;
        break;
        case 0b11010: /*div*/
            aux = aux / s1;
        break;
        case 0b11011: /*div.u*/
            auxu = auxu / s1;
        break;
        case 0b11100: /*mak*/
        break;
        case 0b11101: /*extu*/ 
        break;          
    }

    if(rd >= 0) {
        (*ens).br[j] = res;
    }
    rd *= -1;
    switch(j) {
        default:
        break;
    }

}