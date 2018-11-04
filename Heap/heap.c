#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// -_-_-_-_-_-_-_-_-_-_-_-_-_-_  MACROS  _-_-_-_-_-_-_-_-_-_-_-_-_-_- //

#define PADRE(x)      (x - 1)/2
#define HIJO_DER(x)   2*x + 2
#define HIJO_IZQ(x)   2*x + 1

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

	void** nuevos_elem = realloc(heap -> elementos,sizeof(void*)*(heap -> capacidad)*FACTOR_REDIMENSION);
	if (!nuevos_elem) return false;
	heap -> elementos = nuevos_elem;
	heap -> capacidad *= FACTOR_REDIMENSION;
	return true;
}

void swap(void** elementos,size_t pos1,size_t pos2){
	void * aux = elementos[pos1];
	elementos[pos1] = elementos[pos2];
	elementos[pos2] = aux;
}


size_t max(void** elementos,size_t elem1, size_t elem2, cmp_func_t cmp){
	if (cmp(elementos[elem1],elementos[elem2]) > 0 ) return elem1;
	return elem2;
}

void downheap(void* elementos[],size_t cantidad,cmp_func_t cmp,size_t pos){
    size_t hijo_izq = HIJO_IZQ(pos);
    size_t hijo_der = HIJO_DER(pos);
    size_t hijo_mayor;

    if (hijo_izq >= cantidad) return;

    if (hijo_der >= cantidad){
        if (cmp(elementos[pos],elementos[hijo_izq]) > 0)
            return;
        hijo_mayor = hijo_izq;
    }
    else{
        hijo_mayor = max(elementos,hijo_izq,hijo_der,cmp);
        if (cmp(elementos[pos],elementos[hijo_mayor]) > 0)
            return;
        
    }
    
    swap(elementos,pos,hijo_mayor);
    downheap(elementos,cantidad,cmp,hijo_mayor);
}

void upheap(void* elementos[],size_t cantidad, cmp_func_t cmp, size_t pos){
	if (cantidad == 1 || pos==0) return;
	size_t padre = PADRE(pos);

	if ( cmp(elementos[pos],elementos[padre]) > 0){
		swap(elementos,pos,padre);
		upheap(elementos,cantidad,cmp,padre);
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

	heap -> elementos = elementos;
	heap -> capacidad = n*2;
	heap -> cantidad = n;
	heap -> cmp = cmp;

	for (size_t i = n;i > 0;i--)
		downheap(heap -> elementos, heap -> cantidad, heap -> cmp,i-1);

	return heap;
}

void* heap_ver_max(const heap_t *heap){

	if (!heap || heap -> cantidad == 0) return NULL;
	return heap -> elementos[0];
}

bool heap_encolar(heap_t* heap, void* dato){

	if( ((heap -> cantidad /heap -> capacidad)*100) > FACTOR_CARGA && !redimensionar(heap))
		return false;

	heap -> elementos[heap -> cantidad] = dato;
	(heap -> cantidad)++;
	upheap(heap -> elementos, heap -> cantidad, heap -> cmp,heap -> cantidad-1);
	
	return true;
}

void* heap_desencolar(heap_t *heap){

	if (!heap || heap -> cantidad == 0) return NULL;

	swap(heap -> elementos,0,heap -> cantidad - 1);
	(heap -> cantidad)--;
	downheap(heap -> elementos, heap -> cantidad, heap -> cmp,0);

	void* elem = heap -> elementos[heap -> cantidad];
	heap -> elementos[heap -> cantidad] = NULL;
	return elem;
}

size_t heap_cantidad(const heap_t *heap){

	return (!heap) ? 0 : heap -> cantidad;
}

bool heap_esta_vacio(const heap_t *heap){

	return (!heap) ? true : !heap -> cantidad;
}

void heap_sort(void *elementos[], size_t cantidad, cmp_func_t cmp){
	if (cantidad < 2) return;

	for(size_t i = cantidad - 1; i > 0; i--)
		downheap(elementos,cantidad,cmp,i);
	
	downheap(elementos,cantidad - 1,cmp,0);

	for(size_t i = cantidad - 1; i > 0; i--){
		swap(elementos,0,i);
		downheap(elementos,cantidad - 1,cmp,0);
		cantidad--;
	}

	downheap(elementos,cantidad - 1,cmp,0);
}

void heap_destruir(heap_t *heap, void destruir_elemento(void *elementos)){
	if(destruir_elemento)
		for(int i = 0;i < heap -> cantidad; i++)
			destruir_elemento(heap -> elementos[i]);

	free(heap -> elementos);
	free(heap);
}


/*		       ("`-'  '-/") .___..--' ' "`-._
		         ` O_ O  )    `-.   (      ) .`-.__. `)
		         (_Y_.) ' ._   )   `._` ;  `` -. .-'
		      _.. `--'_..-_/   /--' _ .' ,)
		   ( i l ),-''  ( l i),'  ( ( ! .-'  
()-().----.          .
 \"/` ___  ;___.'
  ` ^^   ^^                                                          */
// -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-  //