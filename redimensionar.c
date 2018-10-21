bool redimensionar(hash_t* hash, int escalar){
	campo_t** nuevos_campos = calloc(hash -> capacidad * escalar,sizeof(campo_t*));
	uint32_t nueva_pos;

	for(size_t i = 0; i < hash -> capacidad;i++)
	{
		if(hash -> campos[i])
		{
			if(hash -> campos[i] -> estado == OCUPADO)
			{
				nueva_pos = funcion_hash(hash -> campos[i] -> clave,strlen(hash -> campos[i] -> clave)) % (hash->capacidad*escalar);
				nuevos_campos[nueva_pos] = hash -> campos[i];
			}
			else
			{
				campo_destruir(hash -> campos[i],NULL);
			}
		}
	}

	free(hash -> campos);
	hash -> campos = nuevos_campos;
	hash -> capacidad *= escalar;
	hash -> borrados = 0;
	hash -> vacios = hash -> ocupados - hash -> vacios;
	return true;
}