#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// -_-_-_-_-_-_-_-_  DEFINICION DE  TIPOS DE DATO  _-_-_-_-_-_-_-_-_- //

typedef struct vec_union{
	char   sep;
	char*  vec_union;
	size_t pos_vec_union;
}vec_union_t;

// -_-_-_-_-_-_-_-_-_-_-  FUNCIONES AUXILIARES  -_-_-_-_-_-_-_-_-_-_- //

void iterar_strv(char* strv[],void funcion(void* posicion, void* extra),void* extra)
{
	char*  substr  = strv[0];
	size_t pos_substr, pos_strv = 0;

	while(substr)
	{
		pos_substr = 0;
		while(substr[pos_substr] != '\0')
		{
			funcion(&substr[pos_substr],extra);
			pos_substr++;
		}
		funcion(&substr[pos_substr],extra);
		pos_strv++;
		substr  = strv[pos_strv];
	}
}

void _contar_caracteres(void* caracter,void* cantidadidad_caracteres)
{
	(*((size_t*)cantidadidad_caracteres))++;
}

size_t contar_caracteres(char* strv[])
{
	if(!strv)
		return 0;

	size_t cantidad_caracteres = 0;
	iterar_strv(strv,_contar_caracteres,&cantidad_caracteres);
	return cantidad_caracteres;
}

size_t contar_separaciones(const char* str,char sep)
{
	size_t pos_str = 0;
	size_t cantidad_separaciones = 1;

	while(str[pos_str] != '\0')
	{
		if(str[pos_str] == sep)
			cantidad_separaciones++;
		pos_str++;
	}

	return cantidad_separaciones;
}

// -_-_-_-_-_-_-_-_-_-_-   FUNCIONES PRINCIPALES  -_-_-_-_-_-_-_-_-_- //

void free_strv(char* strv[])
{
	if(!strv)
		return;
	
	int i = 0;

	while(strv[i] != NULL)
	{
		free(strv[i]);
		i++;
	}

	free(strv);
}

char** split(const char* str, char sep)
{
	char** strv = malloc(sizeof(char**)*(contar_separaciones(str,sep) + 1));

	if(!strv)
		return NULL;

	if(sep == '\0')
	{
		strv[0] = strdup(str);
		strv[1] = NULL;
		return strv;
	}

	size_t pos_strv = 0;
	
	const char*  substr = str; 
	size_t pos_substr = 0;

	while(substr[pos_substr] != '\0')
	{
		if(substr[pos_substr] == sep)
		{
			strv[pos_strv] = strndup(substr,pos_substr);

			if(!strv[pos_strv])
			{
				free_strv(strv);
				return NULL;
			}
			pos_strv++;
			substr += pos_substr + 1;
			pos_substr = 0;
		}
		else
		{
			pos_substr++;
		}
	}

	strv[pos_strv] = strndup(substr,pos_substr);

	if(!strv[pos_strv])
	{
		free_strv(strv);
		return NULL;
	}

	strv[pos_strv + 1] = NULL;

	return strv;
}

void _join(void* caracter,void* vec_unido)
{
	vec_union_t* _vec_unido = (vec_union_t*) vec_unido;

	if( *((char*) caracter)  != '\0')
	_vec_unido -> vec_union[ _vec_unido -> pos_vec_union] = *((char*) caracter);
	else
	_vec_unido -> vec_union[ _vec_unido -> pos_vec_union] = _vec_unido -> sep;

	(_vec_unido -> pos_vec_union)++;
}

char* join(char* strv[],char sep)
{
	vec_union_t vec_unido;
	vec_unido.vec_union = malloc(sizeof(char)*(contar_caracteres(strv) + 1));
	vec_unido.pos_vec_union = 0;
	vec_unido.sep = sep;

	if(!(vec_unido.vec_union))
		return NULL;

	if(!strv || !strv[0])
	{
		vec_unido.vec_union[vec_unido.pos_vec_union] = '\0';
		return vec_unido.vec_union;
	}

	iterar_strv(strv,_join,&vec_unido);
	vec_unido.vec_union[vec_unido.pos_vec_union - 1] = '\0';

	return vec_unido.vec_union;
}