//Información de vuelos

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "hash.h"
#include "heap.h"
#include "abb.h"
#include "lista.h"
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
	vp_t* vuelo1 = (vp_t*) dato1; 
	vp_t* vuelo2 = (vp_t*) dato2;
	if (vuelo1->prioridad < vuelo2->prioridad) return 1; //Nuestro heap es de maximos, lo queremos de minimos
	else if (vuelo1->prioridad > vuelo2->prioridad) return -1;
	return -strcmp(vuelo1->codigo,vuelo2->codigo);
} 

void obtener_info_vuelos(hash_t* info_vuelos,char* vuelo){
	char* info = hash_obtener(info_vuelos,vuelo);
	if (!info) 
		fprintf(stderr,"Error en comando info_vuelo\n");
	else
		printf("%s\n",info);
}


void prioridad_vuelos(vp_t** vuelos,int cant,int k){
	heap_t* heap = heap_crear(cmp); //Heap de minimos
	int i = 0;
	for (;i<k && i<cant;i++){
		heap_encolar(heap,vuelos[i]);
	}
	for (;i<cant;i++){
		if (((vp_t*)heap_ver_max(heap))->prioridad < vuelos[i]->prioridad){ //O(n*log(k))
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

time_t convertir_a_time(char* fecha){
	struct tm tiempo;
	char* año = {fecha[0],fecha[1],fecha[2],fecha[3],'\0'}; //NO entiendo porque no funciona
	tiempo.tm_year = atoi(año) - 1900;
	char* mes = {fecha[5],fecha[6],'\0'};
	tiempo.tm_mon = atoi(mes) -1;
	char* dia = {fecha[8],fecha[9],'\0'};
	tiempo.tm_mday = atoi(dia);
	char* hora = {fecha[11],fecha[12],'\0'};
	tiempo.tm_hour = atoi(hora);
	char* minutos = {fecha[14],fecha[15],'\0'};
	tiempo.tm_min = atoi(minutos);
	char* seg = {fecha[17],fecha[18],'\0'};
	tiempo.tm_sec = atoi(seg);
	return mktime(&tiempo);

}
void borrar(abb_t* abb,hash_t* hash,char* desde,char* hasta){
	time_t s_desde = convertir_a_time(desde);
	time_t s_hasta = convertir_a_time(hasta);
	lista_t* lista = lista_crear();
	abb_iter_t* iter = abb_iter_in_crear(abb);
	abb_iter_in_llegar_a(iter,s_desde);
	while (!abb_iter_in_al_final(iter)){
		char* codigo = abb_iter_in_ver_actual(iter);
		if (convertir_a_time(abb_obtener(abb,codigo)) > s_hasta)
			break;
		hash_borrar(hash,codigo);
		lista_insertar_ultimo(lista,codigo);
		abb_iter_in_avanzar(iter);
	}
	while (!lista_esta_vacia(lista)){
		abb_borrar(abb,lista_borrar_primero(lista));
	}
	lista_destruir(lista,NULL);
	abb_iter_in_destruir(iter);
}