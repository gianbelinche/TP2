#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// -_-_-_-_-_-_-_-_-_-_-_      CONSTANTES     -_-_-_-_-_-_-_-_-_-_-_- //

#define TAM_INICIAL 128
#define FACTOR_DE_REDIMENSION  61
#define ESCALAR_DE_REDIMENSION 2
#define FNV_PRIME_32  16777619
#define FNV_OFFSET_32 2166136261U
#define NO_ENCONTRADO -1
#define AL_FINAL      -1
#define OCUPADO  1
#define BORRADO -1

// -_-_-_-_-_-_-_-_  DEFINICION DE  TIPOS DE DATO  _-_-_-_-_-_-_-_-_- //

typedef void (*hash_destruir_dato_t)(void *);

typedef struct campo{
	char* clave;
	void* dato;
	short estado;
} campo_t;

typedef struct hash{
	campo_t** campos;
	size_t ocupados;
	size_t vacios;
	size_t borrados;
	uint32_t capacidad;
	hash_destruir_dato_t destruir_dato;
} hash_t;

typedef struct hash_iter{ 
	const hash_t* hash;
	int actual;
}hash_iter_t;


// -_-_-_-_-_-_-_-_-_-_-  FUNCIONES AUXILIARES  -_-_-_-_-_-_-_-_-_-_- //

uint32_t FNV32(const char *s)
{
    uint32_t hash = FNV_OFFSET_32, i;
    for(i = 0; i < strlen(s); i++)
    {
        hash = hash ^ (s[i]);       // Xor al byte siguiente hasta el final del hash.
        hash = hash * FNV_PRIME_32; // Multiplica por el número primo óptimo.
    }
    return hash;
} 

uint32_t funcion_hash(const char* clave){
	return FNV32(clave);
}

campo_t* campo_crear(const char* clave, void* dato){
	campo_t* campo = malloc(sizeof(campo_t));

	if (!campo) 
		return NULL;

	campo->clave = strdup(clave);
	campo->dato = dato;
	campo->estado = OCUPADO;
	return campo;
}

void campo_destruir(campo_t* campo, hash_destruir_dato_t destruir_dato){
	free(campo -> clave);

	if(destruir_dato)
		destruir_dato(campo -> dato);

	free(campo);
}

ssize_t hash_buscar(const hash_t* hash, const char* clave){
	uint32_t pos = funcion_hash(clave);
	pos %= hash->capacidad;

	while (hash->campos[pos]){
		campo_t* campo = hash->campos[pos];

		if (campo->estado == OCUPADO && !strcmp(campo->clave,clave))
			return pos;

		pos++;
		pos %= hash->capacidad;
	}

	return NO_ENCONTRADO;
}

bool _redimensionar(hash_t* hash,uint32_t porcentaje){

	uint32_t pos;
	campo_t* campo;
	campo_t** campos_nuevos = calloc(hash->capacidad*porcentaje,sizeof(campo_t*));

	if (!campos_nuevos)
		return false;

	for (int i=0;i<hash->capacidad;i++){
		campo = hash->campos[i];
		if (campo){
		
			if(campo->estado == OCUPADO){
				pos = funcion_hash(campo->clave);
				pos %= (hash->capacidad*porcentaje);

				while(campos_nuevos[pos]){
					pos++;
					pos %= hash->capacidad*porcentaje;
				}

			campos_nuevos[pos] = campo;
			}
			else{
				campo_destruir(campo,NULL);
			}
		}
	}

	free(hash -> campos);
	hash->campos = campos_nuevos;
	hash->capacidad *= porcentaje;
	hash->borrados = 0;
	hash->vacios = hash->capacidad - hash->ocupados;
	return true;
}

// -_-_-_-_-_-_-_-_-_-_-  PRIMITIVAS DEL HASH  -_-_-_-_-_-_-_-_-_-_- //

