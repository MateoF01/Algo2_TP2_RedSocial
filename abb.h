#ifndef ABB_H
#define ABB_H

#include <stdbool.h>
#include <stddef.h>

typedef struct abb abb_t;
typedef struct abb_iter abb_iter_t;

//Funcion que compara las claves que se guarden en el abb
//Post: Devuelve 0 si las claves son iguales, un entero positivo si la primera clave es mayor que la segunda, y un entero
//      negativo si la segunda clave es mayor que la primera.
typedef int (*abb_comparar_clave_t) (const char *, const char *);

//Funcion que permite destruir los datos que se guardaran en el abb.
typedef void (*abb_destruir_dato_t) (void *);


/* PRIMITIVAS DEL ABB */

//Crea un abb
//Post: Devuelve un abb vacio, en caso de error devuelve NULL.
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

//Guarda un elemento en el arbol. En caso de que la clave ya existiera en el abb, reemplaza el dato. 
//Sino puede guardarlo, devuelve false
//Pre: La estructura arbol fue inicializada
//Post: Devuelve true si se almacenó el par (clave, dato) correctamente, y false en caso contrario.
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

//Borra un elemento del arbol y devuelve el dato asociado. Devuelve NULL si el dato no estaba.
//Pre: La estructura arbol fue inicializada
//Post: Se devolvio el elemento eliminado. En el caso de que estuviera guardado, devolvio NULL.
void *abb_borrar(abb_t *arbol, const char *clave);

//Obtiene el dato de un elemento del arbol, si la clave no se encuentra devuelve NULL.
//Pre: La estructura fue inicializada.
//Post: Devuelve el elemento buscado, sino se encuntra devuleve NULL.
void *abb_obtener(const abb_t *arbol, const char *clave);

//Determina si una clave pertenece o no al arbol.
//Pre: La estructura fue inicializada.
//Post: Devuelve true si la clave pertenece al arbol, y false en caso contrario.
bool abb_pertenece(const abb_t *arbol, const char *clave);

//Pre: la estructura fue inicializada.
//Post: Devuelve la cantidad de elemntos que hay en el arbol.
size_t abb_cantidad(const abb_t *arbol);

//Destruye el arbol liberando toda la memoria pedida, y llamando a la funcion destruir dato para cada elemento.
//Pre: La estructura fue inicializada
//Post: Se destruyo el arbol.
void abb_destruir(abb_t *arbol);

/* PRIMITIVA DEL ITERADOR INTERNO */

// Itera el abb en orden hasta que la funcion visitar devuelva false. Si nunca lo hace, itera hasta recorrer todos los elementos.
// extra es un puntero que permite guardar informacion entre cada llamada de la funcion visitar.
// Pre: La estructura fue inicializada
// Post: se recorrio el arbol in order y se aplico la funcion visitar a cada elemento. Si el arbol era nulo, no hace nada.
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);

/* PRIMITIVAS DEL ITERADOR EXTERNO */

// Crea una iterador deL ABB que se posiciona en el primer elemento del mismo.
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

// Avanza iterador, realizando un recorrido in order.
bool abb_iter_in_avanzar(abb_iter_t *iter);

// Devuelve la clave actual, esa clave no se puede modificar ni liberar.
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

// Comprueba si terminó la iteración
bool abb_iter_in_al_final(const abb_iter_t *iter);

// Destruye el iterador
void abb_iter_in_destruir(abb_iter_t* iter);

#endif  // ABB_H
