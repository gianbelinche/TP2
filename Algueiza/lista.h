#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>


// -_-_-_-_-_-_-_-  DECLARACION DE LOS TIPOS DE DATO  -_-_-_-_-_-_-_-_- //

typedef struct lista lista_t;
typedef struct lista_iter lista_iter_t;

// -_-_-_-_-_-_-_-_-_-_-  PRIMITIVAS DE LA LISTA  -_-_-_-_-_-_-_-_-_-_- //

/**
* Aclaraciones generales: 
*
* 1 - Las funciones de tipo booleano, devuelven verdadero si se verifica
*     aquello que su nombre afirma y falso en caso contrario.
*
* 2 - Es precondición de toda función aqui expeusta la existencia de la lista
*     que reciben por parametro, a excepción de lista_crear.
**/

//Post: Devuelve una lista vacia.
lista_t *lista_crear(void);

//Post: Devuelve un booleano, dependiento si la lista tiene o no elementos.
bool lista_esta_vacia(const lista_t *lista);


//Post: El elemento fue incluído al inicio de la lista.
bool lista_insertar_primero(lista_t *lista, void *dato);

//Post: El elemento fue incluído al final de la lista
bool lista_insertar_ultimo(lista_t *lista, void *dato);

//Post: Se eliminó el primer elemento de la lista y fué retornado.
void *lista_borrar_primero(lista_t *lista);

//Post: Se devuelve el primer elemento de la lista, o NULL si ésta
//		está vacía.
void *lista_ver_primero(const lista_t *lista);

//Post: Se devuelve el último elemento de la lista, o NULL si ésta
//		está vacía.
void *lista_ver_ultimo(const lista_t* lista);

//Post: Se retorna el largo de la lista. Si esta vacía su largo es 0.
size_t lista_largo(const lista_t *lista);

//Post: La lista se destruye. Si desruir dato es distinto de NULL
//		se aplica a cada elemento de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));

// -_-_-_-_-_-_-_-_-_-_-  ITERADORES DE LA LISTA  -_-_-_-_-_-_-_-_-_-_- //

//Pre:  La función visitar responde al formato presentado.
//Post: Recibe una función visitar a la que pasa por argumento cada elemento de
//		la lista y la variable extra. Interrumpe su función si visitar devuelve falso
//		o si se acaban los elementos de la lista.
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);


//Post: Se crea un puntero que se coloca en el primer elemento de la lista.
lista_iter_t* lista_iter_crear(lista_t *lista);

//Post: El puntero avanza a menos que ya se encuentre al final de la lista.
bool lista_iter_avanzar(lista_iter_t *iter);

//Post: Devuelve el valor actual del elemento al que el puntero señala.
void *lista_iter_ver_actual(const lista_iter_t *iter);

//Post: Retorna un booleano dependiendo de si se recorrieron o no todos los
//		elementos de la lista.
bool lista_iter_al_final(const lista_iter_t *iter);

//Post: Se agrega el elemento dado a la lista, en la posición en la que se encuentra
//		el puntero. El puntero apunta entonces al elemento agregado y el elemento que
//		ocupaba dicha posición es ahora el siguiente.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

//Post: Se elimina y se retorna el elemento que el puntero señalaba.El puntero ahora
//		marca el elemento que le seguía al eliminado.
void *lista_iter_borrar(lista_iter_t *iter);

//Post: Se elimina el iterador.
void lista_iter_destruir(lista_iter_t *iter);

// -_-_-_-_-_-_-_-_-_-_-_-   PRUEBAS UNITARIAS  -_-_-_-_-_-_-_-_-_-_-_- //

//Post: Se realizaron pruebas diseñadas por el alumno.
void pruebas_lista_alumno(void);

#endif // LISTA_H
