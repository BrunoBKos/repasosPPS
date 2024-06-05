#include "map.h"
int main( void ) {
    int ok,i;
    int keys[1024];
    char* str = "iteracion\0";
    map_t* mapa;
    i = 0;
    ok = 0;
    mapa = newmap(NULL,NULL);
    fprintf(stdout,"%ld ",sizeof(int));
    while(ok == 0 && i < 1024) {
        put(mapa,&keys[i],str);
        i++;
    }
    if(ok == 1) {
        fprintf(stdout,"%d",i);
    }
    i = 0;
    while(ok == 0 && i < 1024) {
        str = get(mapa,&keys[i]);
        
        if(str != NULL) {
            fprintf(stdout,"%d ",i);
        }
        i++;
    }
    free(mapa);
    return ok;
}
map_t* newmap(unsigned long(*hash)(void* key), int(*equals)(void* one,void* two)) {
    int i;
    map_t* res;
    res = (map_t*) malloc(sizeof(map_t));
    if(res != NULL) {
        for(i = 0; i < 1024; i++) {
            (*res).table[i] = NULL;
        }
        if(hash == NULL) {
            (*res).hash = hs;
        }
        else {
            (*res).hash = hash;
        }
        if(equals == NULL) {
            (*res).equals = eq;
        }
        else {
            (*res).equals = equals;
        }
    }
    return res;
}

unsigned long hs(void* key) {
    return (unsigned long) key;
}

int eq(void* one,void* two) {
    return one == two;
}

int put(map_t* map, void* key, void* val) {
    int ok = 0;
    int aux;
    lista_t* lst;
    lista_t* l;
    void** list;
    void** lt;
    void* aux1;
    void* aux2;
    void* aux3;
    unsigned long hash;
    if(key == NULL || val == NULL) {
        return 1;
    }
    hash = (*map).hash(key);
    aux = (hash % 512)*2;
    if((*map).table[aux] == NULL) {
        (*map).table[aux] = key;
        (*map).table[aux+1] = val;
    }
    else if((*map).table[aux+1] == NULL) {
        lst = (*map).table[aux];
        list = (*lst).elems;
        ok = binarysearch(lst,hash,(*map).hash);
        if(ok > 0) {
            if(list[(ok*2)+1] != NULL) {
                if((*map).equals(key,list[ok*2])) {
                    list[(ok*2) + 1] = val;
                    ok = 0;
                }
                else {
                    l = (lista_t*) malloc(sizeof(lista_t));
                    if(l == NULL) {
                        return 1;
                    }
                    lt = (void**) malloc(sizeof(void*)*6);
                    if(lt == NULL) {
                        return 1;
                    }
                    lt[0] = list[ok*2];
                    lt[1] = list[ok*2 + 1];
                    lt[2] = key;
                    lt[3] = val;
                    lt[4] = NULL;
                    lt[5] = NULL;
                    (*l).tam = 6;
                    (*l).elems = lt;
                    list[ok*2] = l;
                    list[ok*2 + 1] = NULL;
                }
            }
            else {
                lst = (lista_t*) list[ok*2];
                list = (*lst).elems;
                ok = 0;
                aux = 0;
                while(ok == 0 && list[aux+2] != NULL) {
                    if((*map).equals(key,list[aux])) {
                        list[aux+1] = val;
                        ok = 1;
                    }
                    aux = aux + 2;
                }
                if(ok == 0) {
                    list = (void**) realloc(list,(*lst).tam + 2);
                    if(list == NULL) {
                        return 1;
                    }
                    list[aux] = key;
                    list[aux + 1] = val;
                    list[aux+2] = NULL;
                    list[aux+3] = NULL;
                    (*lst).elems = list;
                }
                ok = 0;
            }
        }
        else {
            list = realloc(list,sizeof(void*)*((*lst).tam+2));
            if(list == NULL) {
                return 1;
            }
            (*lst).elems = list;
            (*lst).tam = ((*lst).tam + 2);
            aux = ((ok + 1)*(-1))*2;
            aux1 = key;
            aux3 = val;
            while(list[aux] != NULL) {
                aux2 = list[aux];
                list[aux] = aux1;
                aux1 = aux2;
                aux2 = list[aux+1];
                list[aux+1] = aux3;
                aux3 = aux2;
                aux = aux + 2;
            }
            list[aux] = aux1;
            list[aux+1] = aux3;
            list[aux+2] = NULL;
            list[aux+3] = NULL;
        }
    }
    else {
        if((*map).hash((*map).table[aux]) == hash) {
            if((*map).equals(key,(*map).table[aux])) {
                (*map).table[aux+1] = val;
            }
            else {
                lst = (lista_t*) malloc(sizeof(lista_t));
                if(lst == NULL) {
                    return 1;
                }
                list = (void**) malloc(sizeof(void*)*4);
                if(list == NULL) {
                    return 1;
                }
                list[2] = NULL;
                list[3] = NULL;
                list[1] = NULL;
                l = (lista_t*) malloc(sizeof(lista_t));
                if(l == NULL) {
                    return 1;
                }
                lt = (void**) malloc(sizeof(void*)*6);
                if(lt == NULL) {
                    return 1;
                }
                lt[0] = (*map).table[aux];
                lt[1] = (*map).table[aux+1];
                lt[2] = key;
                lt[3] = val;
                lt[4] = NULL;
                lt[5] = NULL;
                (*l).tam = 6;
                (*l).elems = lt;
                (*lst).tam = 4;
                (*lst).elems = list;
                (*map).table[aux] = lst;
                (*map).table[aux+1] = NULL;
            }
        }
        else {
            lst = (lista_t*) malloc(sizeof(lista_t));
            if(lst == NULL) {
                return 1;
            }
            list = (void**) malloc(sizeof(void*)*6);
            if(list == NULL) {
                return 1;
            }
            list[4] = NULL;
            list[5] = NULL;
            if (hash < (*map).hash((*map).table[aux]) ) {
                list[0] = key;
                list[1] = val;
                list[2] = (*map).table[aux];
                list[3] = (*map).table[aux+1];
            }
            else {
                list[2] = key;
                list[3] = val;
                list[0] = (*map).table[aux];
                list[1] = (*map).table[aux+1];
            }
            (*lst).tam = 6;
            (*lst).elems = list;
            (*map).table[aux] = lst;
            (*map).table[aux+1] = NULL;
            ok = 0;
        }
    }
    return ok;
}

