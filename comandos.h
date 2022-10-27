#ifndef COMANDOS_H
#define COMANDOS_H

#include <stdbool.h>
#include <stddef.h>
#include "lista.h"
#include "abb.h"


typedef struct algogram algogram_t;

//PRE: Recibe un hash con los nombres 
algogram_t* algogram_crear(lista_t* usuarios);

//PRE: La estructura fue creada.
//POST: Destruye la estructura liberando toda la memoria pedida.
void algogram_destruir(algogram_t* algogram);

//PRE: La estructura fue creada y previamente inicializada.
//POST:Devuelve 0 si el usuario se pudo loggear, -1 si dicho usuario no existia y 1 habia otro usuario loggeado previamente.
//		Funciona en O(1)
int login(algogram_t *algogram, char* usuario);

//PRE: La estructura fue creada y previamente inicializada.
//POST:Devuelve true si el usuario loggeado se desloggeo, y false en caso de que no hubiera un usuario loggeado previamente.
//		Funciona en O(1)
bool logout(algogram_t *algogram);

//PRE: La estructura fue creada y previamente inicializada.
//POST:Devuelve true si se pudo publicar el post, y false en caso de que no hubiera un usuario loggeado previamente.
//		Funciona en O(u log(p))
bool publicar(algogram_t *algogram, char* texto);

//PRE: La estructura fue creada y previamente inicializada.
//POST: Devuelve el ID del proximo post del feed del usuario loggeado actualmente y lo quita del feed.
//		Funciona en O(log(p)).
int ver_proximo_post(algogram_t *algogram);

//PRE: La estructura fue creada y previamente inicializada.
//POST: Devuelve el nombre del usurio que publico el post correspondiente al ID recibido, en caso de recibir un ID invalido,
//		devuelve NULL. Funciona en O(1)
const char* ver_post_usuario(algogram_t *algogram, size_t ID_post);

//PRE: La estructura fue creada y previamente inicializada.
//POST: Devuelve el texto correspondiente al post del ID recibido, en caso de recibir un ID invalido, devuelve NULL.
//		Funciona en O(1)
const char* ver_post_texto(algogram_t *algogram, size_t ID_post);

//PRE: La estructura fue creada y previamente inicializada.
//POST: Devuelve la cantidad de likes del post correspondiente al ID recibido, en caso de recibir un ID invalido
//		devuelve NULL. Funciona en O(1)
int ver_post_cantidad_likes(algogram_t *algogram, size_t ID_post);

//PRE: La estructura fue creada y previamente inicializada.
//POST: Devuelve true si se pudo likear el post correspondiente al ID, y false en caso de que no haya un usurio loggeado
//		previamente o el ID sea invalido. 
//		Funciona en 
bool likear_post(algogram_t *algogram, size_t ID_post);								

//PRE: La estructura fue creada y previamente inicializada.
//POST:Devuelve un arbol binario con orden alfabetico con los nombres de los usuarios que le dieron like al post del ID correspondiente.
//		En caso de que el ID sea invalido o el post no tenga likes, devuelve NULL.
//		Funciona en O(1) ------- porque no estoy pasando eso a una lista y solo devulevo el abb.. estara mal?? 
//	SI, ESTA MAL, BARBARA PODRIA MODIFICAR EL ABB Y ROMPER EL PROGRAMA
abb_t* mostrar_likes(algogram_t *algogram, size_t ID_post);

#endif  // COMANDOS_H
