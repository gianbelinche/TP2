#include "pila.h"
#include <stdlib.h>
#include <stdbool.h>
#define TAM_MIN 10

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

// ...

pila_t* pila_crear(){
	pila_t* pila = malloc(sizeof(pila_t));
	pila->datos = malloc(TAM_MIN * sizeof(void*));
	pila->cantidad = 0;
	pila->capacidad = TAM_MIN;
	return pila;
}

void pila_destruir(pila_t *pila){
	free(pila->datos);
	free(pila);
}

bool pila_esta_vacia(const pila_t *pila){
	if (pila->cantidad == 0)
		return true;
	return false;
}

bool pila_apilar(pila_t *pila, void* valor){
	if (pila->cantidad == pila->capacidad){
		void* datos_nuevos = realloc(pila->datos,pila->capacidad * 2 * sizeof(void*));
		if (datos_nuevos == NULL)
			return false;
		else{
			pila->datos = datos_nuevos;
			pila->capacidad *= 2;
		}
	}
	(pila->datos)[pila->cantidad] = valor;
	pila->cantidad += 1;
	return true;
} 

void* pila_ver_tope(const pila_t *pila){
	if (pila_esta_vacia(pila))
		return NULL;
	return (pila->datos)[pila->cantidad - 1];
}

void* pila_desapilar(pila_t *pila){
	if (pila_esta_vacia(pila))
		return NULL;
	if (pila->cantidad <= pila->capacidad*(1/4) && pila->capacidad >= TAM_MIN){
		void* datos_nuevos = realloc(pila->datos,(pila->capacidad / 2) * sizeof(void*));
		if (datos_nuevos == NULL)
			return NULL;
		else{
			pila->datos = datos_nuevos;
			pila->capacidad /= 2;
		}
	}	
	void* elemento = (pila->datos)[pila->cantidad - 1];
	pila->cantidad -= 1;
	return elemento;
}
