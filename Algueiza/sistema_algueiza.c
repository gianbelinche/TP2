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



#define FORMATO_VUELO "%zi, %zi, %[^,], %[^,], %[^,], %zi, fecha, %i, %i, %[^\n]\n"

typedef struct vuelo{
	size_t numero_de_vuelo;
	size_t aerolinea;
	char*  puerto_origen;
	char*  puerto_destino;
	char*  numero_de_cola;
	size_t prioridad;
	char* fecha;
	int retraso;
	int tiempo_de_vuelo;
	bool cancelado;
}vuelo_t;

typedef bool (*comando_t)(abb_t*,hash_t*,char**);

bool agregar_archivo (abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes);
bool ver_tablero     (abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes);
bool info_vuelo      (abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes);
bool borrar          (abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes);

const int       COMANDOS_CANTIDAD    = 5;
const char*     COMANDOS_NOMBRES[]   = {"agregar_archivo","ver_tablero","info_vuelo","prioridad_vuelos","borrar"};
const comando_t COMANDOS_FUNCIONES[] = {agregar_archivo  , ver_tablero , info_vuelo , prioridad_vuelos , borrar };

// -_-_-_-_-_-_-_-_-_-_-   FUNCIONE  PRINCIPAL  -_-_-_-_-_-_-_-_-_-_- //

bool interpretar_comando(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char* ordenes[])
{
	for(size_t i = 0, i < COMANDOS_CANTIDAD,i++)
		if(!strcmp(ordenes[0],COMANDOS_NOMBRES[i]))
			return COMANDOS_FUNCIONES[i](vuelos_x_fecha,vuelos_x_codigo,ordenes);

	return false;
}

int main()
{
	abb_t*  vuelos_x_fecha = abb_crear();

	if(!vuelos_x_fecha) return;

	hash_t* vuelos_x_codigo = hash_crear();

	if(!vuelos_x_codigo){
		abb_destruir(vuelos_x_fecha,NULL);
		return;
	}

	char** entrada = malloc(sizeof(char**));

	if(!entrada){
		abb_destruir(vuelos_x_fecha,NULL);
		hash_destruir(vuelos_x_codigo,NULL);
		return;
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



time_t convertir_a_time(char* fecha){
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

int comparar_fechas(const void* dato1,const void* dato2){

	char* fecha1 = (char*) dato1;
	char* fecha2 = (char*) dato2;

	time_t tiempo1 = convertir_a_time(fecha1);
	time_t tiempo2 = convertir_a_time(fecha2);

	return tiempo1 - tiempo2;

}

int mi_strcmp(const void* dato1,const void* dato2){
	return -strcmp((char*) dato1,(char*) dato2);
}
// -_-_-_-_-_-_-_-_-_-_-_-_-_   COMANDOS   _-_-_-_-_-_-_-_-_-_-_-_-_- //

bool agregar_archivo(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes){

	FILE* archivo = fopen(ordenes[1],"r");

	if(!archivo) return false;

	vuelo_t* vuelo_actual = malloc(sizeof(vuelo_t));

	if(!vuelo_actual) return false;

	while(fscanf(archivo,FORMATO_VUELO	, vuelo_actual -> numero_de_vuelo, vuelo_actual -> aerolinea, vuelo_actual -> puerto_origen
		, vuelo_actual -> puerto_destino, vuelo_actual -> numero_de_cola, vuelo_actual -> prioridad, vuelo_actual -> fecha
		, vuelo_actual -> retraso, vuelo_actual -> tiempo_de_vuelo, vuelo_actual -> cancelado) != EOF) {

		abb_guardar(vuelos_x_fecha,vuelo_actual -> fecha,vuelo_actual);
		hash_guardar(vuelos_x_codigo, vuelo_actual -> codigo,vuelo_actual);

		vuelo_t* vuelo_actual = malloc(sizeof(vuelo_t));

		if(!vuelo_actual) return false;
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

	if( comparar_fechas(fecha_actual,((vuelos_en_rango*) rango).fecha_min) > 0
		&& comparar_fechas(fecha_actual,((vuelos_en_rango*) rango).fecha_max) < 0 ){
		if(orden_ascendente)
			lista_insertar_ultimo(((vuelos_en_rango*) rango).vuelos,vuelo_actual);
		else
			lista_insertar_primero(((vuelos_en_rango*) rango).vuelos,vuelo_actual);
		return true;
	}

	return false;
}

bool ver_tablero(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes){

	struct vuelos_en_rango vuelos_en_rango;

	vuelos_en_rango.vuelos = lista_crear();
	if(!vuelos_en_rango.vuelos) return false;

	vuelos_en_rango.orden_ascendente = atoi(ordenes[2]);
	vuelos_en_rango.fecha_min = convertir_a_time(ordenes[3]);
	vuelos_en_rango.fecha_max = convertir_a_time(ordenes[4]);
	
	abb_in_order(vuelos, obtener_vuelos_en_rango,&vuelos_en_rango);

	int contador = atoi(ordenes[1]);
	while(!lista_esta_vacia(vuelos_en_rango.vuelos) && contador > 0)
	{
		vuelo_t vuelo_actual = lista_borrar_primero(vuelos_en_rango.vuelos);
		printf("%s - %zi\n",vuelo_actual.fecha,vuelo_actual.codigo);
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


// Aca estás suponiendo que te llega una dupla de vuelos y prioridad, todo joya pero de donde las sacas?
// En mi opinion ese heap deberia recorrer directamente el hash y ahi si queres guardar la dupla



bool prioridad_vuelos(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes){
	heap_t* heap = heap_crear(mi_strcmp); //Heap de minimos
	hash_iter_t* iter = hash_iter_crear(vuelos_x_codigo);
	int k = atoi(ordenes[1]);
	int i = 0;
	while (!hash_iter_al_final(iter) && i < k){
		heap_encolar(heap,hash_iter_ver_actual(iter));
		i++;
		hash_iter_avanzar(iter);
	}
	while (!hash_iter_al_final(iter)){
		char* codigo_max = heap_ver_max(heap);
		char* actual = hash_iter_ver_actual(iter);
		vuelos_t* vuelo_max = hash_obtener(vuelos_x_codigo,codigo_max);
		vuelos_t* vuelo_actual = hash_obtener(vuelos_x_codigo,actual);
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
		vuelo_t* vuelo = hash_obtener(vuelos_x_codigo,actual);
		printf("%d - %s\n",vuelo -> prioridad, flights[l]);
	}
	return true;
}

void avanzar_hasta(abb_iter_t* iter,char* desde){
	while (!abb_iter_in_al_final(iter)){
		char* fecha_actual = abb_iter_in_ver_actual(iter);
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
		char* fecha = abb_iter_in_ver_actual(iter);
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