#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "lista.h"
#include "hash.h"
#include "abb.h"
#include "strutil.h"
#include "heap.h"

// -_-_-_-_-_-_-_-_-_-_-_-_-_-_  MACROS  _-_-_-_-_-_-_-_-_-_-_-_-_-_- //

#define FORMATO_ARCHIVO "%zi, %zi, %[^,], %[^,], %[^,], %zi, %[^,], %i, %i, %[^\n]\n"

typedef struct vuelo{
	char* codigo;
	size_t aerolinea;
	char*  puerto_origen;
	char*  puerto_destino;
	char*  numero_de_cola;
	size_t prioridad;
	char* fecha;
	int retraso;
	int tiempo_de_vuelo;
	bool cancelado;
	char* resumen;
}vuelo_t;

typedef bool (*comando_t)(abb_t*,hash_t*,char**);

bool agregar_archivo (abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes);
bool ver_tablero     (abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes);
bool info_vuelo      (abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes);
bool prioridad_vuelos(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes);
bool borrar          (abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes);

const int       COMANDOS_CANTIDAD    = 5;
const char*     COMANDOS_NOMBRES[]   = {"agregar_archivo","ver_tablero","info_vuelo","prioridad_vuelos","borrar"};
const comando_t COMANDOS_FUNCIONES[] = {agregar_archivo  , ver_tablero , info_vuelo , prioridad_vuelos , borrar };

// -_-_-_-_-_-_-_-_-_-_-   FUNCIONE  PRINCIPAL  -_-_-_-_-_-_-_-_-_-_- //

bool interpretar_comando(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char* ordenes[])
{
	for(size_t i = 0; i < COMANDOS_CANTIDAD; i++)
		if(!strcmp(ordenes[0],COMANDOS_NOMBRES[i]))
			return COMANDOS_FUNCIONES[i](vuelos_x_fecha,vuelos_x_codigo,ordenes);

	return false;
}

int comparar_fechas(const char* dato1,const char* dato2);

int main()
{
	abb_t* vuelos_x_fecha = abb_crear(comparar_fechas);

	if(!vuelos_x_fecha) return -1;

	hash_t* vuelos_x_codigo = hash_crear(destruir_vuelo);

	if(!vuelos_x_codigo){
		abb_destruir(vuelos_x_fecha);
		return -1;
	}

	char** entrada = malloc(sizeof(char**));

	if(!entrada){
		abb_destruir(vuelos_x_fecha);
		hash_destruir(vuelos_x_codigo);
		return -1;
	}

	*entrada = NULL;
	size_t tam = 0;

	char** strv0; char** strv;
	
	while(getline(entrada,&tam,stdin) > 0)
	{
		if(**entrada == '\n')
			continue;

		strv0 = split(*entrada,'\n');
		strv = split(*strv0,' ');

		interpretar_comando(vuelos_x_fecha,vuelos_x_codigo,strv);

		free_strv(strv0);
		free_strv(strv);
	}
	free(*entrada);
	free(entrada);
	return 0;
}

// -_-_-_-_-_-_-_-_-_-_-  FUNCIONES AUXILIARES  -_-_-_-_-_-_-_-_-_-_- //

time_t convertir_a_time(const char* fecha){
	struct tm tiempo;
	char ano[] = {fecha[0],fecha[1],fecha[2],fecha[3],'\0'}; //NO entiendo porque no funciona
	tiempo.tm_year = atoi(ano) - 1900;
	char mes[] = {fecha[5],fecha[6],'\0'};
	tiempo.tm_mon = atoi(mes) -1;
	char dia[] = {fecha[8],fecha[9],'\0'};
	tiempo.tm_mday = atoi(dia);
	char hora[] = {fecha[11],fecha[12],'\0'};
	tiempo.tm_hour = atoi(hora);
	char minutos[] = {fecha[14],fecha[15],'\0'};
	tiempo.tm_min = atoi(minutos);
	char seg[] = {fecha[17],fecha[18],'\0'};
	tiempo.tm_sec = atoi(seg);
	return mktime(&tiempo);

}

int comparar_fechas(const char* fecha1,const char* fecha2){

	time_t tiempo1 = convertir_a_time(fecha1);
	time_t tiempo2 = convertir_a_time(fecha2);

	return tiempo1 - tiempo2;

}

int mi_strcmp(const void* dato1,const void* dato2){
	return -strcmp((char*) dato1,(char*) dato2);
}

