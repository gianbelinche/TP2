//Información de vuelos

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "hash.h"
#include "heap.h"
/*
hash_t* guardar_info_vuelos(FILE* vuelos,size_t cantidad_vuelos){ //Despues veremos como guardamos los vuelos
	hash_t* hash = hash_crear(NULL);
	for (int i = 0;i<cantidad_vuelos;i++){
		char* info = NULL;
		size_t cantidad = 0;
		if (getline(vuelos,info,cantidad) == -1) break;
		char** nombre = split(info);
		hash_guardar(hash,nombre[0],info);
	}
	return hash;
}
*/

typedef struct vuelo_prioridad{
	char* codigo;
	int prioridad;
}vp_t;

int cmp(const void* dato1, const void* dato2){
	*(vp_t*) dato1; //MI no entender porque no funcionar
	*(vp_t*) dato2;
	if (dato1->prioridad < dato2->prioridad) return 1; //Nuestro heap es de maximos, lo queremos de minimos
	else if (dato1->prioridad > dato2->prioridad) return -1;
	return -strcmp(dato1->codigo,dato2->codigo);
} 

void obtener_info_vuelos(hash_t* info_vuelos,char* vuelo){
	char* info = hash_obtener(info_vuelos,vuelo);
	if (!info) 
		fprintf(stderr,"Error en comando info_vuelo\n");
	else
		printf("%s\n",info);
}

void prioridad_vuelos_v1(heap_t* prioridades,int k){
	vp_t* vuelos[k];
	int j = 0;
	for(int i;i<k;i++){
		if (heap_esta_vacio(prioridades)) break;
		vp_t* vuelo = heap_desencolar(prioridades);
		printf("%d - %s\n",vuelo -> prioridad, vuelo -> codigo ); //O(n*log(n))
		vuelos[i] = vuelo;
		j++;
	}
	for (int i = 0;i<j;i++){
		heap_encolar(prioridades,vuelos[i]);
	}
}

void prioridad_vuelos_v2(vp_t** vuelos,int cant,int k){
	heap_t* heap = heap_crear(cmp); //Heap de minimos
	int i = 0;
	for (;i<k && i<cant;i++){
		heap_encolar(heap,vuelos[i]);
	}
	for (;i<cant;i++){
		if (heap_ver_max(heap)->prioridad < vuelos[i]->prioridad){ //O(n*log(k))
			heap_desencolar(heap); 
			heap_encolar(heap,vuelos[i]);
		}
	}
	vp_t* flights[k];
	int j = 0;
	while (!heap_esta_vacio(heap)){
		vp_t* vuelo = heap_desencolar(heap);
		flights[j] = vuelo;
		j++;
	}
	for (int l = j;l >= 0;l--){
		printf("%d - %s\n",flights[l] -> prioridad, flights[l] -> codigo );
	}
}