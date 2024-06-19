#include "comp.h"

int main(int argc, char** argv) {
    if(argc < 4) {
        return 1;
    }
    if((argv[3])[0] == 'c') {
        return comprime(argv[1],argv[2]);
    }
    else if((argv[3])[0] == 'd') {
        return descomprime(argv[1],argv[2]);
    }
    else {
        return 2;
    }
}