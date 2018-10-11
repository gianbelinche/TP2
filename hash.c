typedef void destruir(void*) destruir_t;

typedef struct campo{
	char* calve;
	void* dato;
	short estado;
} campo_t;

typedef struct hash{
	campo_t** campos;
	size_t ocupados;
	size_t vacios;
	size_t borrados;
	destruir_t destruir_dato;
} hash_t;