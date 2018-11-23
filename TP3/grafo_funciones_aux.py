import math

#BFS

def bfs(grafo, origen):
	visitados = set()
	padres = {}
	orden = {}
	cola = colaueue()
	visitados.add(origen)
	padres[origen] = None
	orden[origen] = 0

	while len(cola) != 0:
		v = cola.get()
		for w in grafo:
			if w not in visitados:
				visitados.add(w)
				padres[w] = v
				orden[w] = orden[v] + 1
				cola.put(w)

	return padres_orden #No se como devolver ambos, ayuda


#DFS

def _dfs(grafo,v,visitados,padres,orden):
	visitados.add(v)
	for w in grafo.adjacentes[v]: #Ni idea de como hacer esto
		if w not in visitados:
				padres[w] = v
				orden[w] = orden[v] + 1
				_dfs(grafo,w,visitados,padres,orden)


def dfs(grafo,origen):
	visitados = set()
	padres = {}
	orden = {}
	padres[origen] = None
	orden[origen] = 0

	_dfs(grafo,origen,visitados,padres,orden)

	return padres_orden #No se como devolver ambos, ayuda


#Orden topoloigico

def orden_topologico(grafo):
	grados = {}
	for v in grafo: grado[v] = 0

	for v in grafo:
		for w in grafo.adjacentes[v]: #Ni idea de como hacer esto
			grados[w] += 1

	cola = cola()
	for v in grafo:
		if grados[v] == 0 : cola.put(v)

	orden_topologico = {}

	while len(cola) != 0:
		v = cola.get()
		orden_topologico.append(v)

	for w in grafo.adjacentes[v]: #Ni idea de como hacer esto
		grados[w] -= 1
		if grado[w] == 0 : cola.put(w)

	if(len(orden_topologico) == len(grafo)):
		return orden_topologico
	else:
		return None


#Dijsktra

def reconstuir_camino(padres,destino):
	recorrido = [destino]
	
	while padres[destino] != None:
		destino = padres[destino]
		recorrido.append(destino)

	return recorrido

def camino_minimo(grafo, origen):#, destino):
	distancia = {}
	padres = {}

	for v in grafo: distancia[v] = math.inf
	distancia[origen] = 0
	padres[origen] = None
	heap = heap()

	heap.heappush(origen,distancia[origen])

	while len(heap) != 0:
		v = heap.heappop()
		#if(v == destino): return reconstruir_camino(origen,destino,padre)

		for w in grafo.adjacentes[v]: #Ni idea de como hacer esto
			if distancia[v] + grafo.ver_peso(v,w): #Dependerá de tu implementacion
				padre[w] = v
				heap.heappush(w,distancia[w])

	return padres_distancia #No se como devolver ambos, ayuda



#	A partir de aquí la cosa empieza a patinar feo	#

#Betweeness centrality (Forma supuestamente optimizada)

def filtrar_infinitos(lista):
	for w in lista:
		if (lista[w] == math.inf):
			lista.pop(w)

def ordenar_vertices(grafo, distancia):
	pass

def centralidad_intermediacion(grafo):
	centralidad = {}

	for v in grafo: centralidad[v] = 0

	for v in grafo:
		padres_distancia = camino_minimo(grafo,v)
		centralidad_auxiliar = {}

	for v in grafo: centralidad_auxiliar[v] = 0
	filtrar_infinitos(distancia)
	
	vertices_ordenados = ordenar_vertices(grafo, distancia) #???

	for w in vertices_ordenados:
		if (w == v) : continue
		centralidad_auxiliar[w] += 1
		centralidad_auxiliar[padre[w]] += centralidad_auxiliar[w]

	for w in grafo:
		if (w == v) : continue
		centralidad[w] += centralidad_auxiliar[w]

	return centralidad