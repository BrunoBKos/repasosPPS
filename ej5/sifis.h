#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char params[3];

struct pag_s {
    void* ent[26];
}; typedef struct pag_s pag_t;

/*main*/ 
int main (int argc, char** argv);

/*inicializar una pagina*/ 
pag_t* new_pag(); 

/*estructurar*/
pag_t* cargar(FILE* fp);

/*meter*/
int meter(pag_t* res,char* linea); 

/*fallo*/
void fallo(pag_t*,int n1, int n2, int s,char* linea);

/*liberar memoria*/
void liberar(pag_t* pag);

/*push*/
int push(pag_t* est,FILE* fp);

/*TLB traducciones*/
void* TLB (void);
