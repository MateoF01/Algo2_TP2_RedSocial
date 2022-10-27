#include "lista.h"
#include <stdio.h>
static const size_t NULO = 0;

typedef struct nodo {
	void *dato;
	struct nodo* prox;
} nodo_t;

struct lista {
	nodo_t* prim;
	nodo_t* ult;
	size_t largo;
};

struct lista_iter {
	lista_t* lista;
	nodo_t* actual;
	nodo_t* anterior;
};

//Crea un nodo nuevo
//POST: Devuelve un nuevo nodo que contiene el valor indicado y prox es NULL.
nodo_t *crear_nodo_lista(void *valor){
	nodo_t *nodo = malloc(sizeof(nodo_t));

	if(nodo == NULL){
		return NULL;
	}

	nodo->dato = valor;
	nodo->prox = NULL;
	return nodo;
}

/* PRIMITIVAS DE LA LISTA */

lista_t *lista_crear(void){

	lista_t *lista = malloc(sizeof(lista_t));

	if(lista == NULL){
		return NULL;
	}

	lista->prim = NULL;
	lista->ult = NULL;
	lista->largo = NULO;
	return lista;
}

bool lista_esta_vacia(const lista_t *lista){

	if(lista->largo != NULO){
		return false;
	}
	return true;
}

bool lista_insertar_primero(lista_t *lista, void *dato){
	nodo_t *nodo = crear_nodo_lista(dato);

	if(nodo == NULL){
		return false;
	}

	if(lista_esta_vacia(lista)){
		lista->ult = nodo;
	}else{
		nodo->prox = lista->prim;
	}

	lista->prim = nodo;
	(lista->largo)++;
	return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato){
	nodo_t *nodo = crear_nodo_lista(dato);

	if(nodo == NULL){
		return false;
	}
	
	if(lista_esta_vacia(lista)){
		lista->prim = nodo;
	}else{
		lista->ult->prox = nodo;
	}

	lista->ult = nodo;
	(lista->largo)++;
	return true;
}

void *lista_borrar_primero(lista_t *lista){

	if(lista_esta_vacia(lista)){
		return NULL;
	}

	if(lista->prim == lista->ult){
		lista->ult = NULL;
	}

	void *primero = lista->prim->dato;
	nodo_t *nodo_aux = lista->prim;
	lista->prim = lista->prim->prox;
	free(nodo_aux);

	(lista->largo)-- ;
	return primero;
}

void *lista_ver_primero(const lista_t *lista){

	if(lista_esta_vacia(lista)){
		return NULL;
	}
	return lista->prim->dato;
}

void *lista_ver_ultimo(const lista_t* lista){

	if(lista_esta_vacia(lista)){
		return NULL;
	}
	return lista->ult->dato;
}

size_t lista_largo(const lista_t *lista){
	return (lista->largo);
}

void lista_destruir(lista_t *lista, void (*destruir_dato)(void *)){

	while(!lista_esta_vacia(lista)){

		if(*destruir_dato != NULL){
			destruir_dato(lista->prim->dato);
		}
		lista_borrar_primero(lista);
	}

	free(lista);
}

/* PRIMITIVA ITERADOR INTERNO */ 

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){

	if(lista_esta_vacia(lista)){
		return;
	}

	nodo_t *nodo = lista->prim;
	while((nodo != NULL) && visitar(nodo->dato, extra)){
		nodo = nodo->prox;
	}

	return;
}

/* PRIMITIVAS ITERADOR EXTERNO */ 

lista_iter_t *lista_iter_crear(lista_t *lista) {

	lista_iter_t *iter = malloc(sizeof(lista_iter_t));

	if (iter == NULL){
		return NULL;
	}
	iter->lista = lista;
	iter->actual = iter->lista->prim;
	iter->anterior = NULL;
	return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter) {

	if (iter->actual == NULL){
		return false;
	}

	iter->anterior = iter->actual;
	iter->actual = iter->actual->prox;
	return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter) {

	if (iter->actual == NULL){
		return NULL;
	}
	return iter->actual->dato;
}

bool lista_iter_al_final(const lista_iter_t *iter) {

	if (iter->actual != NULL){
		return false;
	}
	return true;
}

void lista_iter_destruir(lista_iter_t *iter){
	
	free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato) {

	nodo_t* nuevo_nodo = crear_nodo_lista(dato);
	
	if (nuevo_nodo == NULL){
		return false;
	}

	if (iter->actual == iter->lista->prim){//esta al principio
		nuevo_nodo->prox = iter->actual;
		iter->lista->prim = nuevo_nodo;
		
		if(lista_esta_vacia(iter->lista)){
			iter->lista->ult = nuevo_nodo;
		}
	}
	
	else if (lista_iter_al_final(iter)){//esta al final
		iter->anterior->prox = nuevo_nodo;
		nuevo_nodo->prox = NULL;
		iter->lista->ult = nuevo_nodo;
	}

	else {//esta en el medio
		iter->anterior->prox = nuevo_nodo;
		nuevo_nodo->prox = iter->actual;
	}

	iter->actual = nuevo_nodo;
	iter->lista->largo++;
	return true;
}

void *lista_iter_borrar(lista_iter_t *iter){

	if (iter->actual == NULL){
		return NULL;
	}

	void *dato = iter->actual->dato;
	nodo_t* nodo_a_eliminar = iter->actual; 

	if (iter->actual == iter->lista->prim){//esta al principio
		iter->lista->prim = iter->actual->prox;
	}
	
	else if (iter->actual->prox == NULL){//esta en el ultimo nodo
		iter->anterior->prox = NULL;
		iter->lista->ult = iter->anterior;
	}

	else {//esta en el medio
		iter->anterior->prox = iter->actual->prox;
	}

	iter->actual = iter->actual->prox;
	iter->lista->largo--;
	free(nodo_a_eliminar);
	return dato; 
}
