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
		for w in grafo:
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

def camino_minimo(grafo, origen):#, destino):
	distancia = {}
	padres = {}

	for v in grafo: 
		distancia[v] = math.inf

	distancia[origen] = 0
	padres[origen] = None
	heap = []

	heapq.heappush(heap,(origen,distancia[origen]))

	while len(heap) != 0:
		v,distancia = heapq.heappop(heap)
		#if(v == destino): return reconstruir_camino(origen,destino,padre)

		for w in grafo.adyacentes(v):
			if distancia + grafo.ver_peso(v,w):
				padres[w] = v
				distancia[w] = distancia + grafo.ver_peso(v,w)
				heapq.heappush(heap,(w,distancia[w]))

	return padres,distancia



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


grafo = Grafo()
grafo.agregar_vertice(1)	
grafo.agregar_vertice(2)
grafo.agregar_vertice(3)
grafo.agregar_arista(1,2,25)
grafo.agregar_arista(1,3,20)
grafo.agregar_arista(2,3,5)

padres,distancia = camino_minimo(grafo,1)
print(padres)
print(distancia)
