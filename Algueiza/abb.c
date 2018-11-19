#define _GNU_SOURCE
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "pila.h"
#include "abb.h"

// -_-_-_-_-_-_-_-_  DEFINICION DE  TIPOS DE DATO  _-_-_-_-_-_-_-_-_- //

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
	const abb_t* abb;
	pila_t* pila;
};


// -_-_-_-_-_-_-_-_-_-_-  FUNCIONES AUXILIARES  -_-_-_-_-_-_-_-_-_-_- //

nodo_t* abb_crear_nodo(const char* clave, void* dato){
	nodo_t* nodo = calloc(1, sizeof(nodo_t));
	if(!nodo) return NULL;

	nodo -> clave = strdup(clave);
	nodo -> dato = dato;
	nodo -> izq = NULL;
	nodo -> der = NULL;

	return nodo;
}

bool abb_raiz_esta_vacia(abb_t* abb){
	return (!abb -> raiz);
}

nodo_t* _abb_buscar(const nodo_t* nodo,const char* clave,abb_comparar_clave_t cmp,pila_t* pila){
	if(!nodo) return NULL;

	if (pila)
		pila_apilar(pila,(void*) nodo);

	if( cmp(nodo -> clave,clave) == 0){
		return (nodo_t*) nodo;
	}

	if( cmp(nodo -> clave,clave) < 0){
		return _abb_buscar(nodo -> der,clave, cmp,pila);
	}

	return _abb_buscar(nodo -> izq,clave, cmp,pila);
}

nodo_t* abb_buscar (const abb_t* abb,const char* clave,pila_t* pila){
	if(abb_raiz_esta_vacia((abb_t*)abb)) return NULL;

	return _abb_buscar(abb -> raiz, clave,abb -> cmp,pila);
}


nodo_t* _abb_obtener_padre(const nodo_t* nodo,const char* clave, abb_comparar_clave_t cmp){
	if(!nodo) return NULL;

	if ( cmp(nodo -> clave,clave) == 0)
		return NULL;

	if(  cmp(nodo -> clave,clave) < 0){
		if(!nodo -> der)
			return (nodo_t*) nodo;

		if(cmp(nodo -> der -> clave, clave) == 0)
			return (nodo_t*) nodo;

		return _abb_obtener_padre(nodo -> der, clave,cmp);
	}

	if(!nodo -> izq)
		return (nodo_t*) nodo;

	if(cmp(nodo -> izq -> clave, clave) == 0)
		return (nodo_t*) nodo;

	return _abb_obtener_padre(nodo -> izq, clave,cmp);
}

nodo_t* abb_obtener_padre(const abb_t* abb,const char* clave){
	if(abb_raiz_esta_vacia((abb_t*)abb)) return NULL;

	return _abb_obtener_padre(abb -> raiz, clave, abb -> cmp);
}


// -_-_-_-_-_-_-_-_-_-_-  PRIMITIVAS DEL ABB  -_-_-_-_-_-_-_-_-_-_- //


abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
	if(!cmp) return NULL;
	
	abb_t* abb = calloc(1, sizeof(abb_t));
	if(!abb) return NULL;

	abb -> raiz = NULL;
	abb -> cmp = cmp; 
	abb -> destruir_dato = destruir_dato;
	return abb;
}

bool abb_guardar(abb_t* abb, const char *clave, void *dato){
	nodo_t* padre = abb_obtener_padre(abb,clave);
	nodo_t* hijo;
	bool esta_repetido = true;

	// Si es la raíz
	if (!padre){
		hijo = abb->raiz;
		if (!abb->raiz){
			esta_repetido = false;
			hijo = abb_crear_nodo(clave,dato);
		}
		abb -> raiz = hijo;	
	}
	else if(abb -> cmp(padre -> clave,clave) == 0){
		hijo = padre;
		esta_repetido = false;
	}
	else{
		if(abb -> cmp(padre -> clave,clave) < 0){
			if (!padre -> der){
				padre -> der = abb_crear_nodo(clave,dato);
				esta_repetido = false;
			}
			hijo = padre -> der;
		}
		else{
			if (!padre -> izq){
				esta_repetido = false;
				padre -> izq = abb_crear_nodo(clave,dato);
			}
			hijo = padre -> izq;
		}
	}

	if(!hijo) return false;

	if((padre && abb -> cmp(padre -> clave,clave) == 0) || esta_repetido){
		if (abb -> destruir_dato)
			abb -> destruir_dato(hijo -> dato);
		hijo -> dato = dato;
	}	
	else{
		(abb -> cantidad)++;
	}

	return true;
}

void* abb_obtener(const abb_t* abb,const char* clave){
	nodo_t* nodo = abb_buscar(abb,clave,NULL);
	if (!nodo) return NULL;
	return nodo -> dato;
}


bool abb_pertenece(const abb_t* abb,const char* clave){
	nodo_t* nodo = abb_buscar(abb,clave,NULL);
	return !(!nodo);
}


size_t abb_cantidad(abb_t *abb){
	if(!abb) return 0;
	return abb -> cantidad;
}

