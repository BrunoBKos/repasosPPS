#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*en proceso de revision*/
struct cache_s {
    int** arr;
}; typedef struct cache_s cache_t;

/*en proceso de implementacion*/

void procesar_op(int op);

struct ens_s {
    unsigned int pc;
    int** mp;
    int br[32];
    map_t* cache;
}; typedef struct ens_s ens_t;