char* obtener_codigo_en_lista(lista_t* lista, char* codigo)
{
	lista_iter_t* lista_iter  = lista_iter_crear(lista);
	if(!lista_iter) return NULL;

	void* codigo_actual;
	
	while(!lista_iter_al_final(lista_iter))
	{
		codigo_actual = lista_iter_ver_actual(lista_iter);
		if(!strcmp(codigo_actual,codigo)) break;

		lista_iter_avanzar(lista_iter);
	}

	lista_iter_destruir(lista_iter);

	return (!strcmp(codigo_actual,codigo)) ? codigo_actual : NULL;
}

void borrar_codigo_en_lista(lista_t* lista, char* codigo)
{
	lista_iter_t* lista_iter  = lista_iter_crear(lista);
	if(!lista_iter) return;

	void* codigo_actual;
	
	while(!lista_iter_al_final(lista_iter))
	{
		codigo_actual = lista_iter_ver_actual(lista_iter);
		if(!strcmp(codigo_actual,codigo))
		{
			lista_iter_borrar(lista_iter);
			break;
		}

		lista_iter_avanzar(lista_iter);
	}

	lista_iter_destruir(lista_iter);
}

int leer_vuelo(FILE* archivo,vuelo_t* vuelo_actual){
	/* Esto tengo que arreglarlo dependiendo de que agamos
	return fscanf(archivo,FORMATO_ARCHIVO, vuelo_actual -> codigo, vuelo_actual -> aerolinea, vuelo_actual -> puerto_origen
		, vuelo_actual -> puerto_destino, vuelo_actual -> numero_de_cola, vuelo_actual -> prioridad, vuelo_actual -> fecha
		, vuelo_actual -> retraso, vuelo_actual -> tiempo_de_vuelo, vuelo_actual -> cancelado);
		*/
	return -1;
}

// -_-_-_-_-_-_-_-_-_-_-_-_-_   COMANDOS   _-_-_-_-_-_-_-_-_-_-_-_-_- //

bool agregar_archivo(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes){

	FILE* archivo = fopen(ordenes[1],"r");
	if(!archivo) return false;

	vuelo_t* vuelo_previo;
	vuelo_t* vuelo_actual = malloc(sizeof(vuelo_t));
	if(!vuelo_actual){
		fclose(archivo);
		return false;
	}

	lista_t* codigos_asosiados;

	while(leer_vuelo(archivo, vuelo_actual) != EOF) {

		vuelo_previo = hash_obtener(vuelos_x_codigo,vuelo_actual -> codigo);
		codigos_asosiados = abb_obtener(vuelos_x_fecha,vuelo_previo -> fecha);

		if(vuelo_previo && strcmp(vuelo_previo -> fecha,vuelo_actual -> fecha)){
			borrar_codigo_en_lista(codigos_asosiados,vuelo_previo -> codigo);
			if(lista_esta_vacia(codigos_asosiados))
			{
				lista_destruir(abb_borrar(vuelos_x_fecha,vuelo_previo -> fecha),NULL);
				codigos_asosiados = NULL;
			}
		}

		if(!codigos_asosiados)
		{
			codigos_asosiados = lista_crear();
			
			if(!codigos_asosiados){
			free(vuelo_actual);
			fclose(archivo);
			return false;
			}

			abb_guardar(vuelos_x_fecha,vuelo_actual -> fecha,codigos_asosiados);
		}

		lista_insertar_ultimo(codigos_asosiados, vuelo_actual -> codigo);
		hash_guardar(vuelos_x_codigo, vuelo_actual -> codigo,vuelo_actual);

		vuelo_actual = malloc(sizeof(vuelo_t));
		if(!vuelo_actual){
		fclose(archivo);
		return false;
		}
		
	}

	fclose(archivo);
	return true;
}

struct vuelos_en_rango{
	lista_t* vuelos;
	bool orden_ascendente;
	char* fecha_max;
	char* fecha_min;
	
};

bool obtener_vuelos_en_rango(const char* fecha_actual, void* vuelo_actual, void* rango){
	//Si falla hay algo turbio aca
	struct vuelos_en_rango vuelos_en_rango = *((struct vuelos_en_rango*) (rango));

	if( comparar_fechas(fecha_actual,vuelos_en_rango.fecha_min) > 0
		&& comparar_fechas(fecha_actual,vuelos_en_rango.fecha_max) < 0 ){

		if(vuelos_en_rango.orden_ascendente)
			lista_insertar_ultimo(vuelos_en_rango.vuelos,vuelo_actual);
		else
			lista_insertar_primero(vuelos_en_rango.vuelos,vuelo_actual);
		return true;
	}

	return false;
}

