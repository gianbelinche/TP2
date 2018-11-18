#ifndef ABB_H
#define ABB_H

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

// -_-_-_-_-_-_-_-  DECLARACION DE LOS TIPOS DE DATO  -_-_-_-_-_-_-_-_- //

typedef struct abb_iter abb_iter_t;
typedef struct abb abb_t;

typedef int  (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);

// -_-_-_-_-_-_-_-_-_-_-  PRIMITIVAS DEL ABB  -_-_-_-_-_-_-_-_-_-_- //

/**
* Aclaraciones generales: 
*
* 1 - Las funciones de tipo booleano, devuelven verdadero si se verifica
*     aquello que su nombre afirma y falso en caso contrario.
*
* 2 - Es precondición de toda función aqui expeusta la existencia del árbol/iterador
*     que reciben por parametro.
**/

//Pre:  La función de comparación debe ser distinta a NULL. Además debe devolver
//		un entero negativo si la primera cadena es menor que la segunda, cero si
//		coinciden o un entero positivo en el caso restante.
//Post: Devuelve un arbol vacío.
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

//Post: Inserta el elemento en el árbol, junto con su calve asociada.
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

//Post: Si existe un elemento en el árbol cuya clave coincide con la indicada, retorna
//		dicho elemento. En caso contrario devuelve NULL. 
void *abb_obtener(const abb_t *arbol, const char *clave);

//Post: Retorna un bool si es que existe un elemento en el árbol cuya clave coincide con 
//		la indicada.
bool abb_pertenece(const abb_t *arbol, const char *clave);

//Post: Devuelve la cantidad de elementos en un árbol.
size_t abb_cantidad(abb_t *arbol);

//Post: Si existe un elemento en el árbol cuya clave coincide con la indicada, lo elimina
//		y retorna su valor. En caso contrario devuelve NULL.
void *abb_borrar(abb_t *arbol, const char *clave);

//Post: Destruye el arbol y aplica la función de destrucción a todos los elementos que
//		contiene.
void abb_destruir(abb_t *arbol);

// -_-_-_-_-_-_-_-_-_-_-  ITERADORES DEL ABB  -_-_-_-_-_-_-_-_-_-_- //

//Pre:  La función visitar responde al formato presentado.
//Post: Recibe una función visitar a la que pasa por argumento cada elemento
//		del árbol, su calve y la variable extra. Interrumpe su función si visitar devuelve 
//		falso o si se acaban los elementos del árbol. 
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);

//Post: Se crea un puntero que se coloca en el primer elemento del arbol.
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

//Post: El puntero avanza a menos que ya se encuentre al final del árbol.
bool abb_iter_in_avanzar(abb_iter_t *iter);

//Post: Devuelve el valor actual del elemento al que el puntero señala.
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

//Post: Retorna un booleano dependiendo de si se recorrieron o no todos los
//		elementos del árbol.
bool abb_iter_in_al_final(const abb_iter_t *iter);

//Post: Se elimina el iterador.
void abb_iter_in_destruir(abb_iter_t* iter);

void abb_iter_in_llegar_a(abb_iter_t* iter,char* llegada);

#endif // ABB_H