void* get(map_t* map, void* key) {
    void* res;
    int aux, i;
    lista_t* lst;
    void** list;
    unsigned long hash;
    if(key == NULL) {
        return NULL;
    }
    hash = (*map).hash(key);
    aux = (hash % 512)*2;
    if((*map).table[aux] == NULL) {
        res = NULL;
    }
    else if((*map).table[aux+1] == NULL) {
        lst = (lista_t*) (*map).table[aux];
        list = (*lst).elems;
        i = binarysearch(lst, hash,(*map).hash);
        if(i < 0) {
            res = NULL;
        }
        else {
            if(list[i*2 + 1] != NULL) {
                if((*map).equals(key,list[i*2])) {
                    res = list[i*2];
                }
                else {
                    return NULL;
                }
            }
            else {
                lst = (lista_t*) list[i*2];
                list = (*lst).elems;
                i = 0;
                aux = 0;
                while(i == 0 && list[aux+2] != NULL) {
                    if((*map).equals(key,list[aux])) {
                        res = list[aux+1];
                        i = 1;
                    }
                    aux = aux + 2;
                }
                if(i == 0) {
                    return NULL;
                }
            }
        }
    }
    else {
        if((*map).equals(key,(*map).table[aux])) {
            res = (*map).table[aux+1];
        }
        else {
            res = NULL;
        }
    }
    return res;
}

int rm(map_t* map, void* key) {
    int res;
    int aux, i;
    void** list;
    lista_t* lst;
    unsigned long hash;
    if(key == NULL) {
        return 1;
    }
    hash = (*map).hash(key);
    aux = (hash % 512)*2;
    if((*map).table[aux] == NULL) {
        res = 1;
    }
    else if((*map).table[aux+1] == NULL) {
        lst = (lista_t*) (*map).table[aux];
        list = (*lst).elems;
        i = binarysearch(lst, hash,(*map).hash);
        if(i < 0) {
            res = 1;
        }
        else {
            res = aux;
            aux = i*2;
            while(list[aux] != NULL) {
                list[aux] = list[aux+2];
                list[aux+1] = list[aux+3];
                aux = aux + 2;
            }
            list = realloc(list, sizeof(void*)*((*lst).tam - 2));
            if(list == NULL) { return -1;}
            (*lst).tam = (*lst).tam - 2;
            if((*lst).tam < 6) {
                (*map).table[aux] = list[0];
                (*map).table[aux+1] = list[1];
                free(list);
                free(lst);
            }
            (*lst).elems = list;
            res = 0;
        }
    }
    else {
        if((*map).table[aux] == key) {
            (*map).table[aux] = NULL;
            (*map).table[aux+1] = NULL;
            res = 0;
        }
        else {
            res = 1;
        }
    }
    return res;
}

int binarysearch(lista_t* list, unsigned long elem, unsigned long(*hash)(void* key)) {
    int res,ok;
    int sup,inf;
    void** lst;
    ok = 1;
    inf = 0;
    sup = (((*list).tam - 2)/2)-1;
    lst = (*list).elems;
    while(ok == 1 && (sup-inf) > 1 ) {
        res = ((sup - inf)/2) + inf;
        if(elem < hash(lst[res*2])) {
           sup = res; 
        }
        else if(elem > hash(lst[res*2])) {
            inf = res;
        }
        else {
            ok = 0;
        }
    }
    if(ok == 0) {
        return res;
    }
    else {
        if(elem == hash(lst[sup*2])) {
           return sup;
        }
        if(elem == hash(lst[inf*2])) {
            return inf;
        }
        if(elem > hash(lst[sup*2])) {
            return (sup + 2)*(-1);
        }
        if(elem > hash(lst[inf*2])) {
            return (sup + 1)*(-1);
        }
        return (inf + 1)*(-1);
    }

    return res;
}