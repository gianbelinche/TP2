#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#define TAM_INICIAL 97

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

uint32_t funcion_hash(const char* clave,uint32_t largo){
	uint32_t seed = 1548786; //Número random que debe respetarse en cada llamada
	return murmurhash (clave, largo,seed);
}


typedef void hash_destruir_dato_t(void*);

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
	hash_destruir_dato__t destruir_dato;
} hash_t;


campo_t* campo_crear(char* clave, void* dato){
	campo_t* campo = malloc(sizeof(campo_t));
	if (!campo) return NULL;
	campo->clave = clave;
	campo->dato = dato;
	campo->estado = OCUPADO;
	return campo;
}

hash_t* hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if (!hash) return NULL;
	campo_t** campos = malloc(sizeof(campo_t*)*TAM_INICIAL);
	if (!campos){
		free(hash);
		return NULL;
	}
	hash->campos = campos;
	hash->ocupados = 0;
	hash->vacios = TAM_INICIAL;
	hash->borrados = 0;
	hash->capacidad = TAM_INICIAL;
	hash->destruir_dato = destruir_dato
	return hash;1
}
//Busca un elemento en el hash, devuelve su posicion, si no esta devuelve -1.
int _hash_buscar(hash_t* hash, char* clave){
	uint32_t pos = funcion_hash(clave,strlen(clave));
	pos %= hash->capacidad;
	while (hash->campos[pos]){
		campo_t* campo = hash->campos[pos];
		if (campo->estado == OCUPADO && !strcmp(campo->clave,clave)) return pos;
		pos++;
		pos %= hash->capacidad;
	}
	return -1;
}

bool redimensionar(hash_t* hash,int portentaje){
	campo_t** campos_nuevos = malloc(sizeof(campo_t*)*hash->capacidad*porcentaje);
	if (!campos_nuevos) return false;
	for (int i=0;i<hash->capacidad;i++){
		campo_t* campo = hash->campos[i];
		if (campo && campo->estado == OCUPADO){
			uint32_t pos = funcion_hash(campo->clave,strlen(campo->clave));
			pos %= (hash->capacidad*porcentaje);
			campos_nuevos[pos] = campo;
		}
	} 
	hash->campos = campos_nuevos;
	hash->capacidad *= porcentaje;
	hash->borrados = 0;
	hash->vacios = hash->capacidad - hash->ocupados;
	return true;

}
bool hash_guardar(hash_t* hash, const char* clave, void* dato){
	campo_t* campo = campo_crear(clave,dato);
	if (!campo) return false;
	if ((hash->ocupados + hash->borrados)/hash->capacidad > FACTOR_DE_REDIMENSION){
		bool ok = redimensionar(hash,2);
		if (!ok) return false;
	} 
	int esta = hash_buscar(hash,clave);
	if (esta != -1){
		campo_t* a_eliminar = hash->campos[esta];
		hash->campos[esta] = campo;
		destruir_campo(a_eliminar);
		return true;
	}
	uint32_t posicion = funcion_hash(clave,strlen(clave));
	posicion %= hash->capacidad;
	
	while(hash->campos[posicion]){
		posicion++;
		posicion %= hash->capacidad;
	}
	hash->campos[posicion] = campo;
	hash->ocupados ++;
	hash->vacios --;
	return true;
}

void* hash_obtener(const hash_t* hash, const char* clave){
	int pos = _hash_buscar(hash,clave);
	if (pos == -1) return NULL;
	return (hash->campos[pos])->dato;
}

