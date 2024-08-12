#include "compr.h"

int main(int argc, char** argv) {
    int i;
    FILE* fp;
    if(argc < 2) {
        return 1;
    }
    fp = fopen(argv[1],"w");
    for(i = 0; i < 50000000; i++) {
        fprintf(fp,"%c",'\0');
    }
    return 0;
}