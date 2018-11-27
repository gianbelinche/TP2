from grafo import *
from grafo_funciones_aux import *

grafo = Grafo()
vertices = ["A","B","C","D","E","F","G"]

for v in vertices:
	grafo.agregar_vertice(v)

grafo.agregar_arista("A","B",0)
grafo.agregar_arista("B","C",0)
grafo.agregar_arista("B","E",0)
grafo.agregar_arista("C","D",0)
grafo.agregar_arista("D","E",0)
grafo.agregar_arista("E","F",0)
grafo.agregar_arista("F","G",0)
grafo.agregar_arista("G","A",0)


def reconstruir_camino(padres,destino):
	if (destino == None): return None
	recorrido = [destino]
	
	while padres[destino] != None:
		destino = padres[destino]
		recorrido.append(destino)

	return recorrido

def mostrar_grafo(grafo):
	for v in grafo:
		for w in grafo.adyacentes(v):
			print("{} - {} - {}".format(v,grafo.ver_peso(v,w),w)) 

def _viaje_n_lugares(grafo,distancia,visitados,padres,origen, v, escalas,escalas_restantes):
	visitados.add(v)
	print(v)

	if (escalas_restantes > 0):
		for w in grafo.adyacentes(v):
			if (w not in visitados) and (escalas_restantes - distancia[w] >= 0):
				padres[w] = v
				resultado = _viaje_n_lugares(grafo,distancia,visitados,padres,origen,w,escalas,escalas_restantes - 1)
				if(resultado != None): return resultado

	elif (distancia[v] == 1): return v;

	visitados.remove(v)
	return None

def calcular_distancia(grafo,origen,escalas):
	visitados = set()
	visitados.add(origen)
	distancia = {}
	distancia[origen] = 0
	cola = Queue()
	cola.put(origen)

	while not cola.empty():
		v = cola.get()
		if(distancia[v] <= escalas/2):
			for w in grafo.adyacentes(v):
				if w not in visitados:
					visitados.add(w)
					distancia[w] = distancia[v] + 1
					cola.put(w)
	return distancia

def viaje_n_lugares(grafo,origen,escalas):
	visitados = set()
	visitados.add(origen)
	distancia = calcular_distancia(grafo,origen,escalas)
	padres = {}
	padres[origen] = None

	final = _viaje_n_lugares(grafo,distancia,visitados,padres,origen,origen,escalas,escalas)

	if (final == None):
		print("No se encontro recorrido")
	else:
		print(reconstruir_camino(padres,final))


viaje_n_lugares(grafo,"A",4)
