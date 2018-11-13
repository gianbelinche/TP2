#include "lista.h"
#include <stdbool.h>
#include <stdlib.h>


typedef struct nodo{
	void* dato;
	struct nodo* siguiente;
} nodo_t;


/* Crea un nuevo nodo, al que se le asigna un dato, y se establece su siguiente como NULL, devuelve un puntero al nodo*/
nodo_t* nodo_crear(void* valor){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (!nodo) return NULL;
	nodo->dato = valor;
	nodo->siguiente = NULL;
	return nodo;
}

/* Recibe un nodo, lo destruye y devuelve el valor que se encontraba en este*/
void* nodo_destruir(nodo_t* nodo){
	void* valor = nodo->dato;
	free(nodo);
	return valor;
}



typedef struct lista{
	nodo_t* primero;
	nodo_t* ultimo;
	size_t tam; 
} lista_t;

lista_t *lista_crear(){
	lista_t* lista = malloc(sizeof(lista_t));
	if (!lista) return NULL;
	lista->primero = NULL;
	lista->ultimo = NULL;
	lista->tam = 0;
	return lista;
}

bool lista_esta_vacia(const lista_t *lista){
	return lista->tam==0;
}

bool lista_insertar_primero(lista_t *lista, void *dato){
	nodo_t* nodo = nodo_crear(dato);
	if (nodo==NULL) return false;
	if (lista_esta_vacia(lista))
		lista->ultimo = nodo;
	nodo->siguiente = lista->primero;
	lista->primero = nodo;
	lista->tam += 1;
	return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato){
	nodo_t* nodo = nodo_crear(dato);
	if (nodo==NULL) return false;
	if (lista_esta_vacia(lista))
		lista->primero = nodo;
	else
		lista->ultimo->siguiente = nodo;
	lista->ultimo = nodo;
	lista->tam += 1;
	return true;
}


void *lista_borrar_primero(lista_t *lista){
	if (lista_esta_vacia(lista)) return NULL;
	nodo_t* a_destruir = lista->primero;
	lista->primero = a_destruir->siguiente;
	if (lista->primero==NULL)
		lista->ultimo = NULL;
	lista->tam -= 1;
	return nodo_destruir(a_destruir);

}

void *lista_ver_primero(const lista_t *lista){
	if (lista->primero == NULL) return NULL;
	return lista->primero->dato;
}

void *lista_ver_ultimo(const lista_t* lista){
	if (lista->ultimo == NULL) return NULL;
	return lista->ultimo->dato;
}

size_t lista_largo(const lista_t *lista){
	return lista->tam;
}

void lista_destruir(lista_t *lista, void destruir_dato(void *)){
	while (!lista_esta_vacia(lista)){
		void* dato = lista_borrar_primero(lista);
		if (destruir_dato != NULL)
			destruir_dato(dato);
	}
	free(lista);
}


void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){
	nodo_t* actual = lista->primero;
	while(actual){
		void* dato = actual->dato;
		bool seguir = visitar(dato,extra);
		if (!seguir) break;
		actual = actual->siguiente;
	}
}

typedef struct lista_iter{
	nodo_t* actual;
	nodo_t* anterior;
	lista_t* lista;
} lista_iter_t;

lista_iter_t *lista_iter_crear(lista_t *lista){
	lista_iter_t* iter = malloc(sizeof(lista_iter_t));
	if (!iter) return NULL;
	iter->actual = lista->primero;
	iter->anterior = NULL;
	iter->lista = lista;
	return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter){
	if (iter->actual == NULL) return false;
	iter->anterior = iter->actual;
	iter->actual = iter->actual->siguiente;
	return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter){
	if (iter->actual == NULL) return NULL;
	return iter->actual->dato;
}

bool lista_iter_al_final(const lista_iter_t *iter){
	return iter->actual == NULL;
}

void lista_iter_destruir(lista_iter_t *iter){
	free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato){
	nodo_t* a_insertar = nodo_crear(dato);
	if (a_insertar == NULL) return false;
	a_insertar->siguiente = iter->actual;
	if (iter->anterior == NULL)
		iter->lista->primero = a_insertar;
	else
		iter->anterior->siguiente = a_insertar;
	if (iter->actual == NULL)
		iter->lista->ultimo = a_insertar;
	iter->actual = a_insertar;
	iter->lista->tam += 1;
	return true;
}

void *lista_iter_borrar(lista_iter_t *iter){
	if (iter->actual == NULL) return NULL;
	void* dato = iter->actual->dato;
	if (iter->anterior == NULL)
		iter->lista->primero = iter->actual->siguiente;
	else
		iter->anterior->siguiente = iter->actual->siguiente;
	if (iter->actual == iter->lista->ultimo)
		iter->lista->ultimo = iter->anterior;
	nodo_t* a_destruir = iter->actual;
	iter->actual = a_destruir->siguiente;
	iter->lista->tam -= 1;
	nodo_destruir(a_destruir);
	return dato;
}

