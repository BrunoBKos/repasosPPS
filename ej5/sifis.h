#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char params[2];

struct pag_s {
    void* ent[26];
}; typedef struct pag_s pag_t;

/*main*/ 
int main (int argc, char** argv);

/*inicializar una pagina*/ 
pag_t* new_pag(); 

/*estructurar*/
pag_t* cargar(FILE* fp, FILE* out);

/*meter*/
int meter(pag_t* res,char* linea, FILE* out);

/*fallo*/
void fallo(FILE* out,char* linea);

/*sacar*/ 
void sacar(char* linea, FILE* fp);

/*liberar memoria*/
void liberar(pag_t* pag);

/*push*/
int push(pag_t* pag,FILE* fp);

/*modifica*/
int modifica(FILE* fp,FILE* out, pag_t* tp);