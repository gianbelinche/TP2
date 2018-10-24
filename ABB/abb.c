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

bool abb_pertenece(const abb_t* abb,const char* clave){
	abb_t* arbol = abb_buscar(abb,clave);
	return !arbol==NULL;
}

void* abb_obtener(const abb_t* abb,const char* clave){
	abb_t* arbol = abb_buscar(abb,clave);
	if (!arbol) return NULL;
	return arbol->dato;
}

void* abb_borrar(abb_t* abb,const char* clave){
	abb_t* arbol = abb_buscar(abb,clave);
	if (!arbol) return NULL;
	void* dato = arbol->dato;
	abb_t* padre = abb_obtener_padre(abb,clave);
	if (!arbol->izq && !arbol->der)
		abb_borrar_sin_hijos(padre,arbol);
	else if (arbol->izq || arbol->der)
		abb_borrar_un_hijo(padre,arbol);
	else
		abb_borrar_dos_hijos(arbol);
	return dato;
}
