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
	return *(int*)x - *(int*)y;
}

bool cumple_condicion_heap(heap_t* heap, cmp_func_t cmpx){
	void* anterior = heap_desencolar(heap);
	while(!heap_esta_vacio(heap)){
		if(cmpx(anterior,heap_ver_max(heap)) < 0)
			return false;

		anterior = heap_desencolar(heap);
	}
    
	return true;
}

static void prueba_heap_crear_vacio(){
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

    int  valores[] = {10,9,8,4,5,8,3,1,7,6,1,12,45,78};
    char msg[30];

    for(int i = 0; i < 10; i++){
    		heap_encolar(heap,&valores[i]);
    	    sprintf(msg,"Inserto valor %i",i + 1);
    	    print_test(msg,heap_cantidad(heap) == i + 1 && !heap_esta_vacio(heap));
    }

    print_test("Se cumple la condicion de heap",cumple_condicion_heap(heap,cmp));
    heap_destruir(heap,NULL);
}

static void prueba_heap_volumen(size_t largo){
    puts("PRUEBA HEAP VOLUMEN");
    heap_t* heap = heap_crear(cmp);

    int valores[largo];
    bool ok = true;

    for(int i = 0; i < largo;i++){
    	valores[i] = rand();
    	ok = heap_encolar(heap,&valores[i]) && ok;
    }

    print_test("Almacenar muchos elementos", ok);
    print_test("La cantidad de elementos es correcta", heap_cantidad(heap) == largo);
    print_test("Se cumple la condicion de heap",cumple_condicion_heap(heap,cmp));
    print_test("La cantidad de elementos es 0", heap_cantidad(heap) == 0);
	heap_destruir(heap,NULL);

}


