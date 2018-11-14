#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "lista.h"
#include "hash.h"
#include "abb.h"
#include "strutil.h"

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

int comparar_fecha(fecha_t fecha1 , fecha_t fecha2)
{
	return -1;
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
	fecha_t fecha_max;
	fecha_t fecha_min;
	
};

bool obtener_vuelos_en_rango(const char* fecha_actual, void* vuelo_actual, void* rango){

	if( comparar_fecha(fecha_actual,((vuelos_en_rango*) rango).fecha_min) > 0
		&& comparar_fecha(fecha_actual,((vuelos_en_rango*) rango).fecha_max) < 0 ){
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


int cmp(const void* dato1, const void* dato2){ //Estaria bueno un nombre mas descriptivo que cmp
	vp_t* vuelo1 = (vp_t*) dato1; 
	vp_t* vuelo2 = (vp_t*) dato2;
	if (vuelo1->prioridad < vuelo2->prioridad) return 1; //Nuestro heap es de maximos, lo queremos de minimos
	else if (vuelo1->prioridad > vuelo2->prioridad) return -1;
	return -strcmp(vuelo1->codigo,vuelo2->codigo);
} 

bool info_vuelo(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes){
	char* info = hash_obtener(vuelos_x_codigo,ordenes[1]);
	if (!info) 
		fprintf(stderr,"Error en comando info_vuelo\n");
	else
		printf("%s\n",info);//Aca va a ser necesario interpretar un poco mas lo que tiene el hash
}


// Aca estás suponiendo que te llega una dupla de vuelos y prioridad, todo joya pero de donde las sacas?
// En mi opinion ese heap deberia recorrer directamente el hash y ahi si queres guardar la dupla

bool prioridad_vuelos(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes)
bool prioridad_vuelos(vp_t** vuelos,int cant,int k){
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


bool borrar(abb_t* vuelos_x_fecha,hash_t* vuelos_x_codigo,char** ordenes){

	time_t desde = convertir_a_time(ordenes[1]);
	time_t hasta = convertir_a_time(ordenes[2]);
	
	lista_t* lista = lista_crear();
	abb_iter_t* iter = abb_iter_in_crear(vuelos_x_fecha);
	abb_iter_in_llegar_a(iter,desde);

	//Faltan un monton de comprobaciones por NULL;
	
	while (!abb_iter_in_al_final(iter)){
		char* codigo = abb_iter_in_ver_actual(iter);
		if (convertir_a_time(abb_obtener(vuelos_x_fecha,codigo)) > hasta) // Esto hay que cambiarlo
			break;
		hash_borrar(vuelos_x_codigo,codigo);
		lista_insertar_ultimo(lista,codigo);
		abb_iter_in_avanzar(iter);
	}
	while (!lista_esta_vacia(lista)){
		abb_borrar(vuelos_x_fecha,lista_borrar_primero(lista));
	}

	lista_destruir(lista,NULL);
	abb_iter_in_destruir(iter);
}