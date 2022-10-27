#define _POSIX_C_SOURCE 200809L
#include "comandos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>   

#include "lista.h"
#include "hash.h"
#include "heap.h"
#include "abb.h"

typedef struct afinidad {
    size_t prioridad;
    size_t ID_post;
} afinidad_t;

typedef struct usuario {
    char* nombre; //si no inicializo el tamaño de esto me tira segmentation fault el STRCPY CUANDO CREO LOS USUARIOS
    heap_t* feed;
    size_t pos;
} usuario_t;

typedef struct post {
    char* nombre;
    char* texto;
    size_t ID;
    abb_t* likes;
} post_t;

struct algogram {
    usuario_t* loggeado;
    hash_t* usuarios;
    hash_t* publicaciones;  
};


int comp_afinidad (const void* x, const void* y){
    afinidad_t* afin_1 = (afinidad_t*)x;
    afinidad_t* afin_2 = (afinidad_t*)y;

    size_t prioridad_1 = afin_1->prioridad;
    size_t prioridad_2 = afin_2->prioridad;

    if (prioridad_1 > prioridad_2){
        return -1;
    }
    if (prioridad_1 == prioridad_2){
        if(afin_1->ID_post > afin_2->ID_post){
            return -1;
        }
    }
    return 1;
}

usuario_t* crear_usuario(char* nombre, size_t pos){

    usuario_t* usuario = malloc(sizeof(usuario_t));

    if (usuario == NULL){
        return NULL;
    }

    usuario->nombre = strdup(nombre);
    usuario->pos = pos;
    usuario->feed = heap_crear(comp_afinidad);

    if (usuario->feed == NULL){
        free(usuario);
        return NULL;
    }   

    return usuario;
}

void destruir_usuario(void* dato){
    usuario_t* usuario = dato;
    heap_destruir(usuario->feed, free);
    free(usuario->nombre);
    free(usuario);
}


hash_t* crear_hash_usuarios(lista_t* usuarios){

    hash_t* hash = hash_crear(destruir_usuario);
    if (hash == NULL){
        return NULL;
    }

    size_t pos = 0;
    lista_iter_t* iter = lista_iter_crear(usuarios);
    while (!lista_iter_al_final(iter)){
        
        char* nombre = (char*) lista_iter_ver_actual(iter);
        usuario_t* usuario = crear_usuario(nombre, pos);
        hash_guardar(hash, nombre, usuario);
        pos++;
        lista_iter_avanzar(iter);
    }
    lista_iter_destruir(iter);
    return hash;
}

post_t* crear_post (char* nombre_usuario, char* texto_post, size_t ID_post){

    post_t* post = malloc(sizeof(post_t));

    if (post == NULL){
        return NULL;
    }

    post->nombre = strdup(nombre_usuario);  
    post->texto = strdup(texto_post);
    post->ID = ID_post;
    post->likes = abb_crear(strcmp, NULL);

    if (post->likes == NULL){
        free(post);
        return NULL;
    }

    return post;
}

void destruir_post(void* dato){
    post_t* post = dato;
    abb_destruir(post->likes);
    free(post->nombre);
    free(post->texto);
    free(post);

}

algogram_t* algogram_crear(lista_t* usuarios){

    algogram_t* algogram = malloc(sizeof(algogram_t));

    if (algogram == NULL){
        return NULL;
    }

    algogram->loggeado = NULL;
    algogram->usuarios = crear_hash_usuarios(usuarios);
    if (algogram->usuarios == NULL){
        return NULL;
    }

    algogram->publicaciones = hash_crear(destruir_post);
    if (algogram->publicaciones == NULL){
        return NULL;
    }
   
    return algogram;
}


// -------------------FUNCCIONES LOGIN / LOGOUT (falta logout)----------------------------------

bool hay_alguien_loggeado(algogram_t* algogram){    
    return algogram->loggeado == NULL ? false : true; 
}

bool usuario_no_existe(algogram_t* algogram, char* nombre_usuario){
    return !hash_pertenece(algogram->usuarios, nombre_usuario);
}

int login(algogram_t* algogram , char* nombre_usuario){

    if (hay_alguien_loggeado(algogram)){
        return 1;
    }

    if (usuario_no_existe(algogram, nombre_usuario)){ 
        return -1;     
    }

    algogram->loggeado = hash_obtener(algogram->usuarios, nombre_usuario);
    return 0;
}

bool logout(algogram_t* algogram){
    
    if (!hay_alguien_loggeado(algogram)){  
        return false; 
    }

    algogram->loggeado = NULL;
    return true;
}

