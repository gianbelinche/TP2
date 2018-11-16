#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lista.h"

// -_-_-_-_-_-_-_-  DEFINICION DE LOS TIPOS DE DATO  -_-_-_-_-_-_-_-_- //

typedef struct nodo_lista{
	struct nodo_lista* nodo_siguiente;
	void*  dato;

}nodo_lista_t;

struct lista
{
	size_t cantidad;
	nodo_lista_t* primer_nodo;
	nodo_lista_t* ultimo_nodo;

};

struct lista_iter
{
	lista_t* lista;
	nodo_lista_t* nodo_anterior;
	nodo_lista_t* nodo_actual;

};

// -_-_-_-_-_-_-_-_-_-_-  FUNCIONES AUXILIARES  -_-_-_-_-_-_-_-_-_-_- //

nodo_lista_t* nodo_crear(void* dato)
{
	nodo_lista_t* nodo = malloc(sizeof(nodo_lista_t));

	if(!nodo)
		return NULL;

	nodo -> nodo_siguiente = NULL;
	nodo -> dato = dato;

	return  nodo;
}

void nodo_destruir(nodo_lista_t* nodo)
{
	free(nodo);
}


// -_-_-_-_-_-_-_-_-_-_-  PRIMITIVAS DE LA LISTA  -_-_-_-_-_-_-_-_-_-_- //

lista_t* lista_crear(void)
{
	lista_t* lista = malloc(sizeof(lista_t));

	if(!lista)
		return NULL;

	lista -> cantidad = 0;
	lista -> primer_nodo = NULL;
	lista -> ultimo_nodo = NULL;

	return lista;
}

bool lista_esta_vacia(const lista_t* lista)
{
	return (!lista -> cantidad && !lista -> primer_nodo && !lista -> ultimo_nodo);
}

bool lista_insertar_primero(lista_t* lista, void* dato)
{
	nodo_lista_t* nodo = nodo_crear(dato);

	if(!nodo)
		return false;

	if (lista_esta_vacia(lista))
		lista -> ultimo_nodo = nodo;
	else 
		nodo -> nodo_siguiente = lista -> primer_nodo;
	
	lista -> primer_nodo = nodo;
	(lista -> cantidad)++;

	return true;

}


bool lista_insertar_ultimo(lista_t* lista, void* dato)
{
	nodo_lista_t* nodo = nodo_crear(dato);

	if(!nodo)
		return false;

	if (lista_esta_vacia(lista))
		lista -> primer_nodo = nodo;
	else 
		lista -> ultimo_nodo -> nodo_siguiente = nodo;
	
	lista -> ultimo_nodo = nodo;
	(lista -> cantidad)++;

	return true;
}

void* lista_borrar_primero(lista_t* lista)
{
	if(lista_esta_vacia(lista))
		return NULL;
	
	nodo_lista_t* nodo_a_desenlistar = lista -> primer_nodo;
	void*		  dato = nodo_a_desenlistar -> dato;

	lista -> primer_nodo = nodo_a_desenlistar -> nodo_siguiente;

	if(lista -> ultimo_nodo == nodo_a_desenlistar)
		lista -> ultimo_nodo = NULL;

	nodo_destruir(nodo_a_desenlistar);
	(lista -> cantidad)--;

	return dato;
}

void *lista_ver_primero(const lista_t *lista)
{
	if(lista_esta_vacia(lista))
		return NULL;

	return lista -> primer_nodo -> dato;
}

void *lista_ver_ultimo(const lista_t* lista)
{
	if(lista_esta_vacia(lista))
		return NULL;

	return lista -> ultimo_nodo -> dato;
}

size_t lista_largo(const lista_t *lista)
{
	return lista -> cantidad;
}

void lista_destruir(lista_t *lista, void destruir_dato(void *))
{
	while(!lista_esta_vacia(lista))
	{
		if(!destruir_dato)	 lista_borrar_primero(lista);
		else	destruir_dato(lista_borrar_primero(lista));	
	}

	free(lista);
}

// -_-_-_-_-_-_-_-_-_-_-  ITERADORES DE LA LISTA  -_-_-_-_-_-_-_-_-_-_- //

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra)
{
	nodo_lista_t* nodo_actual = lista -> primer_nodo;

	while(nodo_actual && visitar(nodo_actual -> dato,extra))
		nodo_actual = nodo_actual -> nodo_siguiente;
}

lista_iter_t* lista_iter_crear(lista_t* lista)
{
	lista_iter_t* iter = malloc(sizeof(lista_iter_t));

	if(!iter)
		return NULL;

	iter -> lista = lista;
	iter -> nodo_actual   = lista -> primer_nodo;
	iter -> nodo_anterior = NULL;

	return iter;
}

bool lista_iter_al_final(const lista_iter_t *iter)
{
	if(!(iter -> nodo_actual))
		return true;

	return false;
}

bool lista_iter_avanzar(lista_iter_t* iter)
{
	if(lista_iter_al_final(iter))
		return false;

	iter -> nodo_anterior = iter -> nodo_actual;
	iter -> nodo_actual   = iter -> nodo_anterior -> nodo_siguiente;
	return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter)
{
	if(lista_iter_al_final(iter))
		return NULL;

	return iter -> nodo_actual -> dato;
}



bool lista_iter_insertar(lista_iter_t* iter, void *dato)
{
	nodo_lista_t* nodo = nodo_crear(dato);

	if(!nodo)
		return false;

	if(!(iter -> nodo_anterior))
		iter -> lista -> primer_nodo = nodo;
	else
		iter -> nodo_anterior -> nodo_siguiente = nodo;

	if(lista_iter_al_final(iter))
		iter -> lista -> ultimo_nodo = nodo;

	nodo -> nodo_siguiente = iter -> nodo_actual;
	(iter -> lista -> cantidad)++;
	iter -> nodo_actual = nodo;
	return true;
}

void *lista_iter_borrar(lista_iter_t* iter)
{
	if(lista_iter_al_final(iter))
		return NULL;

	void* dato = iter -> nodo_actual -> dato;

	if(!(iter -> nodo_anterior))
		iter -> lista -> primer_nodo = iter -> nodo_actual -> nodo_siguiente;
	else
		iter -> nodo_anterior -> nodo_siguiente = iter -> nodo_actual -> nodo_siguiente;

	if (!(iter -> nodo_actual -> nodo_siguiente))
		iter -> lista -> ultimo_nodo = iter -> nodo_anterior;

	nodo_destruir(iter -> nodo_actual);

	if(iter -> nodo_anterior)
		iter -> nodo_actual = iter -> nodo_anterior -> nodo_siguiente;
	else
		iter -> nodo_actual = iter -> lista -> primer_nodo;

	(iter -> lista -> cantidad)--;
	return dato;
	
}

void lista_iter_destruir(lista_iter_t* iter)
{
	free(iter);
}