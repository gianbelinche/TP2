//Información de vuelos

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

void obtener_info_vuelos(hash_t* hash,char* vuelo){
	char* info = hash_obtener(hash,vuelo);
	if (!info) 
		printf("Error en comando info_vuelo\n",stderr);
	else
		printf("%s\n",info);
}