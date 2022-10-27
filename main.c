#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comandos.h"
#include "lista.h"


lista_t* crear_lista_usuarios(char* nombre_archivo){

	FILE *mi_archivo = fopen(nombre_archivo,"r");

	if(mi_archivo == NULL) {
		fprintf(stdout, "%s","ERROR\n");
		return NULL;
	}

	lista_t* lista = lista_crear();

	if (lista == NULL){
		return NULL;
	}
	char* linea = NULL;
	size_t tam = 0;
	char* nombre = NULL;
	while ((getline(&linea, &tam, mi_archivo)) != EOF){
		nombre = strdup(linea);
		lista_insertar_ultimo(lista, nombre);
	}

	free(linea);
	fclose(mi_archivo);
	
	return lista;
}

void mostrar_login(algogram_t* algogram){
	char* nombre = NULL;
	size_t tam = 0;

	if(getline(&nombre, &tam, stdin) == EOF){
		return;
	}

	int result = login(algogram, nombre);

	if(result == 0){
		fprintf(stdout, "Hola %s", nombre);
	} else if(result == 1){
		fprintf(stdout, "%s\n","Error: Ya habia un usuario loggeado");
	} else{
		fprintf(stdout, "%s\n","Error: usuario no existente");
	}

	free(nombre);
}

void mostrar_logout(algogram_t* algogram){
	if(logout(algogram)){
		fprintf(stdout, "%s\n", "Adios");
	} else {
		fprintf(stdout, "%s\n","Error: no habia usuario loggeado"); 
	}	
}

void mostrar_publicar(algogram_t* algogram){
	char* texto = NULL;
	size_t tam = 0;

	if(getline(&texto, &tam, stdin) == EOF){
		return;
	}

	if(publicar(algogram, texto)){
		fprintf(stdout, "Post publicado\n");
	} else {
		fprintf(stdout, "%s\n","Error: no habia usuario loggeado");
	}

	free(texto);
}

void mostrar_ver_siguiente_feed(algogram_t* algogram){
	int ID_post = ver_proximo_post(algogram);
	if(ID_post != -1) {
		char* nombre = strdup(ver_post_usuario(algogram, (size_t) ID_post));//hago esto porque no puedo modificar el nombre del usurio y necesito imprimirlo sin el \n
		size_t pos = strlen(nombre);
		if(nombre[pos-1] == '\n'){
			nombre[pos-1] = '\0';
		}
		fprintf(stdout, "Post ID %i\n", ID_post);
		fprintf(stdout, "%s dijo: %s", nombre , ver_post_texto(algogram, (size_t) ID_post));
		fprintf(stdout, "Likes: %i\n",  ver_post_cantidad_likes(algogram, (size_t) ID_post));
		free(nombre);
	} else {
		fprintf(stdout, "%s\n", "Usuario no loggeado o no hay mas posts para ver");
	}
	
}

void mostrar_likear_post(algogram_t* algogram){
	char* ID = NULL;
	size_t tam = 0;

	if(getline(&ID, &tam, stdin) == EOF){
		return;
	}

	int ID_post = atoi(ID);
	if(likear_post(algogram, (size_t) ID_post)){  
		fprintf(stdout, "Post likeado\n");
	} else {
		fprintf(stdout, "%s\n","Error: Usuario no loggeado o Post inexistente");
	}
	free(ID);
}

bool imprimir_nombres(const char* nombre, void* dato, void* extra){
	fprintf(stdout, "	%s", nombre);
	return true;
}

void mostrar_likes_post(algogram_t* algogram){
	char* ID = NULL;
	size_t tam = 0;

	if(getline(&ID, &tam, stdin) == EOF){
		return;
	}

	int ID_post = atoi(ID);
	abb_t* likes = mostrar_likes(algogram, (size_t)ID_post);
			
	if(likes != NULL){
		fprintf(stdout, "El post tiene %i likes:\n", ver_post_cantidad_likes(algogram, (size_t) ID_post));
		int extra = 0;
		abb_in_order(likes, imprimir_nombres, &extra);
	} else {
		fprintf(stdout, "%s\n","Error: Post inexistente o sin likes");
	}
	free(ID);
}


//------------------------MAIN-----------------------------------------

int main(int argc, char* argv[]){

	lista_t* usuarios = crear_lista_usuarios(argv[1]);
	algogram_t* algogram = algogram_crear(usuarios);
	lista_destruir(usuarios, free);

	char* comando = NULL;
	size_t tam = 0;

	while(getline(&comando, &tam, stdin) != EOF){

		if(strcmp(comando, "login\n") == 0){
			mostrar_login(algogram);

		} else if (strcmp(comando, "logout\n") == 0){
			mostrar_logout(algogram);

		} else if (strcmp(comando, "publicar\n") == 0){
			mostrar_publicar(algogram);

		} else if(strcmp(comando, "ver_siguiente_feed\n") == 0){
			mostrar_ver_siguiente_feed(algogram);

		} else if(strcmp(comando, "likear_post\n") == 0){
			mostrar_likear_post(algogram);
			
		} else if(strcmp(comando, "mostrar_likes\n") == 0){
			mostrar_likes_post(algogram);
			
		} else {
			fprintf(stdout, "%s\n","Error: comando inexistente");
		}
	}

	free(comando);
	algogram_destruir(algogram);

	return 0;
}

