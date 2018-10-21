#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// -_-_-_-_-_-_-_-_-_-_-_      CONSTANTES     -_-_-_-_-_-_-_-_-_-_-_- //

#define TAM_INICIAL 256
#define NO_ENCONTRADO -1
#define OCUPADO 1
#define BORRADO -1
#define FACTOR_DE_REDIMENSION  0.61
#define ESCALAR_DE_REDIMENSION 2
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
	size_t capacidad;
	hash_destruir_dato_t destruir_dato;
} hash_t;

typedef struct hash_iter{ 
	const hash_t* hash;
	int actual;
}hash_iter_t;


// -_-_-_-_-_-_-_-_-_-_-  FUNCIONES AUXILIARES  -_-_-_-_-_-_-_-_-_-_- //
uint32_t murmurhash (const char *key, uint32_t len, uint32_t seed);

uint32_t funcion_hash(const char* clave,uint32_t largo){
	uint32_t seed = 1548786; //Número random que debe respetarse en cada llamada
	return murmurhash (clave, largo,seed);
}

campo_t* campo_crear(const char* clave, void* dato){
	campo_t* campo = malloc(sizeof(campo_t));
	if (!campo) return NULL;
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

int hash_buscar(const hash_t* hash, const char* clave){
	uint32_t pos = funcion_hash(clave,strlen(clave)) % hash->capacidad;

	while (hash->campos[pos]){
		campo_t* campo = hash->campos[pos];

		if (campo->estado == OCUPADO && !strcmp(campo->clave,clave))
			return pos;

		pos++;
		pos %= hash->capacidad;
	}

	return NO_ENCONTRADO;
}

bool redimensionar(hash_t* hash,int porcentaje){

	uint32_t pos;
	campo_t* campo;
	campo_t** campos_nuevos = calloc(hash->capacidad*porcentaje,sizeof(campo_t*));
	if (!campos_nuevos) return false;

	for (int i=0;i<hash->capacidad;i++){
		campo = hash->campos[i];
		if (campo){
		
			if(campo->estado == OCUPADO){
			pos = funcion_hash(campo->clave,strlen(campo->clave));
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

// -_-_-_-_-_-_-_-_-_-_-  PRIMITIVAS DEL HUSH  -_-_-_-_-_-_-_-_-_-_- //

hash_t* hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if (!hash) return NULL;

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
	if (!campo) return false;

	if ( (hash->ocupados + hash->borrados) > hash->capacidad*FACTOR_DE_REDIMENSION){
		bool ok = redimensionar(hash,ESCALAR_DE_REDIMENSION);
		if (!ok) return false;
	} 

	int esta = hash_buscar(hash,clave);
	if (esta != NO_ENCONTRADO){
		campo_t* a_eliminar = hash->campos[esta];
		hash->campos[esta] = campo;
		campo_destruir(a_eliminar, hash -> destruir_dato);
		return true;
	}

	uint32_t posicion = funcion_hash(clave,strlen(clave));
	posicion %= hash->capacidad;
	
	while(hash->campos[posicion]){
		posicion++;
		posicion %= hash->capacidad;
	}
	hash->campos[posicion] = campo;
	hash->ocupados++;
	hash->vacios--;
	return true;
}

void* hash_obtener(const hash_t* hash, const char* clave){
	int pos = hash_buscar(hash,clave);
	if (pos == NO_ENCONTRADO) return NULL;
	return (hash->campos[pos])->dato;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	int pos = hash_buscar(hash,clave);
	if (pos == NO_ENCONTRADO) return false;
	return true;
}

size_t hash_cantidad(const hash_t *hash){
	return hash -> ocupados;
}
void* hash_borrar(hash_t *hash, const char *clave){
	int pos = hash_buscar(hash,clave);
	if (pos == NO_ENCONTRADO) return NULL;

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

// -_-_-_-_-_-_-_-_-_-_-  ITERADOR DEL HUSH  -_-_-_-_-_-_-_-_-_-_- //

bool hash_iter_al_final(const hash_iter_t* iter){
	return (iter->actual == -1);
}

bool hash_iter_avanzar(hash_iter_t* iter){
	campo_t** campos = iter->hash->campos;
	if (!hash_iter_al_final(iter)) iter->actual++;
	while(!hash_iter_al_final(iter))
	{
		if (iter->actual >= iter->hash->capacidad) {
			iter->actual = -1;
			continue;
		}

		if(campos[iter->actual] && campos[iter->actual]->estado != BORRADO)
		{
			return true;
		}
		iter->actual++;	
		
		if (iter->actual >= iter->hash->capacidad) {
			iter->actual = -1;
			continue;
		}
	}
	return false;
}

hash_iter_t* hash_iter_crear(const hash_t* hash){
	hash_iter_t* iter = malloc(sizeof(hash_iter_t));
	if (!iter) return NULL;
	iter->hash = hash;
	iter->actual = 0;
	if (hash->ocupados == 0) iter->actual = -1;
	else {
		while((!hash->campos[iter->actual]) || hash->campos[iter->actual]->estado == BORRADO){
			iter->actual++;
			if (iter->actual >= hash->capacidad){
				iter->actual = -1;
				break;
			}
		}
	}
	return iter;
}


const char* hash_iter_ver_actual(const hash_iter_t *iter){
	if (hash_iter_al_final(iter)) return NULL;
	return iter -> hash -> campos[iter -> actual] -> clave;
}

void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
}

// -_-_-_-_-_-_-_-_-_-_-  FUNCION DE HASHING  -_-_-_-_-_-_-_-_-_-_- //
// ACORDARSE DE MOVER ESTO A LA PARTE QUE LE CORRESPONDE BLA BLA BLA//

uint32_t murmurhash (const char *key, uint32_t len, uint32_t seed) {
  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;
  uint32_t r1 = 15;
  uint32_t r2 = 13;
  uint32_t m = 5;
  uint32_t n = 0xe6546b64;
  uint32_t h = 0;
  uint32_t k = 0;
  uint8_t *d = (uint8_t *) key; // 32 bit extract from `key'
  const uint32_t *chunks = NULL;
  const uint8_t *tail = NULL; // tail - last 8 bytes
  int i = 0;
  int l = len / 4; // chunk length

  h = seed;

  chunks = (const uint32_t *) (d + l * 4); // body
  tail = (const uint8_t *) (d + l * 4); // last 8 byte chunk of `key'

  // for each 4 byte chunk of `key'
  for (i = -l; i != 0; ++i) {
	// next 4 byte chunk of `key'
	k = chunks[i];

	// encode next 4 byte chunk of `key'
	k *= c1;
	k = (k << r1) | (k >> (32 - r1));
	k *= c2;

	// append to hash
	h ^= k;
	h = (h << r2) | (h >> (32 - r2));
	h = h * m + n;
  }

  k = 0;

  // remainder
  switch (len & 3) { // `len % 4'
	case 3: k ^= (tail[2] << 16);
	case 2: k ^= (tail[1] << 8);

	case 1:
	  k ^= tail[0];
	  k *= c1;
	  k = (k << r1) | (k >> (32 - r1));
	  k *= c2;
	  h ^= k;
  }

  h ^= len;

  h ^= (h >> 16);
  h *= 0x85ebca6b;
  h ^= (h >> 13);
  h *= 0xc2b2ae35;
  h ^= (h >> 16);

  return h;
}