#include "compr.h"

int compararl(FILE* r, FILE* l);

int main(int argc, char** argv){
    FILE* fp = fopen(argv[1],"r"), *fp1;
    if(argc < 4 ) {
        return 1;
    }
    if((argv[3])[0] == 'c') {
        fp1 = fopen(argv[2],"w");
        comprime(fp,fp1);
    }
    else if((argv[3])[0] == 'd') {
        fp1 = fopen(argv[2],"w");
        descomprime(fp,fp1);
    }
    else {
        fp1 = fopen(argv[2],"r");
        printf("caracteres iguales: %d", compararl(fp,fp1));
    }
    fclose(fp);
    fclose(fp1);
    return 0;
}

int compararl(FILE* r, FILE* l) {
    char rl[1024];
    char ll[1024];
    int ok, tam1, tam2, res;
    res = 0;
    ok = 0;
    while(!ok) {
        fprintf(stderr,"aqui");
        tam1 = fread(rl,1,1024,r);
        tam2 = fread(ll,1,1024,l);
        ok = (tam1 != tam2); 
        tam1 = (tam1 < tam2 ? tam1 : tam2);
        for(tam2 = 0; tam2 < tam1; tam2++) {
            if(rl[tam2] == ll[tam2]) {
                res++;
            }
            else {
                ok = 1;
                break;
            }
        }
    }
    return res;
}