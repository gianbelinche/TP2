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

// -_-_-_-_-_-_-_-_-_-_-_-_-_-_  TIPOS DE DATO  _-_-_-_-_-_-_-_-_-_-_-_-_-_- //

typedef struct vuelo{
	char* codigo;
	char* fecha;
	char* resumen;
	size_t prioridad;

}vuelo_t;

struct vuelos_en_rango{
	lista_t* vuelos;
	bool (*insertar) (lista_t*, void*);
	char* fecha_max;
	char* fecha_min;
	
};


// -_-_-_-_-_-_-_-_-_-_-_-_-_-_      MACROS    _-_-_-_-_-_-_-_-_-_-_-_-_-_- //

typedef bool (*comando_t)(abb_t*,hash_t*,char**);

bool agregar_archivo (abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes);
bool ver_tablero     (abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes);
bool info_vuelo      (abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes);
bool prioridad_vuelos(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes);
bool borrar          (abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes);

const int       COMANDOS_CANTIDAD    = 5;
const char*     COMANDOS_NOMBRES[]   = {"agregar_archivo","ver_tablero","info_vuelo","prioridad_vuelos","borrar"};
const comando_t COMANDOS_FUNCIONES[] = {agregar_archivo  , ver_tablero , info_vuelo , prioridad_vuelos , borrar };

#define NOTACION_ASCENDENTE "asc"
#define NOTACION_DESCENDENTE "desc"

// -_-_-_-_-_-_-_-_-_-_-  FUNCIONES AUXILIARES  -_-_-_-_-_-_-_-_-_-_- //

time_t convertir_a_time(const char* fecha)
{
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
	tiempo.tm_wday = 0;
    tiempo.tm_yday = 0;
    tiempo.tm_isdst = 0;
	return mktime(&tiempo);

}

int comparar_fechas(const char* fecha1,const char* fecha2)
{
	return convertir_a_time(fecha1) - convertir_a_time(fecha2);
}

int strcmp_min(const void* dato1,const void* dato2)
{
	return -strcmp((char*) dato1,(char*) dato2);
}

int strcmp_max(const void* dato1,const void* dato2)
{
	return strcmp((char*) dato1,(char*) dato2);
}

bool obtener_vuelos_en_rango(const char* fecha_actual, void* vuelo_actual, void* rango){
	struct vuelos_en_rango vuelos_en_rango = *((struct vuelos_en_rango*) (rango));

	if( comparar_fechas(fecha_actual,vuelos_en_rango.fecha_min) >= 0
		&& comparar_fechas(fecha_actual,vuelos_en_rango.fecha_max) <= 0 ){
		vuelos_en_rango.insertar(vuelos_en_rango.vuelos,vuelo_actual);
		return true;
	}

	return false;
}