bool ver_tablero(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes){

	struct vuelos_en_rango vuelos_en_rango;

	vuelos_en_rango.vuelos = lista_crear();
	if(!vuelos_en_rango.vuelos) return false;

	vuelos_en_rango.orden_ascendente = atoi(ordenes[2]);
	vuelos_en_rango.fecha_min = ordenes[3];
	vuelos_en_rango.fecha_max = ordenes[4];
	
	abb_in_order(vuelos_x_fecha, obtener_vuelos_en_rango,&vuelos_en_rango);

	int contador = atoi(ordenes[1]);
	while(!lista_esta_vacia(vuelos_en_rango.vuelos) && contador > 0)
	{
		vuelo_t vuelo_actual = lista_borrar_primero(hash_obtener(vuelos_x_codigo,vuelos_en_rango.vuelos));
		printf("%s - %s\n",vuelo_actual.fecha,vuelo_actual.codigo);
		contador--;
	}

	lista_destruir(vuelos_en_rango.vuelos,NULL);
	return true;
}


bool info_vuelo(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes){
	vuelo_t* info = hash_obtener(vuelos_x_codigo,ordenes[1]);
	if (!info){ 
		fprintf(stderr,"Error en comando info_vuelo\n");
		return false;
	}	
	printf("%s\n",info->resumen);
	return true;
}

bool prioridad_vuelos(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes){
	heap_t* heap = heap_crear(mi_strcmp); //Heap de minimos
	hash_iter_t* iter = hash_iter_crear(vuelos_x_codigo);
	int k = atoi(ordenes[1]);
	int i = 0;
	while (!hash_iter_al_final(iter) && i < k){
		heap_encolar(heap,(char*) hash_iter_ver_actual(iter));
		i++;
		hash_iter_avanzar(iter);
	}
	while (!hash_iter_al_final(iter)){
		char* codigo_max = heap_ver_max(heap);
		char* actual = (char*) hash_iter_ver_actual(iter);
		vuelo_t* vuelo_max = hash_obtener(vuelos_x_codigo,codigo_max);
		vuelo_t* vuelo_actual = hash_obtener(vuelos_x_codigo,actual);
		if (vuelo_max->prioridad < vuelo_actual->prioridad){ //O(n*log(k))
			heap_desencolar(heap); 
			heap_encolar(heap,actual);
		}
		hash_iter_avanzar(iter);
	}
	char* flights[k];
	int j = 0;
	while (!heap_esta_vacio(heap)){
		char* vuelo = heap_desencolar(heap);
		flights[j] = vuelo;
		j++;
	}
	for (int l = j;l >= 0;l--){
		vuelo_t* vuelo = hash_obtener(vuelos_x_codigo,flights[l]);
		printf("%d - %s\n",vuelo -> prioridad, flights[l]);
	}
	return true;
}

void avanzar_hasta(abb_iter_t* iter,char* desde){
	while (!abb_iter_in_al_final(iter)){
		char* fecha_actual = (char*) abb_iter_in_ver_actual(iter);
		if (comparar_fechas(fecha_actual,desde) > 0)
			return;
		abb_iter_in_avanzar(iter);
	}
}

bool borrar(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes){

	char* desde = ordenes[1];
	char* hasta = ordenes[2];

	lista_t* lista = lista_crear();
	if (!lista) return false;



	abb_iter_t* iter = abb_iter_in_crear(vuelos_x_fecha);

	if (!iter){
		free(lista);
		return false;
	}

	avanzar_hasta(iter,desde);

	
	while (!abb_iter_in_al_final(iter)){
		char* fecha = (char*) abb_iter_in_ver_actual(iter);
		if (comparar_fechas(fecha,hasta) > 0)
			break;
		lista_t* codigos = abb_obtener(vuelos_x_fecha,fecha);
		while(!lista_esta_vacia(codigos)){
			char* codigo = lista_borrar_primero(codigos);
			hash_borrar(vuelos_x_codigo,codigo);
		}
		lista_insertar_ultimo(lista,fecha);
		abb_iter_in_avanzar(iter);
	}

	while (!lista_esta_vacia(lista)){
		abb_borrar(vuelos_x_fecha,lista_borrar_primero(lista));
	}

	lista_destruir(lista,NULL);
	abb_iter_in_destruir(iter);
	return true;
}