void _abb_destruir(nodo_t* nodo, abb_destruir_dato_t destruir_dato){
	if (!nodo) return;
	_abb_destruir(nodo -> izq,destruir_dato);
	_abb_destruir(nodo -> der,destruir_dato);

	free(nodo -> clave);

	if (destruir_dato && nodo -> dato)
			destruir_dato(nodo -> dato);
	free(nodo);
}

void abb_destruir(abb_t* abb){
	if (!abb) return;
	_abb_destruir(abb -> raiz,abb -> destruir_dato);
	free(abb);
}

void abb_borrar_un_hijo(nodo_t* padre,nodo_t* hijo,abb_destruir_dato_t destruir_dato,abb_t* abb){
	if (!padre){
		free(hijo -> clave);
		if (destruir_dato)
			destruir_dato(hijo -> dato);
		if (hijo -> izq){
			abb->raiz = hijo->izq;
		}
		else{
			abb->raiz = hijo->der;
		}
		free(hijo);
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

void abb_borrar_sin_hijos(nodo_t* padre,nodo_t* hijo,abb_destruir_dato_t destruir_dato,abb_t* abb){
	if (!padre){
		_abb_destruir(hijo,destruir_dato);
		abb->raiz = NULL;
		return;
	}	
	if (padre -> izq == hijo) 
		padre -> izq = NULL;
	else 
		padre -> der = NULL;
	_abb_destruir(hijo, destruir_dato);
}

void abb_borrar_dos_hijos(nodo_t* nodo,abb_t* abb){
	nodo_t* remplazante = nodo -> der;
	while (remplazante -> izq)
		remplazante = remplazante -> izq;
	char* clave = strdup(remplazante -> clave);
	void* dato = abb_borrar(abb,clave);
	free(nodo -> clave);
	nodo -> clave = clave;
	nodo -> dato = dato; 
}

void* abb_borrar(abb_t* abb,const char* clave){
	nodo_t* nodo = abb_buscar(abb,clave,NULL);
	if (!nodo) return NULL;
	nodo_t* padre = abb_obtener_padre(abb,clave);
	void* dato = nodo -> dato;
	if (!nodo -> izq && !nodo -> der)
		abb_borrar_sin_hijos(padre,nodo,abb -> destruir_dato,abb);
	else if (nodo -> izq && nodo -> der)
	{
		abb_borrar_dos_hijos(nodo,abb);
		abb -> cantidad++;
	}
	else
		abb_borrar_un_hijo(padre,nodo,abb -> destruir_dato,abb);
	abb -> cantidad --;
	return dato;
}


// -_-_-_-_-_-_-_-_-_-_-  ITERADOR DEL ABB  -_-_-_-_-_-_-_-_-_-_- //

bool _abb_in_order(nodo_t* nodo, bool visitar(const char *, void *, void *), void *extra){
	if(!nodo) return true;

	return _abb_in_order(nodo -> izq,visitar,extra) &&
				visitar(nodo -> clave,nodo -> dato, extra) &&
					_abb_in_order(nodo -> der,visitar,extra);
}

void abb_in_order(abb_t* abb, bool visitar(const char *, void *, void *), void *extra){
	if(!abb || abb_raiz_esta_vacia(abb)) return;

	_abb_in_order(abb -> raiz,visitar,extra);
}



void apilar_hijos_izq(pila_t* pila, const nodo_t* nodo){
	nodo_t* nodo_izq = nodo -> izq;

	while(nodo_izq){
		pila_apilar(pila,nodo_izq);
		nodo_izq = nodo_izq -> izq;
	}
}

abb_iter_t* abb_iter_in_crear(const abb_t *abb){
	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
	if(!iter) return NULL;

	iter -> pila  = pila_crear();

	if(!iter -> pila){
		free(iter);
		return NULL;
	}

	iter -> abb = abb;
	/*
	if(!abb_raiz_esta_vacia((abb_t* )iter -> abb)){
		pila_apilar(iter -> pila,(abb_t*) iter -> abb -> raiz);
		apilar_hijos_izq(iter -> pila, iter -> abb -> raiz);
	}
	*/

	return iter;
}

bool abb_iter_in_al_final(const abb_iter_t *iter){
	return pila_esta_vacia(iter -> pila);
}

bool abb_iter_in_avanzar(abb_iter_t *iter){
	if(abb_iter_in_al_final(iter))
		return false;

	nodo_t* desapilado = pila_desapilar(iter -> pila);

	if(desapilado -> der){ //&& pila_ver_tope(iter -> pila) && ((nodo_t*)pila_ver_tope(iter -> pila)) -> der != desapilado
		pila_apilar(iter -> pila,desapilado -> der);
		apilar_hijos_izq(iter -> pila,desapilado -> der);
	}

	
	return true;
}

const char* abb_iter_in_ver_actual(const abb_iter_t *iter){
	nodo_t* nodo = pila_ver_tope(iter -> pila);
	if (!nodo) return NULL;
	return nodo -> clave;
}

void abb_iter_in_llegar_a(abb_iter_t* iter,char* llegada){
	abb_buscar(iter->abb,llegada,iter->pila);
}

void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter -> pila);
	free(iter);
}


/*                                                         Z z
                                                    (-.-) z
                                                    (   )             */                                                    
// -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_ //