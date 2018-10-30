


typedef int (*cmp_func_t) (const void* a,const void* b);

typedef struct heap{
	void** elementos;
	size_t cantidad;
	cmp_func_t cmp;
} heap_t;

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
	return heap;
}

bool redimensionar(heap_t* heap){
	void** nuevos_elem = realloc(heap->elementos,FACTOR_RED);
	if (!nuevos_elem) return false;
	heap->elementos = nuevos_elem;
	return true;
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
		downheap(elementos,i);

	heap -> elementos = elementos;
	heap -> cantidad = n;
	heap -> cmp = cmp;
	return heap;
}

void* heap_ver_max(const heap_t *heap){
	if (!heap || heap->cantidad = 0) return NULL;
	return heap -> elementos[0];
}