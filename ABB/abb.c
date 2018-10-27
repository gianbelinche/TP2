//Este va a ser el abb
//Bueno
#define _GNU_SOURCE
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

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

/* Mis sueños y esperanzas rotos
abb_t* _abb_buscar(const abb_t* arbol,const char* clave,int* vinculo)
{
	int comparacion;
	if(!arbol || !(arbol -> clave)) return (abb_t*) arbol;

	comparacion = arbol -> cmp(arbol -> clave,clave);

	if (comparacion == 0) 
		return (abb_t*) arbol;
	
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
	return _abb_buscar(arbol,clave,vinculo);
}

abb_t* abb_obtener_padre(abb_t* arbol,int vinculo)
{
	return arbol - vinculo;
}

*/
abb_t* abb_obtener_hijo(const abb_t* arbol,const char* clave)
{
	if(!arbol) return NULL;

	int comparacion = arbol -> cmp(arbol -> clave,clave);

	if (comparacion == 0) 
		return (abb_t*) arbol;
	
	if (comparacion < 0)
		return arbol -> der;

	return arbol -> izq;
}

abb_t* _abb_buscar(const abb_t* arbol,const char* clave,bool busco_padre)
{
	if(!arbol || !(arbol -> clave)) return (abb_t*) arbol; //poner NULL directamente, sino falla en el caso no clave
	abb_t* siguiente = abb_obtener_hijo(arbol,clave);
	
	if(busco_padre)
	{	
		if (siguiente == arbol)
			return NULL;
		if (!siguiente)
			return (abb_t*) arbol;
		if( !arbol->cmp(siguiente->clave,arbol -> izq-> clave) ||  !arbol->cmp(siguiente->clave,arbol -> der ->clave)) //Y si ninguna de las claves coincide?
			return (abb_t*) arbol;
		return _abb_buscar(siguiente,clave,busco_padre);
	}
	else
	{
		if(siguiente == arbol)
			return (abb_t*) arbol;
	}

	return _abb_buscar(siguiente,clave,busco_padre);
}

abb_t* abb_buscar(const abb_t* arbol,const char* clave)
{
	return _abb_buscar(arbol,clave,false);
}

abb_t* abb_obtener_padre(const abb_t* arbol,const char* clave)
{
	return _abb_buscar(arbol,clave,true);
}



void abb_borrar_sin_hijos(abb_t* padre,abb_t* hijo){
	if (!padre || !padre->clave){
		free(hijo->clave);
		hijo->clave = NULL;
		return;
	}	
	if (padre->izq == hijo) 
		padre->izq = NULL;
	else 
		padre->der = NULL;
	abb_destruir(hijo);
}

void abb_borrar_un_hijo(abb_t* padre,abb_t* hijo){
	if (!padre){
		free(hijo->clave);
		if (hijo->destruir_dato)
			hijo->destruir_dato(hijo->dato);
		if (hijo->izq){
			hijo->clave = hijo->izq->clave;
			hijo->dato = hijo->izq->dato;
			hijo->izq = NULL;
			abb_destruir(hijo->izq);
		}
		else{
			hijo->clave = hijo->der->clave;
			hijo->dato = hijo->der->dato;
			hijo->der = NULL;
			abb_destruir(hijo->der);
		}
		return;

	}
	else if (padre->izq == hijo){
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
	abb_t* padre = abb_obtener_padre(arbol,clave);
	abb_t* hijo;

	// Si es la raíz
	if (!padre)
		hijo = arbol;
	else if(!padre -> clave || padre -> cmp(padre -> clave,clave) == 0)
	{
		hijo = padre;
	}
	else
	{
		if(padre -> cmp(padre -> clave,clave) < 0)
		{
			if (!padre->der)
				padre -> der = abb_crear(padre -> cmp,padre -> destruir_dato);
			hijo = padre -> der;
		}
		else
		{
			if (!padre->izq)
				padre -> izq = abb_crear(padre -> cmp,padre -> destruir_dato);
			hijo = padre -> izq;
		}

		//hijo = abb_crear(padre -> cmp,padre -> destruir_dato);
		if(!hijo) return false;
	}

	if (!hijo->clave)
		(arbol -> cantidad)++;
	else{
		free(hijo->clave);
		if (arbol->destruir_dato)
			arbol->destruir_dato(hijo->dato);
	}
	hijo -> clave = strdup(clave);
	hijo -> dato = dato;
	return true;
}

bool abb_pertenece(const abb_t* abb,const char* clave){
	abb_t* arbol = abb_buscar(abb,clave);
	return arbol!=NULL && arbol->clave!=NULL;
}

void* abb_obtener(const abb_t* abb,const char* clave){
	abb_t* arbol = abb_buscar(abb,clave);
	if (!arbol || !arbol->clave) return NULL;
	return arbol->dato;
}

size_t abb_cantidad(abb_t *arbol)
{
	if(!arbol) return 0;
	return arbol -> cantidad;//return abb_cantidad(arbol -> izq) + abb_cantidad(arbol -> der) + 1;
}


void* abb_borrar(abb_t* abb,const char* clave){
	abb_t* arbol = abb_buscar(abb,clave);
	if (!arbol || !arbol->clave) return NULL;
	void* dato = arbol->dato;
	abb_t* padre = abb_obtener_padre(abb,clave);
	if (!arbol->izq && !arbol->der)
		abb_borrar_sin_hijos(padre,arbol);
	else if (arbol->izq || arbol->der)
		abb_borrar_un_hijo(padre,arbol);
	else
		abb_borrar_dos_hijos(arbol);
	abb->cantidad --;
	return dato;
}

void abb_destruir(abb_t* abb){
	if (!abb) return;
	abb_destruir(abb->izq);
	abb_destruir(abb->der);
	if (abb->clave)
		free(abb->clave);
	if (abb->destruir_dato && abb->dato)
		abb->destruir_dato(abb->dato);
	free(abb);
}

// -_-_-_-_-_-_-_-_-_-_-  ITERADOR DEL ABB  -_-_-_-_-_-_-_-_-_-_- //