static void prueba_heap_arr(){
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

static void prueba_heap_elementos_iguales(){
    puts("PRUEBA HEAP ELEMENTOS IGUALES");

    heap_t* heap = heap_crear(cmp);

    int valores[] = {3,3,3,8,5,8,5,8,5,8,3};

    char msg[30];

    for(int i = 0; i < 10; i++){
        heap_encolar(heap,&valores[i]);
        sprintf(msg,"Inserto valor %i",i + 1);
        print_test(msg,heap_cantidad(heap) == i + 1 && !heap_esta_vacio(heap));
    }
    bool ok = true;
    for (int i = 0; i<10; i++){
        int* max = heap_ver_max(heap);
        print_test("El heap no esta vacio", !heap_esta_vacio(heap));
        int* elem = heap_desencolar(heap);
        print_test("El maximo es correcto",*max == *elem);
        if (i < 4)
            ok = *elem == 8;
        else if (i < 7)
            ok = *elem == 5;
        else
            ok = *elem == 3;
        if (!ok) break;
    }

    print_test("Los elementos tienen la prioridad correcta",ok);
    heap_destruir(heap,NULL);
}

static void prueba_crear_arr_vacio(){
    puts("PRUEBA HEAP CREAR ARR VACIO");

    void* valores[] = {};

    heap_t* heap = heap_crear_arr(valores,0,cmp);
    print_test("El heap está creado",heap);
    print_test("Desencolar no es valido",!heap_desencolar(heap)); 
    print_test("La cantidad de elementos es 0",heap_cantidad(heap) == 0);
    print_test("El heap esta vacío",heap_esta_vacio(heap));
    print_test("Cumple la propiedad de heap",cumple_condicion_heap(heap,cmp));

    heap_destruir(heap,NULL);

}

static void prueba_heap_arr_vacio_insertar(){
    puts("PRUEBA INSERTAR CON ARR VACIO");
    void* valores[] = {};

    heap_t* heap = heap_crear_arr(valores,0,cmp);

    int i = 2, j = 1, k = 3;
    print_test("Elemento encolado correctamente",heap_encolar(heap,&i));
    print_test("Elemento encolado correctamente",heap_encolar(heap,&j));
    print_test("Elemento encolado correctamente",heap_encolar(heap,&k));

    print_test("El heap no esta vacio",!heap_esta_vacio(heap));
    print_test("Elemento desencolado con prioridad correcta",heap_desencolar(heap) == &k);
    print_test("El heap no esta vacio",!heap_esta_vacio(heap));
    print_test("Elemento desencolado con prioridad correcta",heap_desencolar(heap) == &i);
    print_test("El heap no esta vacio",!heap_esta_vacio(heap));
    print_test("Elemento desencolado con prioridad correcta",heap_desencolar(heap) == &j);
    print_test("El heap esta vacio",heap_esta_vacio(heap));
    print_test("No se puede desencolar el heap vacio",!heap_desencolar(heap));

    heap_destruir(heap,NULL);
}

static void prueba_heap_arr_insertar(){
    puts("PRUEBA HEAP ARR INSERTAR");
    int  valores [] = {10,9,8,4,5,8,3,1,7,6};
    int  mas_valores[] = {10,9,8,4,5,8,3,1,7,6,1,12,45,78};
    char msg[30];

    void* punteros[10];

    for(int i = 0;i < 10;i++)
        punteros[i] = &valores[i];

    heap_t* heap = heap_crear_arr(punteros, 10, cmp);
    print_test("Se crea exitosamente el heap",heap);

    for(int i = 0; i < 10; i++){
            heap_encolar(heap,&mas_valores[i]);
            sprintf(msg,"Inserto valor %i",i + 1);
            print_test(msg,heap_cantidad(heap) == i + 11);
    }

    print_test("Se cumple la condicion de heap",cumple_condicion_heap(heap,cmp));
    heap_destruir(heap,NULL);
}

static void prueba_heap_arr_volumen(size_t largo){

    puts("PRUEBA HEAP ARR VOLUMEN");
    int  valores [] = {10,9,8,4,5,8,3,1,7,6};
    int mas_valores[largo];

    void* punteros[10];

    for(int i = 0;i < 10;i++)
        punteros[i] = &valores[i];

    heap_t* heap = heap_crear_arr(punteros, 10, cmp);
    print_test("Se crea exitosamente el heap",heap);

    bool ok = true;

    for(int i = 0; i < largo;i++){
        mas_valores[i] = rand();
        ok = heap_encolar(heap,&mas_valores[i]) && ok;
    }

    print_test("Almacenar muchos elementos", ok);
    print_test("La cantidad de elementos es correcta", heap_cantidad(heap) == largo + 10);
    print_test("Se cumple la condicion de heap",cumple_condicion_heap(heap,cmp));
    print_test("La cantidad de elementos es 0", heap_cantidad(heap) == 0);
    heap_destruir(heap,NULL);
}
static void prueba_heapsort(){
    puts("PRUEBA HEAPSORT");

    bool ok = true;

    int decena[]              = {10,9,8,4,5,8,3,1,7,6};
    int decena_ordenado[]     = {1,3,4,5,6,7,8,8,9,10};

    int fibonnaci[]           = {1,34,55,5,8,13,3,1,2,21};
    int fibonnaci_ordenado[]  = {1,1,2,3,5,8,13,21,34,55};

    void* punteros[10];

    for(int i = 0;i < 10;i++)
        punteros[i] = &decena[i];

    heap_sort(punteros,10,cmp);

    for(int i = 0;i < 10;i++)
        if(*((int*)punteros[i]) != decena_ordenado[i])
            ok = false;

    print_test("El arreglo se ordena exitosamente",ok);

    for(int i = 0;i < 10;i++)
        punteros[i] = &fibonnaci[i];

    heap_sort(punteros,10,cmp);

    for(int i = 0;i < 10;i++)
        if(*((int*)punteros[i]) != fibonnaci_ordenado[i])
            ok = false;

    print_test("El arreglo se ordena exitosamente",ok);
}



void pruebas_heap_alumno(){
    srand((unsigned) clock());
    prueba_heap_crear_vacio();
    prueba_heap_insertar();
    prueba_heap_elementos_iguales();
    prueba_heap_volumen(5000);

    prueba_crear_arr_vacio();
    prueba_heap_arr_vacio_insertar();
    prueba_heap_arr();
    prueba_heap_arr_insertar();
    prueba_heap_arr_volumen(5000);

    prueba_heapsort();
}