#include "heap.h"
#include "testing.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* ******************************************************************
 *                       PRUEBAS UNITARIAS HEAP
 * *****************************************************************/

int cmp (const void* x,const void* y){
	return *((int*) x) - *((int*) y);
}

bool cumple_condicion_heap(heap_t* heap, cmp_func_t cmpx)
{
	void* anterior = heap_desencolar(heap);
	while(!heap_esta_vacio(heap))
	{
		if(cmpx(anterior,heap_ver_max(heap)) < 0)
			return false;

		anterior = heap_desencolar(heap);
	}

	return true;
}

static void prueba_crear_heap_vacio(){
     puts("PRUEBA HEAP CREAR VACIO");
     heap_t* heap = heap_crear(cmp);
     print_test("El heap está vacío", heap_esta_vacio(heap));
     print_test("La cantidad de elemetnos es nula", heap_cantidad(heap) == 0);
     print_test("Obtener máximo devuelve NULL",!heap_ver_max(heap));
     print_test("Desencolar un elemento devuelve NULL",!heap_desencolar(heap));
     heap_destruir(heap,NULL);
}

static void prueba_heap_insertar(){
    puts("PRUEBA HEAP INSERTAR");
    heap_t* heap = heap_crear(cmp);

    int  valores[] = {10,9,8,4,5,8,3,1,7,6,1};
    char msg[30];

    for(int i = 1; i < 11; i++)
    {
    		heap_encolar(heap,&valores[i]);
    	    sprintf(msg,"Inserto valor %i",i);
    	    print_test(msg,heap_cantidad(heap) == i && !heap_esta_vacio(heap));
    }

    print_test("Se cumple la condicion de heap",cumple_condicion_heap(heap,cmp));
    heap_destruir(heap,NULL);
}

static void prueba_heap_volumen(size_t largo){
    puts("PRUEBA HEAP VOLUMEN");
    heap_t* heap = heap_crear(cmp);

    int valores[largo];
    bool ok = true;

    for(int i = 0; i < largo;i++)
    {
    	valores[i] = rand();
    	ok = heap_encolar(heap,&valores[i]) && ok;
    }

    print_test("Prueba heap almacenar muchos elementos", ok);
    print_test("Prueba heap la cantidad de elementos es correcta", heap_cantidad(heap) == largo);
    print_test("Se cumple la condicion de heap",cumple_condicion_heap(heap,cmp));
    print_test("Prueba heap la cantidad de elementos es 0", heap_cantidad(heap) == 0);
	heap_destruir(heap,NULL);

}

static void prueba_heap_arr()
{
	puts("PRUEBA HEAP ARR");
	int  valores[10] = {10,9,8,4,5,8,3,1,7,6};
	void* punteros[10];

	for(int i = 0;i < 10;i++)
		punteros[i] = &valores[i];

	heap_t* heap = heap_crear_arr(punteros, 10, cmp);
	print_test("Se crea exitosamente el heap",heap);
	print_test("Se cumple la condicion de heap",cumple_condicion_heap(heap,cmp));
    heap_destruir(heap,NULL);
}

void pruebas_heap_alumno(){
	srand((unsigned) clock());
    prueba_crear_heap_vacio();
    prueba_heap_insertar();
    prueba_heap_arr();
    
    prueba_heap_volumen(5000);
}