hash_t* hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if (!hash)
		return NULL;

	campo_t** campos = calloc(TAM_INICIAL,sizeof(campo_t*));
	if (!campos){
		free(hash);
		return NULL;
	}

	hash->campos = campos;
	hash->ocupados = 0;
	hash->vacios = TAM_INICIAL;
	hash->borrados = 0;
	hash->capacidad = TAM_INICIAL;
	hash->destruir_dato = destruir_dato;
	return hash;
}

bool hash_guardar(hash_t* hash, const char* clave, void* dato){
	campo_t* campo = campo_crear(clave,dato);
	if (!campo)
		return false;

	if ( (hash->ocupados + hash->borrados)*100/hash->capacidad > FACTOR_DE_REDIMENSION){
		bool ok = _redimensionar(hash,ESCALAR_DE_REDIMENSION);
		if (!ok)
			return false;
	} 

	ssize_t esta = hash_buscar(hash,clave);
	if (esta != NO_ENCONTRADO){
		campo_t* a_eliminar = hash->campos[esta];
		hash->campos[esta] = campo;
		campo_destruir(a_eliminar, hash -> destruir_dato);
		return true;
	}

	uint32_t pos = funcion_hash(clave);
	pos %= hash->capacidad ;
	
	while(hash->campos[pos]){
		pos++;
		pos %= hash->capacidad;
	}
	hash->campos[pos] = campo;
	hash->ocupados++;
	hash->vacios--;
	return true;
}

void* hash_obtener(const hash_t* hash, const char* clave){
	ssize_t pos = hash_buscar(hash,clave);
	if (pos == NO_ENCONTRADO) 
		return NULL;
	return (hash->campos[pos])->dato;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	ssize_t pos = hash_buscar(hash,clave);
	if (pos == NO_ENCONTRADO)
		return false;
	return true;
}

size_t hash_cantidad(const hash_t *hash){
	return hash -> ocupados;
}

void* hash_borrar(hash_t *hash, const char *clave){
	ssize_t pos = hash_buscar(hash,clave);
	if (pos == NO_ENCONTRADO)
		return NULL;

	hash-> campos[pos] -> estado = BORRADO;
	(hash-> borrados)++;
	(hash-> ocupados)--;

	return hash-> campos[pos] -> dato;
}

void hash_destruir(hash_t *hash){
	for(size_t i = 0; i < hash -> capacidad; i++){
		if(hash-> campos[i]){
			if(hash-> campos[i] -> estado == OCUPADO)
				campo_destruir(hash-> campos[i],hash -> destruir_dato);
			else
				campo_destruir(hash-> campos[i],NULL);
		}
	}

	free(hash -> campos);
	free(hash);
	
}

// -_-_-_-_-_-_-_-_-_-_-  ITERADOR DEL HASH  -_-_-_-_-_-_-_-_-_-_- //

bool hash_iter_al_final(const hash_iter_t* iter){
	return (iter->actual == AL_FINAL);
}

bool hash_iter_avanzar(hash_iter_t* iter){
	campo_t** campos = iter->hash->campos;

	while(!hash_iter_al_final(iter))
	{
		iter->actual++;

		if (iter->actual >= iter->hash->capacidad)
			iter->actual = AL_FINAL;

		else if (campos[iter->actual] && campos[iter->actual]->estado != BORRADO)
			return true;
	}

	return false;
}

hash_iter_t* hash_iter_crear(const hash_t* hash){
	hash_iter_t* iter = malloc(sizeof(hash_iter_t));
	if (!iter)
		return NULL;

	iter->hash = hash;
	iter -> actual = (hash->ocupados) ?  0 : AL_FINAL;

	hash_iter_avanzar(iter);
	return iter;
}

const char* hash_iter_ver_actual(const hash_iter_t *iter){
	if (hash_iter_al_final(iter))
		return NULL;

	return iter -> hash -> campos[iter -> actual] -> clave;
}

void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
}