void insertar_lista_en_heap(lista_t* lista, heap_t* heap)
{
	lista_iter_t* lista_iter  = lista_iter_crear(lista);
	if(!lista_iter) return;

	void* codigo_actual;
	
	while(!lista_iter_al_final(lista_iter))
	{
		codigo_actual = lista_iter_ver_actual(lista_iter);
		heap_encolar(heap,codigo_actual);
		lista_iter_avanzar(lista_iter);
	}

	lista_iter_destruir(lista_iter);
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

void destruir_lista_fechas(void* lista)
{
	lista_destruir(lista,NULL);
}

bool leer_vuelo(FILE* archivo,vuelo_t* vuelo_actual)
{
	char** linea = malloc(sizeof(char**));
	*linea = NULL;
	size_t tam_linea = 0;

	if(getline(linea,&tam_linea,archivo) == -1 || tam_linea <= 1)
	{
		free(*linea);
		free(linea);
		return false;
	}

	char** linea_separada     = split(*linea,',');

	if(!*linea_separada)
	{
		free_strv(linea_separada);
		free(*linea);
		free(linea);
		return false;
	}

	vuelo_actual -> codigo    = strdup(linea_separada[0]);

	if(!vuelo_actual -> codigo)
	{
		free_strv(linea_separada);
		free(*linea);
		free(linea);
		return false;
	}

	vuelo_actual -> prioridad = atoi(linea_separada[5]);
	vuelo_actual -> fecha     = strdup(linea_separada[6]);

	if(!vuelo_actual -> fecha)
	{
		free(vuelo_actual -> codigo);
		free_strv(linea_separada);
		free(*linea);
		free(linea);
		return false;
	}

	vuelo_actual -> resumen   = join(linea_separada,' ');
	
	free(*linea);
	free(linea);
	free_strv(linea_separada);
	return true;
}

void destruir_vuelo(void* vuelo)
{
	free(((vuelo_t*)vuelo) -> codigo);
	free(((vuelo_t*)vuelo) -> fecha);
	free(((vuelo_t*)vuelo) -> resumen);
	free(((vuelo_t*)vuelo));
}

// -_-_-_-_-_-_-_-_-_-_-   FUNCIONES  PRINCIPALES  -_-_-_-_-_-_-_-_-_-_- //

void interpretar_comando(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char* ordenes[])
{
	for(size_t i = 0; i < COMANDOS_CANTIDAD; i++)
		if(!strcmp(ordenes[0],COMANDOS_NOMBRES[i]))
		{
			if(COMANDOS_FUNCIONES[i](vuelos_x_fecha,vuelos_x_codigo,ordenes))
				puts("OK");
			else
				fprintf(stderr,"Error en comando %s\n",COMANDOS_NOMBRES[i]);

			return;
		}

	fprintf(stderr,"Error en comando %s\n",ordenes[0]);
}

int main()
{
	abb_t* vuelos_x_fecha = abb_crear(comparar_fechas,destruir_lista_fechas);

	if(!vuelos_x_fecha) return -1;

	hash_t* vuelos_x_codigo = hash_crear(destruir_vuelo);

	if(!vuelos_x_codigo)
	{
		abb_destruir(vuelos_x_fecha);
		return -1;
	}

	char** entrada = malloc(sizeof(char**));

	if(!entrada)
	{
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
	abb_destruir(vuelos_x_fecha);
	hash_destruir(vuelos_x_codigo);
	return 0;
}


// -_-_-_-_-_-_-_-_-_-_-_-_-_   COMANDOS   _-_-_-_-_-_-_-_-_-_-_-_-_- //


bool agregar_archivo(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes)
{
	FILE* archivo = fopen(ordenes[1],"r");
	if(!archivo) return false;

	vuelo_t* vuelo_previo;
	vuelo_t* vuelo_actual = malloc(sizeof(vuelo_t));
	if(!vuelo_actual)
	{
		fclose(archivo);
		return false;
	}

	lista_t* codigos_asosiados = NULL;

	while(leer_vuelo(archivo, vuelo_actual))
	{
		vuelo_previo = hash_obtener(vuelos_x_codigo,vuelo_actual -> codigo);
		
		if(vuelo_previo)
		{
			codigos_asosiados = abb_obtener(vuelos_x_fecha,vuelo_previo -> fecha);
			borrar_codigo_en_lista(codigos_asosiados,vuelo_previo -> codigo);
			
			if(lista_esta_vacia(codigos_asosiados))
				lista_destruir(abb_borrar(vuelos_x_fecha,vuelo_previo -> fecha),NULL);

			destruir_vuelo(vuelo_previo);
		}

		codigos_asosiados = abb_obtener(vuelos_x_fecha,vuelo_actual -> fecha);

		if(!codigos_asosiados)
		{
			codigos_asosiados = lista_crear();
			
			if(!codigos_asosiados)
			{
				free(vuelo_actual);
				fclose(archivo);
				return false;
			}

			abb_guardar(vuelos_x_fecha,vuelo_actual -> fecha,codigos_asosiados);
		}

		lista_insertar_ultimo(codigos_asosiados, vuelo_actual -> codigo);
		hash_guardar(vuelos_x_codigo, vuelo_actual -> codigo,vuelo_actual);

		vuelo_actual = malloc(sizeof(vuelo_t));

		if(!vuelo_actual)
		{
			fclose(archivo);
			return false;
		}
	}

	free(vuelo_actual);
	fclose(archivo);
	return true;
}



bool ver_tablero(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes){
	if(hash_cantidad(vuelos_x_codigo) == 0) return true;
	
	int contador = atoi(ordenes[1]);
	if(contador <= 0) return false;

	if(!strcmp(ordenes[2],NOTACION_ASCENDENTE) && !strcmp(ordenes[2],NOTACION_DESCENDENTE)) return false;

	struct vuelos_en_rango vuelos_en_rango;

	vuelos_en_rango.vuelos = lista_crear();
	if(!vuelos_en_rango.vuelos) return false;

	vuelos_en_rango.insertar = (!strcmp(ordenes[2],NOTACION_ASCENDENTE)) ? lista_insertar_ultimo : lista_insertar_primero;
	vuelos_en_rango.fecha_min = ordenes[3];
	vuelos_en_rango.fecha_max = ordenes[4];
	
	if(comparar_fechas(vuelos_en_rango.fecha_min,vuelos_en_rango.fecha_max) > 0)
	{
		lista_destruir(vuelos_en_rango.vuelos,NULL);
		return false;
	}

	abb_in_order(vuelos_x_fecha, obtener_vuelos_en_rango,&vuelos_en_rango);
	
	heap_t* heap = heap_crear((!strcmp(ordenes[2],NOTACION_ASCENDENTE)) ? strcmp_min : strcmp_max);
	vuelo_t* vuelo_actual;
	
	while(!lista_esta_vacia(vuelos_en_rango.vuelos) && contador > 0)
	{
		insertar_lista_en_heap(lista_borrar_primero(vuelos_en_rango.vuelos),heap);

		while(!heap_esta_vacio(heap))
		{
			vuelo_actual = hash_obtener(vuelos_x_codigo, heap_desencolar(heap));
			printf("%s - %s\n",vuelo_actual -> fecha,vuelo_actual -> codigo);	
		}

		contador--;
	}

	lista_destruir(vuelos_en_rango.vuelos,NULL);
	heap_destruir(heap,NULL);
	return true;
}

bool borrar(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes){
	if(hash_cantidad(vuelos_x_codigo) == 0) return true;
	struct vuelos_en_rango vuelos_en_rango;

	vuelos_en_rango.vuelos = lista_crear();
	if(!vuelos_en_rango.vuelos) return false;

	vuelos_en_rango.insertar = lista_insertar_ultimo;
	vuelos_en_rango.fecha_min = ordenes[1];
	vuelos_en_rango.fecha_max = ordenes[2];
	
	if(comparar_fechas(vuelos_en_rango.fecha_min,vuelos_en_rango.fecha_max) > 0)
	{
		lista_destruir(vuelos_en_rango.vuelos,NULL);
		return false;
	}

	abb_in_order(vuelos_x_fecha, obtener_vuelos_en_rango,&vuelos_en_rango);
	lista_t* codigos_asosiados;
	char* codigo_actual;
	char* fecha_actual;

	while(!lista_esta_vacia(vuelos_en_rango.vuelos))
	{

		codigos_asosiados = lista_borrar_primero(vuelos_en_rango.vuelos);
		codigo_actual     = lista_borrar_primero(codigos_asosiados);
		fecha_actual      = strdup(((vuelo_t*)hash_obtener(vuelos_x_codigo,codigo_actual)) -> fecha);
		destruir_vuelo(hash_borrar(vuelos_x_codigo,codigo_actual));

		while(!lista_esta_vacia(codigos_asosiados))
		{
			destruir_vuelo(hash_borrar(vuelos_x_codigo,lista_borrar_primero(codigos_asosiados)));
		}

		abb_borrar(vuelos_x_fecha,fecha_actual);
		free(fecha_actual);
	}

	lista_destruir(vuelos_en_rango.vuelos,NULL);
	return true;
}

bool info_vuelo(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes)
{
	if(hash_cantidad(vuelos_x_codigo) == 0) return true;
	vuelo_t* info = hash_obtener(vuelos_x_codigo,ordenes[1]);
	if (!info) return false;

	printf("%s",info -> resumen);
	return true;
}

bool prioridad_vuelos(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes)
{
	if(hash_cantidad(vuelos_x_codigo) == 0) return true;

	heap_t* heap_min = heap_crear(strcmp_min);

	if(!heap_min) return false;

	hash_iter_t* iter = hash_iter_crear(vuelos_x_codigo);

	if(!iter)
	{
		heap_destruir(heap_min,NULL);
		return false;
	}

	size_t cantidad_a_mostrar = atoi(ordenes[1]);
	size_t contador = 0;

	while (!hash_iter_al_final(iter) && contador < cantidad_a_mostrar)
	{
		heap_encolar(heap_min, (char*) hash_iter_ver_actual(iter));
		hash_iter_avanzar(iter);
		contador++;
	}

	vuelo_t* vuelo_min;
	vuelo_t* vuelo_actual;

	while (!hash_iter_al_final(iter))
	{
		vuelo_min    = hash_obtener(vuelos_x_codigo, heap_ver_max(heap_min));
		vuelo_actual = hash_obtener(vuelos_x_codigo, (char*) hash_iter_ver_actual(iter));

		if (vuelo_actual -> prioridad < vuelo_min -> prioridad)
		{
			heap_desencolar(heap_min); 
			heap_encolar(heap_min,vuelo_actual -> codigo);
		}
		hash_iter_avanzar(iter);
	}

	hash_iter_destruir(iter);

	char* vuelos_x_prioridad[cantidad_a_mostrar];
	
	for(int i = 0;i < cantidad_a_mostrar;i++)
	{
		vuelos_x_prioridad[i] = heap_desencolar(heap_min);
	}

	for (;cantidad_a_mostrar > 0;cantidad_a_mostrar--)
	{
		vuelo_actual = hash_obtener(vuelos_x_codigo,vuelos_x_prioridad[cantidad_a_mostrar]);
		printf("%d - %s\n",vuelo_actual -> prioridad, vuelo_actual -> codigo);
	}

	heap_destruir(heap_min,NULL);
	return true;
}