#define _POSIX_C_SOURCE 200809L
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACIDAD_INICIAL 23 
#define FACTOR_MAX 0.63
#define FACTOR_MIN 0.2
#define TAM 144

static const int VACIO = 0, OCUPADO = 1, BORRADO=-1;    

typedef struct tabla {
	char* clave;
	void* dato;
	int estado;
} campo_t;

struct hash {
	campo_t* tabla;
	size_t capacidad;
	size_t cantidad;
	hash_destruir_dato_t destruir_dato;
};

struct hash_iter {
	hash_t hash;
	size_t actual;
};


hash_t *hash_crear(hash_destruir_dato_t destruir_dato){

	hash_t* hash = malloc(sizeof(hash_t));

	if (hash == NULL){
		return NULL;
	}

	hash->capacidad = CAPACIDAD_INICIAL;
	hash->cantidad = 0;
	hash->destruir_dato = destruir_dato;

	hash->tabla = calloc(hash->capacidad, sizeof(campo_t));
	
	if (hash->tabla == NULL) {
		free(hash);
		return NULL;
	}
	return hash;

}
//djb2 --> http://www.cse.yorku.ca/~oz/hash.html

size_t funcion_de_hash(const char *str, size_t capacidad) {
	size_t hash = 5381;
	int c;

	while (0 != (c = *str++)){
		hash = ((hash << 5) + hash) + (size_t)c;
	}
	return hash % capacidad;
}

bool redimensionar_hash(hash_t* hash, size_t cap){

	campo_t* tabla_original = hash->tabla;
	size_t capacidad_original = hash->capacidad;

	hash->capacidad = cap;
	hash->cantidad = 0;
	hash->tabla = calloc(hash->capacidad, sizeof(campo_t));

	if (hash->tabla == NULL) {
		return false;
	}

	for(size_t i = 0; i < capacidad_original; i++){

		if (tabla_original[i].estado == OCUPADO){
			hash_guardar(hash, tabla_original[i].clave, tabla_original[i].dato);
			free(tabla_original[i].clave);
		}
	}
	
	free(tabla_original);

	return true;
}

size_t probing_lineal(const hash_t* hash, char *clave_copia ,size_t pos){
    
    while (hash->tabla[pos].estado != VACIO){
        
        if(hash->tabla[pos].estado == OCUPADO && strcmp(hash->tabla[pos].clave, clave_copia) == 0){
			break;
		}
		pos++;
		if(pos == hash->capacidad){
			pos = 0;
		}
	}
    return pos;
}

size_t buscar_pos(const hash_t *hash, const char *clave){
    
    char clave_copia[TAM];
    strcpy(clave_copia, clave);
	size_t pos = funcion_de_hash(clave_copia, hash->capacidad);
	pos = probing_lineal(hash, clave_copia, pos);
    return pos;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){

	char* clave_copia = strdup(clave);
	size_t pos = funcion_de_hash(clave_copia, hash->capacidad);
	pos = probing_lineal(hash, clave_copia ,pos);    
	
	if(hash_pertenece(hash, clave_copia)){

		if (hash->destruir_dato != NULL){ 
			hash->destruir_dato(hash->tabla[pos].dato);
		}
		hash->tabla[pos].dato = dato;
		free(clave_copia);
		return true;
	}

	hash->tabla[pos].clave = clave_copia;
	hash->tabla[pos].dato = dato;
	hash->tabla[pos].estado = OCUPADO;
	hash->cantidad++; 

	float factor_de_carga = (float)hash->cantidad / (float)hash->capacidad;
	  
	if (factor_de_carga > FACTOR_MAX){
		redimensionar_hash(hash, hash->capacidad*2);
	}
	return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
	
	size_t pos = buscar_pos(hash, clave);

	if(hash->tabla[pos].estado != OCUPADO){ 
		return NULL;
	}

	void* dato = hash->tabla[pos].dato;
	
    if (hash->tabla[pos].clave != NULL){
        free(hash->tabla[pos].clave);	
    }
    
    hash->tabla[pos].dato = NULL;
	hash->tabla[pos].estado = BORRADO;
	hash->cantidad--; 

	float factor_de_carga = (float)hash->cantidad / (float)hash->capacidad;
    
	if (hash->capacidad/2 >= CAPACIDAD_INICIAL && factor_de_carga < FACTOR_MIN){
		redimensionar_hash(hash, hash->capacidad/2);
	}
	
	return dato;
}

void *hash_obtener(const hash_t *hash, const char *clave){

	size_t pos = buscar_pos(hash, clave);
	return (hash->tabla[pos].estado != OCUPADO) ? NULL : (hash->tabla[pos].dato);
}

bool hash_pertenece(const hash_t *hash, const char *clave){

	size_t pos = buscar_pos(hash, clave);
	return (hash->tabla[pos].estado != OCUPADO) ? false : true;
}

size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}

void hash_destruir(hash_t *hash){

	for(size_t i = 0; i < hash->capacidad; i++){
		
		if(hash->tabla[i].estado == OCUPADO){

            if (hash->destruir_dato == NULL) { 
                hash->tabla[i].dato = NULL;
            }
            else {
                hash->destruir_dato(hash->tabla[i].dato);
            }

			free(hash->tabla[i].clave);
		}
	}

	free(hash->tabla); 
	free(hash);
}

/* PRIMITIVAS DEL ITERADOR */

hash_iter_t *hash_iter_crear(const hash_t *hash){

	hash_iter_t *iter = malloc(sizeof(hash_iter_t));

	if(iter == NULL){
		return NULL;
	}

	size_t act = 0;

	while(act < hash->capacidad && hash->tabla[act].estado != OCUPADO){
		act++;
	}
	if(act == hash->capacidad-1 &&  hash->tabla[act].estado != OCUPADO){
		act++;
	}

	iter->hash = *hash;
	iter->actual = act;
	return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	iter->actual++ ;

	while((iter->actual < iter->hash.capacidad) && (iter->hash.tabla[iter->actual].estado != OCUPADO)){
		iter->actual++;
	}

	if(iter->actual >= iter->hash.capacidad){
		return false;
	}

	return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if(hash_iter_al_final(iter)){
		return NULL;
	}
	return iter->hash.tabla[iter->actual].clave;
}

bool hash_iter_al_final(const hash_iter_t *iter){
	return iter->actual >= iter->hash.capacidad;
}

void hash_iter_destruir(hash_iter_t *iter){
	free(iter);
}