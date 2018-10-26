//Este va a ser el abb
//Bueno

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#include "abb.h"
// -_-_-_-_-_-_-_-_-_-_-_      CONSTANTES     -_-_-_-_-_-_-_-_-_-_-_- //
#define RAIZ 0
#define IZQ  0
#define DER  1

// -_-_-_-_-_-_-_-_  DEFINICION DE  TIPOS DE DATO  _-_-_-_-_-_-_-_-_- //


typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);

typedef struct abb{
	struct abb* izq;
	struct abb* der;
	char* clave;
	void* dato;
	size_t cantidad;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir_dato;
}abb_t;

// -_-_-_-_-_-_-_-_-_-_-  FUNCIONES AUXILIARES  -_-_-_-_-_-_-_-_-_-_- //

abb_t* _abb_buscar(abb_t* arbol,const char* clave,int* vinculo)
{
	if(!arbol) return arbol;

	int comparacion = arbol -> cmp(arbol -> clave,clave);

	if (comparacion == 0) 
		return arbol;
	
	if (comparacion > 0)
	{
		*vinculo = DER;
		return _abb_buscar(arbol -> der,clave,vinculo);
	}

	*vinculo = IZQ;
	return _abb_buscar(arbol -> izq,clave,vinculo);
}

abb_t* abb_buscar(const abb_t* arbol,const char* clave,int* vinculo)
{
	*vinculo = RAIZ;
	return _abb_buscar(arbol -> izq,clave,vinculo);
}

size_t abb_cantidad(abb_t *arbol)
{
	return arbol -> cantidad;
}

abb_t* abb_obtener_padre(abb_t* arbol,int vinculo)
{
	return arbol - vinculo;
}

void abb_borrar_sin_hijos(abb_t* padre,abb_t* hijo){
	if (padre->izq == hijo) 
		padre->izq = NULL;
	else 
		padre->der = NULL;
	abb_destruir(hijo);
}

void abb_borrar_un_hijo(abb_t* padre,abb_t* hijo){
	if (padre->izq == hijo){
		if (hijo->izq)
			padre->izq = hijo->izq;
		else
			padre->izq = hijo->der;
	}
	else{
		if (hijo->izq)
			padre->der = hijo->izq;
		else
			padre->der = hijo->der;
	}
	abb_destruir(hijo);
}

void abb_borrar_dos_hijos(abb_t* arbol){
	abb_t* remplazante = arbol->der;
	while (remplazante->izq)
		remplazante = remplazante->izq;
	char* clave = remplazante->clave;
	void* dato = abb_borrar(arbol,clave);
	arbol->clave = clave;
	arbol->dato = dato; 
}

// -_-_-_-_-_-_-_-_-_-_-  PRIMITIVAS DEL ABB  -_-_-_-_-_-_-_-_-_-_- //

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato)
{
	abb_t* arbol = calloc(1, sizeof(abb_t));
	if(!arbol) return NULL;

	arbol -> cmp = cmp;
	arbol -> destruir_dato = destruir_dato;
	return arbol;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato)
{
	int vinculo;
	abb_t* hijo = abb_buscar(arbol,clave,&vinculo);

	if(!hijo)
	{
		hijo = abb_crear(arbol -> cmp,arbol -> destruir_dato);
		if(!hijo) return false;
	}
	
	hijo -> dato = dato;
	return true;
}

bool abb_pertenece(const abb_t* abb,const char* clave){
	int vinculo;
	abb_t* arbol = abb_buscar(abb,clave,&vinculo);
	return arbol;
}

void* abb_obtener(const abb_t* abb,const char* clave){
	int vinculo;
	abb_t* arbol = abb_buscar(abb,clave,&vinculo);
	if (!arbol) return NULL;
	return arbol->dato;
}


void* abb_borrar(abb_t* abb,const char* clave){
	int vinculo;
	abb_t* arbol = abb_buscar(abb,clave,&vinculo);
	if (!arbol) return NULL;
	void* dato = arbol->dato;
	abb_t* padre = abb_obtener_padre(abb,vinculo);
	if (!arbol->izq && !arbol->der)
		abb_borrar_sin_hijos(padre,arbol);
	else if (arbol->izq || arbol->der)
		abb_borrar_un_hijo(padre,arbol);
	else
		abb_borrar_dos_hijos(arbol);
	return dato;
}

void abb_destruir(abb_t* abb){
	if (!abb) return;
	abb_destruir(abb->izq);
	abb_destruir(abb->der);
	free(abb->clave);
	if (abb->destruir_dato)
		abb->destruir_dato(abb->dato);
	free(abb);
}

// -_-_-_-_-_-_-_-_-_-_-  ITERADOR DEL ABB  -_-_-_-_-_-_-_-_-_-_- //
