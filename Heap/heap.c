#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// -_-_-_-_-_-_-_-_-_-_-_-_-_-_  MACROS  _-_-_-_-_-_-_-_-_-_-_-_-_-_- //

#define PADRE(x)      (x + 1)/2
#define HIJO_DER(x)   2*x + 1
#define HIJO_IZQ(x)   2*x + 2

#define TAM_INICIAL  32
#define FACTOR_CARGA 70
#define FACTOR_REDIMENSION  2

// -_-_-_-_-_-_-_-_  DEFINICION DE  TIPOS DE DATO  _-_-_-_-_-_-_-_-_- //

typedef int (*cmp_func_t) (const void* a,const void* b);

typedef struct heap{
	void** elementos;
	size_t cantidad;
	size_t capacidad;
	cmp_func_t cmp;
} heap_t;

// -_-_-_-_-_-_-_-_-_-_-  FUNCIONES AUXILIARES  -_-_-_-_-_-_-_-_-_-_- //

bool redimensionar(heap_t* heap){

	void** nuevos_elem = realloc(heap -> elementos,FACTOR_REDIMENSION);
	if (!nuevos_elem) return false;
	heap->elementos = nuevos_elem;
	return true;
}

void swap (void** x, void** y){

	void* z = *x;
	*x = *y;
	*y = z;
}

int obtener_pos_hijo_mayor(void* elementos[], size_t cantidad, cmp_func_t cmp, size_t pos){

	int hijo_der = HIJO_DER(pos);
	int hijo_izq = HIJO_IZQ(pos);

	if(hijo_der > cantidad) return -1;

	if(cmp(elementos[hijo_der],elementos[hijo_izq]) > 0)
		return hijo_der;

	return hijo_izq;
}

void downheap(void* elementos[], size_t cantidad, cmp_func_t cmp, size_t pos){

	int pos_hijo_mayor = obtener_pos_hijo_mayor(elementos,cantidad,cmp,pos);

	while(pos_hijo_mayor >= 0 && cmp(elementos[pos_hijo_mayor],elementos[pos]) > 0){
		swap(elementos + pos_hijo_mayor, elementos + pos);
		pos = pos_hijo_mayor;
		pos_hijo_mayor = obtener_pos_hijo_mayor(elementos,cantidad,cmp,pos);
	}
}

void upheap(void* elementos[], size_t cantidad, cmp_func_t cmp, size_t pos){

	int pos_padre = PADRE(pos);

	while(pos_padre != pos && cmp(elementos[pos_padre], elementos[pos])){
		swap(elementos + pos_padre, elementos + pos);
		pos = pos_padre;
		pos_padre = PADRE(pos);
	}
}

// -_-_-_-_-_-_-_-_-_-_-   PRIMITIVAS DEL HEAP  -_-_-_-_-_-_-_-_-_-_- //

heap_t* heap_crear(cmp_func_t cmp){

	heap_t* heap = malloc(sizeof(heap_t));
	if (!heap) return NULL;

	void** elementos = malloc(sizeof(void*) * TAM_INICIAL);
	if (!elementos){
		free(heap);
		return NULL;
	}

	heap -> elementos = elementos;
	heap -> cantidad = 0;
	heap -> cmp = cmp;
	heap -> capacidad = TAM_INICIAL;
	return heap;
}

heap_t* heap_crear_arr(void* arreglo[], size_t n, cmp_func_t cmp){

	heap_t* heap = malloc(sizeof(heap_t));
	if (!heap) return NULL;

	void** elementos = malloc(sizeof(void*) * n);
	if (!elementos){
		free(heap);
		return NULL;
	}

	for (int i = 0;i<n;i++)
		elementos[i] = arreglo[i];

	for (int i = n-1;i >= 0;i--)
		downheap(heap -> elementos, heap -> cantidad, heap -> cmp,i);

	heap -> elementos = elementos;
	heap -> cantidad = n;
	heap -> cmp = cmp;
	return heap;
}

void* heap_ver_max(const heap_t *heap){

	if (!heap || heap->cantidad == 0) return NULL;
	return heap -> elementos[0];
}

bool heap_encolar(heap_t* heap, void* dato){

	if( (heap -> cantidad /heap -> capacidad) > FACTOR_CARGA && !redimensionar(heap))
		return false;

	heap->elementos[heap->cantidad] = dato;
	upheap(heap -> elementos, heap -> cantidad, heap -> cmp,heap -> cantidad);
	(heap -> cantidad)++;
	return true;
}

void* heap_desencolar(heap_t *heap){

	if (!heap || heap->cantidad == 0) return NULL;
	swap(heap -> elementos, heap -> elementos + heap -> cantidad);
	heap -> cantidad--;
	upheap(heap -> elementos, heap -> cantidad, heap -> cmp,0);
	return heap -> elementos[heap -> cantidad + 1];
}

size_t heap_cantidad(const heap_t *heap){

	return (!heap) ? 0 : heap -> cantidad;
}

bool heap_esta_vacio(const heap_t *heap){

	return (!heap) ? true : !heap -> cantidad;
}

void heap_sort(void *elementos[], size_t cantidad, cmp_func_t cmp){
	if (cantidad == 0) return;  //i = cantidad -1 puede fallar
	for(int i = cantidad-1; i >= 0; i--)
		downheap(elementos,cantidad,cmp,i);

	for(int i = cantidad; i >= 0; i--){
		swap(&elementos[0],&elementos[i]);
		upheap(elementos,cantidad,cmp,0);
	}
}
<<<<<<< HEAD

void heap_destruir(heap_t *heap, void destruir_elemento(void *elementos))
{
	if(destruir_elemento)
		for(int i = 0;i < heap -> cantidad; i++)
			destruir_elemento(heap -> elementos[i]);

	free(heap -> elementos);
	free(heap);
}
=======
>>>>>>> 7683b0e669dc987e82ff65ebb5f057e357fe3dab
