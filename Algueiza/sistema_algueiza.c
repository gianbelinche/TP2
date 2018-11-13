#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "lista.h"
#include "heap.h"
#include "abb.h"

// -_-_-_-_-_-_-_-_-_-_-_-_-_-_  MACROS  _-_-_-_-_-_-_-_-_-_-_-_-_-_- //

typedef struct fecha{
	char* gian;
}fecha_t;

#define FORMATO_VUELO "%zi, %zi, %[^,], %[^,], %[^,], %zi, fecha, %i, %i, %[^\n]\n"

typedef struct vuelo{
	size_t numero_de_vuelo;
	size_t aerolinea;
	char*  puerto_origen;
	char*  puerto_destino;
	char*  numero_de_cola;
	size_t prioridad;
	fecha_t fecha;
	int retraso;
	int tiempo_de_vuelo;
	bool cancelado;
}vuelo_t;

// -_-_-_-_-_-_-_-_-_-_-  FUNCIONES AUXILIARES  -_-_-_-_-_-_-_-_-_-_- //

int comparar_fecha(fecha_t fecha1 , fecha_t fecha2)
{
	return -1;
}

// -_-_-_-_-_-_-_-_-_-_-    FUNCION PRINCIPAL   -_-_-_-_-_-_-_-_-_-_- //

void agregar_archivo(abb_t* vuelos_x_fecha,heap_t* vuelos_x_codigo,char* nombre_archivo)
{
	FILE* archivo = fopen(nombre_archivo,"r");

	if(!archivo) return;

	vuelo_t* vuelo_actual = malloc(sizeof(vuelo_t));

	if(!vuelo_actual) return;

	while(fscanf(archivo,FORMATO_VUELO	, vuelo_actual -> numero_de_vuelo, vuelo_actual -> aerolinea, vuelo_actual -> puerto_origen
		, vuelo_actual -> puerto_destino, vuelo_actual -> numero_de_cola, vuelo_actual -> prioridad, vuelo_actual -> fecha
		, vuelo_actual -> retraso, vuelo_actual -> tiempo_de_vuelo, vuelo_actual -> cancelado) != EOF) {

		abb_guardar(vuelos_x_fecha,vuelo_actual -> fecha,vuelo_actual);
		hash_guardar(vuelos_x_codigo, vuelo_actual -> codigo,vuelo_actual);

		vuelo_t* vuelo_actual = malloc(sizeof(vuelo_t));

		if(!vuelo_actual) return;
	}

	fclose(archivo);
}


struct vuelos_en_rango{
	lista_t* vuelos;
	fecha_t fecha_max;
	fecha_t fecha_min; 
};

bool obtener_vuelos_en_rango(const char* fecha_actual, void* vuelo_actual, void* rango)
{
	if( comparar_fecha(fecha_actual,((vuelos_en_rango*) rango).fecha_min) > 0
		&& comparar_fecha(fecha_actual,((vuelos_en_rango*) rango).fecha_max) < 0 ){
		lista_insertar_ultimo(((vuelos_en_rango*) rango).vuelos,vuelo_actual);
		return true;
	}

	return false;
}

void mostrar_tablero(abb_t* vuelos_x_fecha,size_t cantidad_a_mostrar,bool ascendente,fecha_t fecha_min, fecha_t fecha_max)
{
	struct vuelos_en_rango vuelos_en_rango;
	vuelos_en_rango.vuelos = lista_crear();
	if(!vuelos_en_rango.vuelos) return;

	vuelos_en_rango.fecha_min = fecha_min;
	vuelos_en_rango.fecha_max = fecha_max;

	abb_pre_order(vuelos, obtener_vuelos_en_rango,&vuelos_en_rango);

	(bool) *borrar (lista_t* lista)  = (ascendente) ? lista_borrar_primero : lista_borrar_ultimo;

	while(!lista_esta_vacia(vuelos_en_rango.vuelos))
	{
		vuelo_t vuelo_actual = borrar(vuelos_en_rango.vuelos);
		printf("%s - %zi\n",vuelo_actual.fecha,vuelo_actual.codigo);
	}

	lista_destruir(vuelos_en_rango.vuelos,NULL);
}