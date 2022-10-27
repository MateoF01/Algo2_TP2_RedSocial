#define _POSIX_C_SOURCE 200809L

#include "abb.h"
#include "pila.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM 144
static const size_t NINGUN_HIJO = 0 , UN_HIJO = 1 , DOS_HIJOS = 2;

typedef struct nodo {
	struct nodo* izq;
	struct nodo* der;
	char* clave;
	void* dato;
} nodo_t;

struct abb {
	nodo_t *raiz;
	size_t cantidad;
	abb_destruir_dato_t destruir_dato;
	abb_comparar_clave_t cmp;
};

struct abb_iter {
    pila_t* pila;
};

nodo_t* crear_nodo(char* clave, void* dato){
	nodo_t* nodo = malloc(sizeof(nodo_t));

	if(nodo == NULL){
		return NULL;
	}

	nodo->clave = strdup(clave);
	nodo->dato = dato;
	nodo->izq = NULL;
	nodo->der = NULL;
	return nodo;
}

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
	abb_t *arbol= malloc(sizeof(abb_t));

	if(arbol == NULL){
		return NULL;
	}

	arbol->cantidad = 0;
	arbol->destruir_dato = destruir_dato;
	arbol->cmp = cmp;
	arbol->raiz = NULL;

	return arbol;
}

//Devuelve el nodo que contenga la clave buscada, si no existe esa clave en el arbol, devuelve su nodo anterior (o sea, su padre).
nodo_t * buscar(abb_comparar_clave_t cmp, nodo_t *nodo, const char *clave_copia, nodo_t *nodo_anterior){
	
	if(nodo == NULL){
		return nodo_anterior;
	}
	
	int comparacion = cmp(clave_copia, nodo->clave);
	
	if(comparacion == 0){
		return nodo;

	} else if (comparacion < 0){
		return buscar(cmp, nodo->izq, clave_copia, nodo);

	} else {
		return buscar(cmp, nodo->der, clave_copia, nodo);

	} 

}
// pre: el nodo existe
nodo_t * buscar_padre(abb_comparar_clave_t cmp, nodo_t *nodo, const char *clave_copia, nodo_t *nodo_anterior){
	
	int comparacion = cmp(clave_copia, nodo->clave);
	
	if(comparacion == 0){
		return nodo_anterior;

	} else if (comparacion < 0){
		return buscar_padre(cmp, nodo->izq, clave_copia, nodo);

	} else {
		return buscar_padre(cmp, nodo->der, clave_copia, nodo);

	} 

}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
	char clave_copia[TAM];
    strcpy(clave_copia, clave);
	nodo_t *nodo_buscado = buscar(arbol->cmp, arbol->raiz, clave_copia, arbol->raiz);
	
	if(nodo_buscado != NULL && arbol->cmp(clave_copia, nodo_buscado->clave) == 0){
		if(arbol->destruir_dato != NULL){
			arbol->destruir_dato(nodo_buscado->dato);
		}
		nodo_buscado->dato = dato; 
		return true;
	}

	nodo_t *nodo_nuevo = crear_nodo(clave_copia, dato); 
	if(nodo_nuevo == NULL){
		return false;
	}

	if(abb_cantidad(arbol) == 0){
		arbol->raiz = nodo_nuevo;
		arbol->cantidad++;
		return true;
	}

	if(arbol->cmp(clave_copia, nodo_buscado->clave) < 0){
		nodo_buscado->izq = nodo_nuevo;
	} else{
		nodo_buscado->der = nodo_nuevo;
	}

	arbol->cantidad++;
	return true;
}

size_t cantidad_de_hijos(nodo_t *nodo){
	if(nodo->izq != NULL && nodo->der != NULL){
		return DOS_HIJOS;
	}else if(nodo->izq == NULL && nodo->der == NULL){
		return NINGUN_HIJO;
	}else {
		return UN_HIJO;
	}
}

nodo_t *buscar_reemplazante(nodo_t *nodo){
	nodo_t *reemplazante = nodo;

	while(reemplazante->izq != NULL){
		reemplazante = reemplazante->izq;
	}

	return reemplazante;
}

