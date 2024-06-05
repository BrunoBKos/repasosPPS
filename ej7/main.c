#include "comp.h"

int main(int argc, char** argv) {
    FILE* fp;
    if(argc < 2) {
        return 1;
    }
    fp = fopen(argv[1],"r");
    if(fp == NULL) {
        return 1;
    }
    
    return comprime(fp);
}