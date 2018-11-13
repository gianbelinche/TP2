#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdlib.h>


/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/


typedef struct lista lista_t;
typedef struct lista_iter lista_iter_t;

/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t *lista_crear(void);

// Devuelve verdadero o falso, según si la lista tiene o no elementos.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

//Se inserta el dato pasado por parámetro en la primer posición de la lista.
//Pre: la lista fue creada.
//Post: todos los elementos que tenía la pila anteriormente se reubicaron según corresponda.
bool lista_insertar_primero(lista_t *lista, void *dato);

//Se inserta el dato pasado por parámetro en la última posición de la lista.
//Pre: la lista fue creada.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

//Se elemina el primer elemento de la lista.
//Pre: la lista fue creada.
//Post: el resto de elementos son reubicados según corresponda, si la lista
//esta vacia, devuelve NULL;
void *lista_borrar_primero(lista_t *lista);

// Obtiene el valor del primer elemento de la lista. Si la cola tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void *lista_ver_primero(const lista_t *lista);

// Obtiene el valor del último elemento de la lista. Si la cola tiene
// elementos, se devuelve el valor del último, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el último elemento de la lista, cuando no está vacía.
void *lista_ver_ultimo(const lista_t* lista);

//Devuelve la cantidad de elementos de la lista.
//Pre: la lista fue creada.
//Post: si la lista esta vacía, devuelve 0.
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/

//Itera la lista, aplicando la función visitar a cada elemento, si la función visitar,
//devuelve true, continúa la iteración al siguiente elemento, en caso de que devuelva false,
//o que no haya mas elementos, detiene la iteración.
//Pre: la lista fue creada, extra es un parámetro de "estado" para que el usuario utilize,
//visitar es una función provista por el usuario, cuyo funcionamiento depende de la logica que
//este quiera aplicar.
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

//Crea un iterador externo para la lista.
//Post: devuelve el iterador. 
lista_iter_t *lista_iter_crear(lista_t *lista);

//Avanza al siguiente elemento de la lista.
//Pre: el iterador fue creado.
//Post: devuelve true si se pudo avanzar y false en caso contrario.
bool lista_iter_avanzar(lista_iter_t *iter);

//Devuelve el elemento en el que se encuentra el iterador.
//Pre: el iterador fue creado.
//Post: si no hay elemento, se devuelve NULL. 
void *lista_iter_ver_actual(const lista_iter_t *iter);

//Devuelve true si se encuentra al final de la lista,
//false en caso contrario.
//Pre: el iterador fue creado.
bool lista_iter_al_final(const lista_iter_t *iter);

//Destruye el iterador externo.
//Pre: el iterador fue creado.
void lista_iter_destruir(lista_iter_t *iter);

//Inserta un elemento en la posicion actual del iterador,
//el elemento que se encontraba en esa posicion, es reubicado en la
//posicion siguiente, y se sigue la misma logica con los adyacentes.
//Pre: el iterador fue creado.
//Post: el elemento se encuentra en la posicion actual.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

//Se elimina el elemento de la posicion actual
//Pre: el iterador fue creado.
//Post: el resto de elementos es reubicado según corresponda.
void *lista_iter_borrar(lista_iter_t *iter);


/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

// Realiza pruebas sobre la implementación del alumno.
//
// Las pruebas deben emplazarse en el archivo ‘pruebas_alumno.c’, y
// solamente pueden emplear la interfaz pública tal y como aparece en lista.h
// (esto es, las pruebas no pueden acceder a los miembros del struct lista).
//
// Para la implementación de las pruebas se debe emplear la función
// print_test(), como se ha visto en TPs anteriores.
void pruebas_lista_alumno(void);


#endif // LISTA_H