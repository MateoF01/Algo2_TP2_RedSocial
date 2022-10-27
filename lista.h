#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdlib.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* La cola está planteada como una cola de punteros genéricos. */

typedef struct lista lista_t;
typedef struct lista_iter lista_iter_t;

/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t *lista_crear(void);

// Devuelve verdadero si la lista no tiene elementos, false en caso contrario.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Agrega un nuevo elemento a la lista en el primer lugar. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista en el primer lugar.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un nuevo elemento a la lista en el ultimo lugar. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista en el ultimo lugar.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Elimina el primer elemento de la lista. Si la lista tiene elementos, se quita el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvío el primer elemento si la lista no estaba vacia y ahora la lista
// tiene un elemento menos
void *lista_borrar_primero(lista_t *lista);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void *lista_ver_primero(const lista_t *lista);

// Obtiene el valor del ultimo elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del ultimo, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el ultimo elemento de la lista, cuando no está vacía.
void *lista_ver_ultimo(const lista_t* lista);

// Devuelve la cantidad de elementos que hay en la lista. 
// Pre: la lista fue creada
// Post: se devolvío la cantidad de elemento de la lista.
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void (*destruir_dato)(void *));

// Itera la lista hasta que se cumpla hasta que la funcion visitar devuelva false.
// Si nunca lo hace, itera hasta llegar al final de la lista.
// Pre: la lista fue creada
// Post: se recorrio la lista
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR
 * *****************************************************************/

// Crea una iterador de lista que se posiciona en el primer elemento de la misma.
// Pre: la lista fue creada
// Post: devuelve un nuevo iterador de lista. Si ocurre un error devuelve NULL
lista_iter_t *lista_iter_crear(lista_t *lista);

// Avanza una posicion de la lista
// Pre: el iterador fue creado
// Post: devuelve True si pudo avanzar una posicion, o False en caso contrario
// Si se encuentra en el ultimo elemento de la lista y se quiere avanzar devuelve False
bool lista_iter_avanzar(lista_iter_t *iter);

// Devuelve el elemento al que apunta el iterador actual. Si la lista está vacia devuelve NULL
// Pre: el iterador fue creado
// Post: se devolvio el elemento, o NULL si la lista estaba vacia. 
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Mientras el iterador no este al final devuelve False. Si llega a null devulve True.
// Pre: el iterador fue creado
// Post: devuelve True si esta al final, o false en caso contrario
bool lista_iter_al_final(const lista_iter_t *iter);

// Destruye el iterador
// Pre: el iterador fue creado
// Post: se destruyó el iterador
void lista_iter_destruir(lista_iter_t *iter);

// Inserta el elemento en la posicion actual del iterador. 
// Si se pudo insertar correctamente devuelve true o false en caso contrario
// Pre: el iterador fue creado
// Post: la lista tiene un nuevo elemento en caso de haber podido insertarlo. 
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Elimina el elemento en la posicion actual del iterador y lo devuelve.
// Si la lista está vacia devuelve NULL.
// Pre: el iterador fue creador
// Post: la lista tiene un elemento menos, y se devolvío el elemento eliminado. O se devolvío NULL
// si la lista estaba vacia
void *lista_iter_borrar(lista_iter_t *iter);


/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

// Realiza pruebas sobre la implementación realizada.
void pruebas_lista_estudiante(void);


#endif  // LISTA_H
