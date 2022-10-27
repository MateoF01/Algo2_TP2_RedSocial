#include "heap.h"
#include <stdio.h>
#include <stdlib.h>

static const size_t TAM_INICIAL = 20;
static const size_t FACTOR_CANT = 2;
static const size_t FACTOR_TAM = 4;

struct heap {
    void** datos;
    size_t cant;
    size_t tam;
    cmp_func_t cmp;
};

bool redimensionar_heap(heap_t* heap, size_t tam_nuevo){

    void **datos_nuevo = realloc(heap->datos, tam_nuevo * sizeof(void*));
    
    if (datos_nuevo == NULL) {
        return false;
    }

    heap->datos = datos_nuevo;
    heap->tam = tam_nuevo;

    return true;
}

void swap(void* datos[], size_t pos_x, size_t pos_y) {
    
    void* aux = datos[pos_x];
    datos[pos_x] = datos[pos_y];
    datos[pos_y] = aux;
}

void upheap(void* datos[], cmp_func_t cmp, size_t pos_hijo){

    if(pos_hijo <= 0){
        return;
    }

    size_t pos_padre = (pos_hijo - 1) / 2;
    int comp = cmp(datos[pos_hijo], datos[pos_padre]);

    if(comp > 0){
        swap(datos, pos_hijo, pos_padre);
        upheap(datos, cmp, pos_padre);
    }
    return;
}

size_t maximo(void* datos[], size_t cant, cmp_func_t cmp, size_t padre, size_t h_izq, size_t h_der){

    if(h_izq >= cant){ //esto es, no tiene hijo izq
        return padre;
    } 

    int comp_izq = cmp(datos[h_izq], datos[padre]);

    if(h_der >= cant){ //esto es, no tiene hijo derecho
        return comp_izq > 0 ?  h_izq : padre;
    } 

    int comp_der = cmp(datos[h_der], datos[padre]);

    if(comp_izq > 0 && comp_der > 0){
        int comp_hijos = cmp(datos[h_izq], datos[h_der]);
        return comp_hijos > 0 ? h_izq : h_der;

    } else if (comp_izq > 0){
        return h_izq;

    } else if (comp_der > 0){
        return h_der;

    }
    return padre;
    
}

void downheap(void* datos[], size_t cant, cmp_func_t cmp, size_t pos_padre){

    if(pos_padre >= cant){
        return;
    }

    size_t pos_hijo_izq = 2 * pos_padre + 1;
    size_t pos_hijo_der = 2 * pos_padre + 2;

    size_t pos_max = maximo(datos, cant, cmp, pos_padre, pos_hijo_izq, pos_hijo_der);

    if(pos_max != pos_padre){
        swap(datos, pos_padre, pos_max);
        downheap(datos, cant, cmp, pos_max);
    }
    return;
}

heap_t *heap_crear(cmp_func_t cmp){

    heap_t* heap = malloc(sizeof(heap_t));

    if (heap == NULL){
        return NULL;
    }

    heap->cmp = cmp;
    heap->cant = 0;
    heap->tam = TAM_INICIAL;

    heap->datos = calloc(heap->tam, sizeof(void*));
   
    if (heap->datos == NULL){
        free(heap);
        return NULL;
    }

    return heap;
}

void heapify(void* datos[], size_t cant, cmp_func_t cmp){

    for(int i= (int) cant-1; i >= 0; i--){ 
        downheap(datos, cant, cmp, (size_t)i);
    }
}

void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){

    heapify(elementos, cant, cmp);
    swap(elementos, 0, cant-1);

    for(size_t i =1 ; i < cant; i++){
        downheap(elementos, cant-i, cmp, 0);
        swap(elementos, 0, cant-i-1);
    }
}

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp){

    heap_t* heap = heap_crear(cmp);

    if (heap == NULL){
        return NULL;
    }
    
    if(n==0){ //si el arreglo estaba vacio, igual devulvo un heap con espacio
        return heap;
    }

    free(heap->datos); // libero la memoria
    heap->cant = n;
    heap->tam = n*2; 
    heap->datos = calloc(heap->tam, sizeof(void*));//pido la memoria que necesito de acuerdo al largo del arreglo

    if (heap->datos == NULL){
        free(heap);
        return NULL;
    }

    for(size_t i = 0; i < heap->cant; i++){ //me copio los datos del arreglo al arreglo del heap
        heap->datos[i] = arreglo[i];
    }

    heapify(heap->datos, heap->cant, heap->cmp);

    return heap;
}

void heap_destruir(heap_t *heap, void destruir_elemento(void *e)){

    for(size_t i = 0; i < heap->cant; i++) { 
        if (destruir_elemento != NULL){
            destruir_elemento(heap->datos[i]);
        }
    }

    free(heap->datos);
    free(heap);

}

size_t heap_cantidad(const heap_t *heap) {

    return heap->cant;
}

bool heap_esta_vacio(const heap_t *heap){

    return heap->cant == 0 ? true : false;
}

bool heap_encolar(heap_t *heap, void *elem) {

    size_t pos_elem = heap->cant;
    heap->datos[pos_elem] = elem;
    heap->cant++;

    if (heap->cant == 1){
        return true;
    }

    upheap(heap->datos, heap->cmp, heap->cant-1);

    if (heap->tam == heap->cant){
        if(!redimensionar_heap(heap, heap->tam * FACTOR_CANT)){ //creo que conviene poner esto primero asi devolvemos false si falla 
            return false;
        }
    }

    return true;
}

void *heap_ver_max(const heap_t *heap) {

    return heap->cant == 0 ? NULL : heap->datos[0];
}

void *heap_desencolar(heap_t *heap){

    if (heap_esta_vacio(heap)){
        return NULL;
    }

    void* valor_a_borrar = heap->datos[0];
    heap->datos[0] = NULL;
    
    
    swap(heap->datos, 0, heap->cant - 1);
    heap->cant--;
    

    downheap(heap->datos, heap->cant, heap->cmp, 0);

    if((heap->cant <= heap->tam/FACTOR_TAM) && (heap->cant/FACTOR_CANT >= TAM_INICIAL)){
    	if(!redimensionar_heap(heap, heap->tam/FACTOR_CANT)){
            printf("fallo la redim\n");
        }
    }

    return valor_a_borrar;
}