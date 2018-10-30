


typedef int (*cmp_func_t) (const void* a,const void* b);

typedef struct heap{
	void** elementos;
	size_t cantidad;
	cmp_func_t cmp;
} heap_t;

