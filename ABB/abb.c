//Este va a ser el abb
typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);

typedef struct abb{
	struct abb* izq;
	struct abb* der;
	char* clave;
	void* dato;
	size_t cantidad;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir_dato;
}abb_t;

void abb_destruir(abb_t* abb){
	if (!abb) return;
	abb_destruir(abb->izq);
	abb_destruir(abb->der);
	free(abb->clave);
	if (abb->destruir_dato)
		abb->destruir_dato(abb->dato);
	free(abb);
}

