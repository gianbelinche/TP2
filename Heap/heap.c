


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

