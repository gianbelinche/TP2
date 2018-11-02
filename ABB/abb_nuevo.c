#define _GNU_SOURCE
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "pila.h"
#include "abb.h"


// -_-_-_-_-_-_-_-_  DEFINICION DE  TIPOS DE DATO  _-_-_-_-_-_-_-_-_- //

typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);

typedef struct nodo {
    char* clave;
    void* dato;
    struct nodo* izq;
    struct nodo* der;
}nodo_t;

struct abb {
    nodo_t* raiz;
    size_t cantidad;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir_dato;
};

struct abb_iter{
	const abb_t* arbol;
	pila_t* pila;
};

// -_-_-_-_-_-_-_-_-_-_-  FUNCIONES AUXILIARES  -_-_-_-_-_-_-_-_-_-_- //

nodo_t* abb_crear_nodo(){
	nodo_t* nodo = calloc(1, sizeof(nodo_t));
	if(!nodo) return NULL;

	return nodo;
}

bool abb_raiz_esta_vacia(abb_t* arbol){
	return (!arbol -> raiz || !arbol -> raiz -> clave);
}

nodo_t* _abb_buscar(const nodo_t* arbol,const char* clave,abb_comparar_clave_t cmp){
	if(!arbol || !(arbol -> clave)) return NULL;

	if( cmp(arbol -> clave,clave) == 0)
		return (nodo_t*) arbol;

	if( cmp(arbol -> clave,clave) < 0)
		return _abb_buscar(arbol -> der,clave, cmp);

	return _abb_buscar(arbol -> izq,clave, cmp);
}

nodo_t* abb_buscar (const abb_t* arbol,const char* clave){
	if(abb_raiz_esta_vacia((abb_t*)arbol)) return NULL;

	return _abb_buscar(arbol -> raiz, clave,arbol -> cmp);
}


nodo_t* _abb_obtener_padre(const nodo_t* arbol,const char* clave, abb_comparar_clave_t cmp){
	if(!arbol) return NULL;

	if ( cmp(arbol -> clave,clave) == 0)
		return NULL;

	if(  cmp(arbol -> clave,clave) < 0){
		if(!arbol -> der)
			return (nodo_t*) arbol;

		if(cmp(arbol -> der -> clave, clave) == 0)
			return (nodo_t*) arbol;

		return _abb_obtener_padre(arbol -> der, clave,cmp);
	}

	if(!arbol -> izq)
		return (nodo_t*) arbol;

	if(cmp(arbol -> izq -> clave, clave) == 0)
		return (nodo_t*) arbol;

	return _abb_obtener_padre(arbol -> izq, clave,cmp);
}

nodo_t* abb_obtener_padre(const abb_t* arbol,const char* clave){
	if(abb_raiz_esta_vacia((abb_t*)arbol)) return NULL;

	return _abb_obtener_padre(arbol -> raiz, clave, arbol -> cmp);
}



void abb_borrar_un_hijo(nodo_t* padre,nodo_t* hijo,abb_destruir_dato_t destruir_dato){
	if (!padre){
		free(hijo -> clave);
		if (destruir_dato)
			destruir_dato(hijo -> dato);
		if (hijo -> izq){
			hijo -> clave = hijo -> izq -> clave;
			hijo -> dato = hijo -> izq -> dato;
			nodo_t* izq = hijo -> izq;
			hijo -> izq = izq -> izq;
			hijo -> der = izq -> der;
			if (izq)
				free(izq);
		}
		else{
			hijo -> clave = hijo -> der -> clave;
			hijo -> dato = hijo -> der -> dato;
			nodo_t* der = hijo -> der;
			hijo -> izq = der -> izq;
			hijo -> der = der -> der;
			if (der)
				free(der);
		}
		return;

	}
	else if (padre -> izq == hijo){
		if (hijo -> izq)
			padre -> izq = hijo -> izq;
		else
			padre -> izq = hijo -> der;
	}
	else{
		if (hijo -> izq)
			padre -> der = hijo -> izq;
		else
			padre -> der = hijo -> der;
	}
	free(hijo -> clave);
	if (destruir_dato)
		destruir_dato(hijo -> dato);
	free(hijo);
}

void _abb_destruir(nodo_t* abb, abb_destruir_dato_t destruir_dato);

void abb_borrar_sin_hijos(nodo_t* padre,nodo_t* hijo,abb_destruir_dato_t destruir_dato){
	if (!padre || !padre -> clave){
		free(hijo -> clave);
		hijo -> clave = NULL;
		return;
	}	
	if (padre -> izq == hijo) 
		padre -> izq = NULL;
	else 
		padre -> der = NULL;
	_abb_destruir(hijo, destruir_dato);
}

void abb_borrar_dos_hijos(nodo_t* arbol,abb_t* abb){
	nodo_t* remplazante = arbol -> der;
	while (remplazante -> izq)
		remplazante = remplazante -> izq;
	char* clave = strdup(remplazante -> clave);
	void* dato = abb_borrar(abb,clave);
	free(arbol -> clave);
	arbol -> clave = clave;
	arbol -> dato = dato; 
}




// -_-_-_-_-_-_-_-_-_-_-  PRIMITIVAS DEL ABB  -_-_-_-_-_-_-_-_-_-_- //



abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
	if(!cmp) return NULL;
	
	abb_t* arbol = calloc(1, sizeof(abb_t));
	if(!arbol) return NULL;

	nodo_t* raiz = abb_crear_nodo();
	if(!raiz){
		free(arbol);
		return NULL;
	}

	arbol -> raiz = raiz;
	arbol -> cmp = cmp; 
	arbol -> destruir_dato = destruir_dato;
	return arbol;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
	nodo_t* padre = abb_obtener_padre(arbol,clave);
	nodo_t* hijo;

	// Si es la raíz
	if (!padre){
		hijo = arbol -> raiz;
	}
	else if(!padre -> clave || arbol -> cmp(padre -> clave,clave) == 0){
		hijo = padre;
	}
	else{
		if(arbol -> cmp(padre -> clave,clave) < 0){
			if (!padre -> der)
				padre -> der = abb_crear_nodo();
			hijo = padre -> der;
		}
		else{
			if (!padre -> izq)
				padre -> izq = abb_crear_nodo();
			hijo = padre -> izq;
		}

		if(!hijo) return false;
	}

	if (!hijo -> clave)
		(arbol -> cantidad)++;
	else{
		free(hijo -> clave);
		if (arbol -> destruir_dato)
			arbol -> destruir_dato(hijo -> dato);
	}

	hijo -> clave = strdup(clave);
	hijo -> dato = dato;
	return true;
}

void* abb_obtener(const abb_t* abb,const char* clave){
	nodo_t* arbol = abb_buscar(abb,clave);
	if (!arbol || !arbol -> clave) return NULL;
	return arbol -> dato;
}


bool abb_pertenece(const abb_t* abb,const char* clave){
	nodo_t* arbol = abb_buscar(abb,clave);
	return !(!arbol || !arbol -> clave);
}


size_t abb_cantidad(abb_t *arbol){
	if(!arbol) return 0;
	return arbol -> cantidad;
}


void* abb_borrar(abb_t* abb,const char* clave){
	nodo_t* arbol = abb_buscar(abb,clave);
	if (!arbol || !arbol -> clave) return NULL;
	nodo_t* padre = abb_obtener_padre(abb,clave);
	void* dato = arbol -> dato;
	if (!arbol -> izq && !arbol -> der)
		abb_borrar_sin_hijos(padre,arbol,abb -> destruir_dato);
	else if (arbol -> izq && arbol -> der)
	{
		abb_borrar_dos_hijos(arbol,abb);
		abb -> cantidad++;
	}
	else
		abb_borrar_un_hijo(padre,arbol,abb -> destruir_dato);
	abb -> cantidad --;
	return dato;
}


void _abb_destruir(nodo_t* abb, abb_destruir_dato_t destruir_dato){
	if (!abb) return;
	_abb_destruir(abb -> izq,destruir_dato);
	_abb_destruir(abb -> der,destruir_dato);
	if (abb -> clave)
		free(abb -> clave);

	if (destruir_dato && abb -> dato)
			destruir_dato(abb -> dato);
	free(abb);
}

void abb_destruir(abb_t* abb){
	if (!abb) return;
	_abb_destruir(abb -> raiz,abb -> destruir_dato);
	free(abb);
}



// -_-_-_-_-_-_-_-_-_-_-  ITERADOR DEL ABB  -_-_-_-_-_-_-_-_-_-_- //

bool _abb_in_order(nodo_t *arbol, bool visitar(const char *, void *, void *), void *extra){
	if(!arbol) return true;

	return _abb_in_order(arbol -> izq,visitar,extra) &&
				visitar(arbol -> clave,arbol -> dato, extra) &&
					_abb_in_order(arbol -> der,visitar,extra);
}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra){
	if(!arbol || abb_raiz_esta_vacia(arbol)) return;

	_abb_in_order(arbol  -> raiz,visitar,extra);
}

void apilar_hijos_izq(pila_t* pila, const nodo_t* arbol){
	nodo_t* arbol_izq = arbol -> izq;

	while(arbol_izq){
		pila_apilar(pila,arbol_izq);
		arbol_izq = arbol_izq -> izq;
	}
}

abb_iter_t* abb_iter_in_crear(const abb_t *arbol){
	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
	if(!iter) return NULL;

	iter -> pila  = pila_crear();

	if(!iter -> pila){
		free(iter);
		return NULL;
	}

	iter -> arbol = arbol;

	if(!abb_raiz_esta_vacia((abb_t* )iter -> arbol)){
		pila_apilar(iter -> pila,(abb_t*) iter -> arbol -> raiz);
		apilar_hijos_izq(iter -> pila, iter -> arbol -> raiz);
	}

	return iter;
}

bool abb_iter_in_al_final(const abb_iter_t *iter){
	return pila_esta_vacia(iter -> pila);
}

bool abb_iter_in_avanzar(abb_iter_t *iter){
	if(abb_iter_in_al_final(iter))
		return false;

	nodo_t* desapilado = pila_desapilar(iter -> pila);

	if(desapilado -> der){
		pila_apilar(iter -> pila,desapilado -> der);
		apilar_hijos_izq(iter -> pila,desapilado -> der);
	}

	return true;
}

void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter -> pila);
	free(iter);
}

const char* abb_iter_in_ver_actual(const abb_iter_t *iter){
	nodo_t* arbol = pila_ver_tope(iter -> pila);
	if (!arbol) return NULL;
	return arbol -> clave;
}