// --------------------------------- PUBLICAR POST ---------------------------------

size_t determinar_afinidad(size_t pos_1, size_t pos_2){
    int afinidad = abs((int) pos_1 - (int) pos_2);
    return (size_t)afinidad;
}

bool publicar(algogram_t* algogram, char* texto) {

    if (!hay_alguien_loggeado(algogram)){
        return false;
    }

    usuario_t* usuario_publicador = algogram->loggeado;
    size_t ID_post = hash_cantidad(algogram->publicaciones);
    post_t* post = crear_post(usuario_publicador->nombre, texto, ID_post);

    char clave[144];
    sprintf(clave, "%d", (int)ID_post);

    hash_guardar(algogram->publicaciones, clave, post);        

    hash_iter_t* iter = hash_iter_crear(algogram->usuarios);
    while(!hash_iter_al_final(iter)){// O(u)
        
        const char* usuario_actual = hash_iter_ver_actual(iter);

        if (strcmp(usuario_actual, usuario_publicador->nombre) != 0){ 

            usuario_t* usuario = hash_obtener(algogram->usuarios, usuario_actual);

            afinidad_t* afinidad = malloc(sizeof(afinidad_t));
            afinidad->prioridad = determinar_afinidad(usuario_publicador->pos, usuario->pos);
            afinidad->ID_post = ID_post; 

            heap_encolar(usuario->feed, afinidad); //O(log(p))
        }
        
        hash_iter_avanzar(iter);
    }

    hash_iter_destruir(iter);
    return true;
} 

int ver_proximo_post(algogram_t* algogram){ 
    if(!hay_alguien_loggeado(algogram)){
        return -1;
    }

    usuario_t* usuario = hash_obtener(algogram->usuarios, algogram->loggeado->nombre);
    heap_t* feed_actual = usuario->feed;

    if(heap_esta_vacio(feed_actual)){
        return -1;
    }

    afinidad_t* afinidad = heap_desencolar(feed_actual);

    char clave[144];
    sprintf(clave, "%d", (int)afinidad->ID_post);

    post_t* proximo_post = hash_obtener(algogram->publicaciones, clave);
    size_t ID_proximo_post = proximo_post->ID;

    free(afinidad);
    return (int)ID_proximo_post;
}

const char* ver_post_usuario(algogram_t *algogram, size_t ID_post){
    char clave[144];
    sprintf(clave, "%d", (int)ID_post);

    if(!hash_pertenece(algogram->publicaciones, clave)){
        return NULL;
    }

    post_t* post = hash_obtener(algogram->publicaciones, clave);

    return post->nombre;
}

const char* ver_post_texto(algogram_t *algogram, size_t ID_post){
    char clave[144];
    sprintf(clave, "%d", (int)ID_post);

    if(!hash_pertenece(algogram->publicaciones, clave)){
        return NULL;
    }

    post_t* post = hash_obtener(algogram->publicaciones, clave);

    return post->texto;
}

int ver_post_cantidad_likes(algogram_t *algogram, size_t ID_post){
    char clave[144];
    sprintf(clave, "%d", (int)ID_post);

    if(!hash_pertenece(algogram->publicaciones, clave)){
        return -1;
    }

    post_t* post = hash_obtener(algogram->publicaciones, clave);
    size_t cant = abb_cantidad(post->likes);

    return (int)cant;
}

bool likear_post(algogram_t *algogram, size_t ID_post){

    if(!hay_alguien_loggeado(algogram)){
        return false;
    }

    char clave[144];
    sprintf(clave, "%d", (int)ID_post);

    if(!hash_pertenece(algogram->publicaciones, clave)){
        return false;
    }

    post_t* post = hash_obtener(algogram->publicaciones, clave);

    if(abb_pertenece(post->likes,  algogram->loggeado->nombre)){//si el usuario ya habia likeado el post
        return true;
    }

    abb_guardar(post->likes, algogram->loggeado->nombre, NULL); 
    return true;
}

abb_t* mostrar_likes(algogram_t *algogram, size_t ID_post){
    char clave[144];
    sprintf(clave, "%d", (int)ID_post);

    if(!hash_pertenece(algogram->publicaciones, clave)){
        return NULL;
    }

    post_t* post = hash_obtener(algogram->publicaciones, clave);
    abb_t* likes = post->likes;

    if(abb_cantidad(likes) == 0){
        return NULL;
    }

    return likes;
}

void algogram_destruir(algogram_t* algogram){

    hash_destruir(algogram->usuarios);
    hash_destruir(algogram->publicaciones);
    free(algogram);

}