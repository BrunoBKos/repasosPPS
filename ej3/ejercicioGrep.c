#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char** argv) {

    FILE* fp;
    char* texto; 
    char linea[2049];
    if(argc < 3) {
        return 1;
    }
    fp = fopen(argv[2],"r");
    if(fp == NULL) {
        return 1;
    }
    texto = argv[1];
    while(fgets(linea,2048,fp) != NULL) {
        if(strstr(linea,texto) != NULL) {
            fprintf(stdout,"%s",linea);
        }
    }
    fclose(fp);

    return 0;
}