void *abb_borrar(abb_t *arbol, const char *clave){ 
	char clave_copia[TAM];
    strcpy(clave_copia, clave);

	nodo_t *nodo_buscado = buscar(arbol->cmp, arbol->raiz, clave_copia, arbol->raiz);
	
	if(nodo_buscado == NULL || arbol->cmp(clave_copia, nodo_buscado->clave) != 0){
		return NULL;
	}

	void *dato_eliminado = nodo_buscado->dato;

	if(cantidad_de_hijos(nodo_buscado) < DOS_HIJOS){ // CASO CON 0 o 1 HIJO

		nodo_t *hijo_izq = nodo_buscado->izq;
		nodo_t *hijo_der = nodo_buscado->der;
		
		nodo_t *abuelo = buscar_padre(arbol->cmp, arbol->raiz, clave_copia, NULL); 

		if(abuelo == NULL){ //si borre la raiz, entonces el abuelo es NULL
			if(hijo_izq != NULL){
				arbol->raiz = hijo_izq;
			}else if(hijo_der != NULL){
				arbol->raiz = hijo_der;
			} else {
				arbol->raiz = NULL;
			}
			free(nodo_buscado->clave);
			free(nodo_buscado);
			arbol->cantidad--;
			return dato_eliminado;
		}

		if(arbol->cmp(clave_copia, abuelo->clave) < 0){
			if(hijo_izq != NULL){
				abuelo->izq = hijo_izq;
			}else{
				abuelo->izq = hijo_der;
			}
			
		} else {
			if(hijo_izq != NULL){
				abuelo->der = hijo_izq;
			}else{
				abuelo->der = hijo_der;
			}
		}

		arbol->cantidad--;
		free(nodo_buscado->clave);
		free(nodo_buscado);
		return dato_eliminado;

	} else { // CASO CON 2 HIJOS

		nodo_t *reemplazante = buscar_reemplazante(nodo_buscado->der);
		char clave_rem[TAM];
		strcpy(clave_rem, reemplazante->clave);
		void *dato_rem = reemplazante->dato;

		abb_borrar(arbol, clave_rem);

		strcpy(nodo_buscado->clave, clave_rem);
		nodo_buscado->dato = dato_rem;
	}
	
	return dato_eliminado;
}

void *abb_obtener(const abb_t *arbol, const char *clave){
	char clave_copia[TAM];
    strcpy(clave_copia, clave);
	nodo_t *nodo_buscado = buscar(arbol->cmp, arbol->raiz, clave_copia, arbol->raiz);

	if(nodo_buscado == NULL || arbol->cmp(clave_copia, nodo_buscado->clave) != 0){
		return NULL;
	}

	return nodo_buscado->dato;
}


bool abb_pertenece(const abb_t *arbol, const char *clave){
	char clave_copia[TAM];
    strcpy(clave_copia, clave);
	nodo_t *nodo_buscado = buscar(arbol->cmp, arbol->raiz, clave_copia, arbol->raiz);


	if(nodo_buscado == NULL || arbol->cmp(clave_copia, nodo_buscado->clave) != 0) {
		return false;
	}

	return true;
}

size_t abb_cantidad(const abb_t *arbol){
	return arbol->cantidad;
}

void _abb_destruir(nodo_t *nodo, abb_destruir_dato_t destruir_dato){

	if(nodo == NULL){
		return;
	}

	_abb_destruir(nodo->izq, destruir_dato);
	_abb_destruir(nodo->der, destruir_dato);

	if(destruir_dato != NULL){
		destruir_dato(nodo->dato);
	}
	free(nodo->clave);
	free(nodo);
}

void abb_destruir(abb_t *arbol){
	_abb_destruir(arbol->raiz, arbol->destruir_dato);
	arbol->cantidad = 0;
	free(arbol);
}

/* PRIMITIVA DEL INTERADOR INTERNO IN ORDER*/

void _abb_in_order(nodo_t *nodo, bool visitar(const char *, void *, void *), void *extra, bool* visitado){

	if(nodo == NULL){
		return;
	}

	_abb_in_order(nodo->izq, visitar, extra, visitado);

	if(!(*visitado)){
		return;
	}

	if(!visitar(nodo->clave, nodo->dato, extra)){
		*visitado = false;
		return;
	}
	
	_abb_in_order(nodo->der, visitar, extra, visitado);
	
}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra){
	bool visitado = true;
	_abb_in_order(arbol->raiz, visitar, extra, &visitado);

}


/* PRIMITIVAS ITERADOR EXTERNO INORDER*/

abb_iter_t *abb_iter_in_crear(const abb_t *arbol){

    abb_iter_t* iter = malloc(sizeof(abb_iter_t));

    if (iter == NULL){
        return NULL;
    }
 
    iter->pila = pila_crear();

    if(iter->pila == NULL){
    	free(iter);
    	return NULL;
    } 

    nodo_t *aux = arbol->raiz;
    while (aux != NULL){
        pila_apilar(iter->pila, aux);
        aux = aux->izq;
    }										

    return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter) {
	
	if(abb_iter_in_al_final(iter)){
		return false;
	}

    nodo_t *nodo = pila_desapilar(iter->pila);

   	nodo = nodo->der;
    if (nodo != NULL){
        pila_apilar(iter->pila, nodo);
        nodo = nodo->izq;

        while (nodo != NULL){
            pila_apilar(iter->pila, nodo);
            nodo = nodo->izq;
        }
    }

    return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
	if(abb_iter_in_al_final(iter)){
		return NULL;
	}

    nodo_t* actual = pila_ver_tope(iter->pila);
    return actual->clave;
}

bool abb_iter_in_al_final(const abb_iter_t *iter){
    
    return pila_esta_vacia(iter->pila);
}

void abb_iter_in_destruir(abb_iter_t* iter){

    pila_destruir(iter->pila);
    free(iter);
}
