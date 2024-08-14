#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*estructuras*/

struct cola_s {
    int tam; 
    int elems[4];
}; typedef struct cola_s cola_t;

struct cmprsor_s {
    unsigned int pos_lns;
    char lineas[6144];
    unsigned long pos_cmpr;
    char text_comp[2048]; 
}; typedef struct cmprsor_s cmprsor_t;

/*funcines principales*/

/*funcion principal de compresion, se le pasan dos argumentos,
un fichero de entrada, el que se quiere comprimir, y un
fichero de salida, en el que se cargara el texto comprimido*/
void comprime(FILE* in, FILE* out);

/*funcion principal de descompresion, se le pasan dos argumentos,
un fichero de entrada, el que se quiere descomprimir, y un
fichero de salida, en el que se cargara el texto descomprimido*/
void descomprime(FILE* in, FILE* out);

/*funciones auxiliares*/

/*funcion de uso principal en la compresion, una vez estudiado 
el caso carga los bits comprimidos en el buffer y actualiza contadores*/
void metedentroconlosbits(int e[2], FILE* out, cmprsor_t* cmpr, int cs, int hs[256], int map[6144]);

/*funcion de uso principal en la descompresion, se encarga de
traducir los bits comprimidos y añadir los caracteres
descomprimidos al buffer y actualiza los contadores*/
void sacafueraconlosbits(cmprsor_t* cmpr, cola_t* cola);

/*funcion para buscar la concidencia mas larga de caracteres
que se encuantran dentro de los carcacteres anteriormente leidos
dentro de un limite, en este caso 35840 caracteres*/
int buscaMax(char* str, int from, int to,int* pos, int hs[256], int map[6144]);

/*funcion auxiliar que se usa en buscaMax para determinar cuantos
caracteres coinciden entre dos lineas de caracteres dentro de un limite*/
int coinCar(char* str,int st1, int st2, int to);

/*funciones para el trabajo con la cola circular*/

/*funcion para añadir un caso a la cola de distancias*/
void enqueue(cola_t* cola, int e);

/*funcion que devuelve la distancia asociada al inice
que se encuentra en la cola*/
int sacacola(cola_t* cola,int e);

/*funcion que devuelve la posicion en que se encuentra
la distancia dentro de la cola,en caso de no haber conincidencias
devuelve una distancia fuera del caso*/
int compara(cola_t* cola, int e);
