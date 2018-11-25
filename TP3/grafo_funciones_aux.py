import math
import heapq
from queue import Queue
from grafo import *

#BFS

def bfs(grafo, origen):
	visitados = set()
	padres = {}
	orden = {}
	cola = Queue()
	visitados.add(origen)
	padres[origen] = None
	orden[origen] = 0
	cola.put(origen)

	while not cola.empty():
		v = cola.get()
		for w in grafo.adyacentes(v):
			if w not in visitados:
				visitados.add(w)
				padres[w] = v
				orden[w] = orden[v] + 1
				cola.put(w)

	return padres,orden


#DFS

def _dfs(grafo,v,visitados,padres,orden):
	visitados.add(v)
	for w in grafo.adyacentes(v):
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

	return padres,orden


#Orden topoloigico

def orden_topologico(grafo):
	grados = {}
	for v in grafo: 
		grado[v] = 0

	for v in grafo:
		for w in grafo.adyacentes(v):
			grados[w] += 1

	cola = Queue()
	for v in grafo:
		if grados[v] == 0 : 
			cola.put(v)

	orden_topologico = []

	while not cola.empty():
		v = cola.get()
		orden_topologico.append(v)

	for w in grafo.adyacentes(v):
		grados[w] -= 1
		if grado[w] == 0 : 
			cola.put(w)

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

def camino_minimo(grafo, origen,precio_tiempo):#, destino):
	distancia = {}
	padres = {}

	if precio_tiempo == 2: #Quiero por cantidad de vuelos al año, cuantos mas, mejor
		dist = math.inf
		dist2 = 0
	else:
		dist = 0
		dist2 = math.inf

	for v in grafo: 
		distancia[v] = dist2

	distancia[origen] = dist
	padres[origen] = None
	heap = []

	heapq.heappush(heap,(origen,distancia[origen]))

	while len(heap) != 0:
		v,distancia = heapq.heappop(heap)
		#if(v == destino): return reconstruir_camino(origen,destino,padre)

		for w in grafo.adyacentes(v):
			if (precio_tiempo == 2 and distancia + grafo.ver_peso(v,w)[precio_tiempo] > distancia[w]) \
			or (precio_tiempo != 2 and distancia + grafo.ver_peso(v,w)[precio_tiempo] < distancia[w]):
				padres[w] = v
				distancia[w] = distancia + grafo.ver_peso(v,w)[precio_tiempo]
				heapq.heappush(heap,(w,distancia[w]))

	return padres,distancia



#	A partir de aquí la cosa empieza a patinar feo	#

#Betweeness centrality (Forma supuestamente optimizada)

def filtrar_infinitos(distancia):
	for w in distancia:
		if (distancia[w] == math.inf):
			distancia.pop(w)

def seg_elemento(tupla):
	return tupla[1]
	

def betweeness_centrality(grafo):
	centralidad = {}

	for v in grafo:
		if w[1] == 0: #Me aseguro que no va a haber ciudades con centralidad
			continue
		centralidad[v] = 0

	for v in grafo:
		if v[1] == 0:
			continue
		padres,distancia = camino_minimo(grafo,v,2)
		centralidad_auxiliar = {}

		for w in grafo:
			if v[1] == 0:
				continue 
			centralidad_auxiliar[w] = 0

		filtrar_infinitos(distancia)

		vertices = list(distancia.values())
	
		vertices_ordenados = sorted(vertices,key = seg_elemento)

		for w in vertices_ordenados:
			if w[1] == 0:
				continue
			centralidad_auxiliar[padre[w]] += centralidad_auxiliar[w] + 1

		for w in grafo:
			if w == v or w[1] == 0: 
				continue
			centralidad[w] += centralidad_auxiliar[w]

	return centralidad

