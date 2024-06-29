#include "compr.h"


int main(int argc, char** argv){
    FILE* fp = fopen(argv[1],"r"), *fp1 = fopen(argv[2],"w");
    if(argc < 3 ) {
        return 1;
    }
    comprime(fp,fp1);
    fclose(fp);
    fclose(fp1);
    return 0;
}