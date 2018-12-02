from grafo import *
from grafo_funciones_aux import *
import sys
import copy
import math
import random
#................................  CLASES ................................#

class Conjunto_Disjunto:
	def __init__(self,lista):
		self.conjuntos = {}
		for v in lista:
			self.conjuntos[v] = v

	def find(self,v):
		if self.conjuntos[v] == v:
			return v
		w = self.find(self.conjuntos[v])
		self.conjuntos[v] = w
		return w

	def union(self,v,w):
		self.conjuntos[self.find(w)] = v



class _IterGrafo_Dir:
	def __init__ (self,cantidad,lista):
		self.cantidad = cantidad
		self.pos = 0
		self.lista = lista

	def __next__(self):
		if self.pos >= self.cantidad:
			raise StopIteration()
		self.pos += 1
		return self.lista[self.pos-1]


class Grafo_Dir:
	
	def __init__ (self):
		self.datos = {}
		self.cantidad = 0

	def agregar_vertice(self,v):
		if v in self.datos:
			return False
		self.datos[v] = {}
		self.cantidad += 1
		return True
		
	def agregar_arista(self,v,w):
		if v not in self.datos or w not in self.datos:
			return False
		self.datos[v][w] = True
		return True	

	def adyacentes(self,v):
		return list(self.datos.get(v,[]))
	
	def __len__ (self):
		return self.cantidad

	def __iter__ (self):
		return _IterGrafo_Dir(self.cantidad,list(self.datos))

#................................  FUNCIONES AUXILIARES ................................#

def crear_grafo(archivo1,archivo2):
	grafo = Grafo()
	aeropuertos_a_ciudades = {}
	ciudades_a_aeropuertos = {}
	coordenadas = {}
	with open(archivo1) as aeropuertos, open(archivo2) as vuelos:
		for linea in aeropuertos:
			elementos = linea.rstrip("\n").split(",")
			ciudad = elementos[0]
			aeropuerto = elementos[1] 
			grafo.agregar_vertice(aeropuerto)
			ciudades_a_aeropuertos[ciudad] = ciudades_a_aeropuertos.get(ciudad,[]) + [aeropuerto]
			aeropuertos_a_ciudades[aeropuerto] = ciudad 
			coordenada = (elementos[2],elementos[3])
			coordenadas[ciudad] = coordenada

		for linea in vuelos:
			elementos = linea.rstrip("\n").split(",")
			aeropuerto_i = elementos[0]
			aeropuerto_j = elementos[1]
			tiempo = elementos[2]
			precio = elementos[3]
			vuelos_x_año = elementos[4]
			peso = (int(tiempo),int(precio),1/int(vuelos_x_año))
			grafo.agregar_arista(aeropuerto_i,aeropuerto_j,peso)

	return grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos,coordenadas		

def mostrar_grafo(grafo):
	for v in grafo:
		for w in grafo.adyacentes(v):
			print("{} - {} - {}".format(v,grafo.ver_peso(v,w),w)) 


def obtener_aristas(grafo):
	aristas = {}
	for v in grafo:
		for w in grafo.adyacentes(v):
			peso = grafo.ver_peso(v,w)
			if (w,v,peso) not in aristas and (v,w,peso) not in aristas:
				aristas[(v,w,peso)] = True

	return aristas

def ord_pesos(arista):
	return arista[2][1]

def ordenar_aristas(aristas):
	return sorted(aristas.keys(),key=ord_pesos)


def es_camino_valido(recorrido,grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos):
	ciudades = list(ciudades_a_aeropuertos.keys())
	for v in recorrido:
		try:
			ciudades.remove(aeropuertos_a_ciudades[v])
		except:
			pass

	anterior = recorrido[0]
	for v in recorrido[1:]:
		if not grafo.estan_conectados(anterior,v):
			return False
		anterior = v
				
	return len(ciudades) == 0	

def ver_costo_recorrido(recorrido,grafo):
	costo = 0
	anterior = recorrido[0]
	for v in recorrido[1:]:
		costo += grafo.ver_peso(anterior,v)[0]
		anterior = v
	return costo

def jajaja(recorrido,grafo):
	anterior = recorrido[0]
	for v in recorrido[1:]:
		if not grafo.estan_conectados(anterior,v):
			return False
		anterior = v
	return True		

def me_conviene(recorrido,grafo,minimo):
	costo = ver_costo_recorrido(recorrido,grafo)
	return costo < minimo[0]
	


def falso_dfs(grafo,v,recorrido,rec_finales,aeropuertos_a_ciudades,ciudades_a_aeropuertos,minimo,anterior,anterior2):
	for w in grafo.adyacentes(v):
		recorrido.append(w)
		if not me_conviene(recorrido,grafo,minimo) or (w == anterior and v == anterior2):
			recorrido.pop()	
		elif es_camino_valido(recorrido,grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos):
			rec_finales.append(recorrido[:])
			minimo[0] = ver_costo_recorrido(recorrido,grafo)
			recorrido.pop()
		else:			
			falso_dfs(grafo,w,recorrido,rec_finales,aeropuertos_a_ciudades,ciudades_a_aeropuertos,minimo,v,anterior)
			recorrido.pop()	

def esquematizar_recorrido(recorrido):
	s = ""
	for punto in recorrido:
		s += "{} -> ".format(punto)
	return s[:-4]

def calcular_distancia(grafo,origen,escalas):
	visitados = set()
	visitados.add(origen)
	distancia = {}
	distancia[origen] = 0
	cola = Queue()
	cola.put(origen)

	while not cola.empty():
		v = cola.get()
		if(distancia[v] <= escalas):
			for w in grafo.adyacentes(v):
				if w not in visitados:
					visitados.add(w)
					distancia[w] = distancia[v] + 1
					cola.put(w)
	return distancia

#................................       COMANDOS        ................................#

def listar_operaciones():
	print("camino_escalas")         #1
	print("camino_mas")             #1
	print("centralidad")            #3
	print("centralidad_aproximada") #1
	print("pagerank")               #2
	print("nueva_aerolinea")        #2
	print("recorrer_mundo")         #1
	print("recorrer_mundo_aprox")   #1
	print("vacaciones")             #3
	print("itinerario")             #2
	print("exportar_kml")           #1

def camino_escalas(grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos,origen,destino):
	caminos = []
	for aeropuerto_a in ciudades_a_aeropuertos[origen]:
		padres,orden = bfs(grafo,aeropuerto_a)
		for aeropuerto_b in ciudades_a_aeropuertos[destino]:
			camino = []
			actual = aeropuerto_b
			i = 0
			while actual:
				camino.append(actual)
				actual = padres[actual]
				i += 1
			caminos.append((camino[:],i))	

	maxi = math.inf
	cam = []		
	for camino in caminos:
		if camino[1] < maxi:
			maxi = camino[1]
			cam = camino[0]			
				
	print(esquematizar_recorrido(cam[::-1]))			

	return cam[::-1]

def camino_mas(grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos,modo,origen,destino): #parece funcionar
	recorridos = []
	for aeropuerto in ciudades_a_aeropuertos[origen]:
		padres,distancias = camino_minimo(grafo,aeropuerto,modo)
		for aeropuerto_b in ciudades_a_aeropuertos[destino]:
			camino = []
			actual = aeropuerto_b
			while actual:
				camino.append(actual)
				actual = padres[actual]
			recorridos.append((camino,distancias[aeropuerto_b]))

	mini = math.inf
	rec = []
	for camino in recorridos:
		if camino[1] < mini:
			mini = camino[1]
			rec = camino[0]			
	
		rec2 = []	

	s = ""	
	while len(rec) != 0:
		s += "{} -> ".format(rec[-1])
		rec2.append(rec.pop())

	
	print(s[:-4])
	
	return rec2,mini

def betweeness_centrality_aprox(grafo):
	centralidad = {}

	for v in grafo:
		centralidad[v] = 0

	for i in range(1,int(len(grafo))):
		cantidad = int(len(grafo))
		v = grafo.vertice_random()

		while cantidad > 0:
			centralidad[v] += 1
			v = random.choice(grafo.adyacentes(v))
			cantidad -= 1

	return centralidad


def ha_convergido(grafo,v_totales,centralidad,centralidad_pasada):
	convergieron = 0

	for v in grafo:
		if (abs(centralidad[v] - centralidad_pasada[v]) <= 0.01):
			convergieron += 1
		else:
			print( "{},{}".format(v,abs(centralidad[v] - centralidad_pasada[v])))

	if(convergieron/v_totales >= 0.99):
		return True
	else:
		return False

def pagerank(grafo):
	centralidad_pasada = {}
	centralidad = {}
	v_totales = 0

	for v in grafo:
		centralidad[v] = 0.25
		v_totales += 1
	
	for v in grafo:
		centralidad[v] = (0.69/v_totales)
		for w in grafo.adyacentes(v):
			centralidad[v] += 0.31 *(centralidad[v]/len(grafo.adyacentes(w)))

	while True:
		centralidad_pasada = copy.deepcopy(centralidad)
		for v in grafo:
			centralidad[v] = (0.69/v_totales)
			for w in grafo.adyacentes(v):
				centralidad[v] += 0.31 *(centralidad[v]/len(grafo.adyacentes(w)))
		
		if(ha_convergido(grafo,v_totales,centralidad,centralidad_pasada)):
			break

		

	return centralidad

def centralidad(grafo,n,modo):
	modos = {
		1:betweeness_centrality,
		2:betweeness_centrality_aprox,
		3:pagerank,
	}
	centralidad = modos.get(modo)(grafo)
	valores = list(centralidad.items())
	valores.sort(key=seg_elemento,reverse=True)
	s = ""
	for i in range(n):
		s += "{}, ".format(valores[i][0])
	print(s[:-2])	

def nueva_aerolinea(archivo,grafo):

	lista = []
	for v in grafo:
		lista.append(v)

	conjunto_disjunto = Conjunto_Disjunto(lista)
	aristas = ordenar_aristas(obtener_aristas(grafo))

	rutas_a_devolver = []

	for arista in aristas:
		v,w,peso = arista

		if(Conjunto_Disjunto.find(conjunto_disjunto, w) != Conjunto_Disjunto.find(conjunto_disjunto, v)):
			rutas_a_devolver.append(arista)
			Conjunto_Disjunto.union(conjunto_disjunto, w, v)

	with open(archivo, "w") as salida:
		for ruta in rutas_a_devolver:
			v,w,peso = ruta
			salida.write("{},{},{},{},{}\n".format(v,w,peso[0],peso[1], int(1/peso[2])))

	print("OK")



def recorrer_mundo_aprox(grafo,ciudades_a_aeropuertos,aeropuertos_a_ciudades,origen,imprimir):
	recorridos = []
	for aeropuerto in ciudades_a_aeropuertos[origen]:
		recorrido = []
		no_visitadas = {}
		for ciudad in ciudades_a_aeropuertos:
			no_visitadas[ciudad] = True
		no_visitadas.pop(aeropuertos_a_ciudades[aeropuerto])	
		actual = aeropuerto
		costo = 0
		while len(no_visitadas) != 0:
			caminos = []
			hay = False
			prox = None
			mini = math.inf
			for ady in grafo.adyacentes(actual):
				if aeropuertos_a_ciudades[ady] in no_visitadas and (prox == None or grafo.ver_peso(actual,ady)[0] < mini):
					hay = True
					prox = ady
					mini = grafo.ver_peso(actual,ady)[0]
			if not hay:	
				padres,distancias = camino_minimo(grafo,actual,0)	
				for ciudad in no_visitadas:	
					for arop in ciudades_a_aeropuertos[ciudad]:
						este_camino = []
						actual = arop
						while actual:
							este_camino.append(actual)
							actual = padres[actual]
						caminos.append((este_camino[::-1],distancias[arop]))
			else:
				caminos.append(([prox],mini))
						
			minimo = min(caminos,key=seg_elemento)
			actual = minimo[0][-1]
			costo += minimo[1]
			for aero in minimo[0]:
				if len(recorrido) > 0 and recorrido[-1] == aero:
					continue
				recorrido.append(aero)
				try:
					no_visitadas.pop(aeropuertos_a_ciudades[aero])
				except:
					pass
		recorridos.append((recorrido,costo))

	minimo = math.inf
	cam = []
	for camino in recorridos:
		if camino[1] < minimo:
			cam = camino[0]
			minimo = camino[1]

	if imprimir:
		print(esquematizar_recorrido(cam))	
		print("Costo: {}".format(minimo))


	return cam,minimo	

def recorrer_mundo(grafo,ciudades_a_aeropuertos,aeropuertos_a_ciudades,origen):
	rec_finales = []
	rec_parcial,minimo = recorrer_mundo_aprox(grafo,ciudades_a_aeropuertos,aeropuertos_a_ciudades,origen,False)
	for aeropuerto in ciudades_a_aeropuertos[origen]:
		recorrido =  []
		recorrido.append(aeropuerto)
		falso_dfs(grafo,aeropuerto,recorrido,rec_finales,aeropuertos_a_ciudades,ciudades_a_aeropuertos,[minimo],None,None)

	maxi = math.inf
	rec =  []	

	for recorrido in rec_finales:
		costo = ver_costo_recorrido(recorrido,grafo)
		if costo < maxi:
			maxi = costo
			rec = recorrido

	print(esquematizar_recorrido(rec))
	print(maxi)
	return rec	

def _viaje_n_lugares(grafo,distancia,visitados,padres,origen, v, escalas,escalas_restantes):
	visitados.add(v)

	if (escalas_restantes > 0):
		for w in grafo.adyacentes(v):
			if (w not in visitados) and (escalas_restantes - distancia[w] >= 0):
				padres[w] = v
				resultado = _viaje_n_lugares(grafo,distancia,visitados,padres,origen,w,escalas,escalas_restantes - 1)
				if(resultado != None): return resultado

	elif (distancia[v] == 1): return v

	visitados.remove(v)
	return None



def viaje_n_lugares(grafo,ciudades_a_aeropuertos,aeropuertos_a_ciudades,origen,escalas):
	for origenx in ciudades_a_aeropuertos[origen]:
		visitados = set()
		visitados.add(origenx)
		distancia = calcular_distancia(grafo,origenx,escalas) #Acota la cantidad de vertices a considerar
		padres = {}
		padres[origenx] = None

		final = _viaje_n_lugares(grafo,distancia,visitados,padres,origenx,origenx,escalas,escalas)

		if (final != None):
			camino_aeropuertos = reconstruir_camino(padres,final)
			camino_aeropuertos.append(origenx)
			print(esquematizar_recorrido(camino_aeropuertos))	
			return camino_aeropuertos
	
	print("No se encontro recorrido")

def exportar_KML(archivo, aeropuertos_a_ciudades,coordenadas, recorrido):
	ciudades_registradas = {}
	ciudad_previa = None
	with open(archivo, "w") as salida:
		salida.write( '<?xml version="1.0" encoding="UTF-8"?>')
		salida.write(' <kml xmlns="http://www.opengis.net/kml/2.2">')
		salida.write('     <Document>')

		es_ciudad = False
		try: 
			aeropuertos_a_ciudades[recorrido[0]]
		except:
			es_ciudad = True

		for v in recorrido:
			if es_ciudad:
				ciudad = v
			else:
				ciudad = aeropuertos_a_ciudades[v]
			if (ciudad not in ciudades_registradas):
				ciudades_registradas[ciudad] = True
				salida.write('     <Placemark>')
				salida.write('     	<name> {} </name>'.format(ciudad))
				salida.write('     		<point>')
				salida.write('  			<coordinates> {},{} </coordinates>'.format(coordenadas[ciudad][1],coordenadas[ciudad][0]))
				salida.write('     		</point>')
				salida.write('     </Placemark>')

			if(ciudad_previa != None):
				salida.write('     <Placemark>')
				salida.write('     		<LineString>')
				salida.write('  			<coordinates> {},{} {},{} </coordinates>'.format(coordenadas[ciudad_previa][1],coordenadas[ciudad_previa][0],coordenadas[ciudad][1],coordenadas[ciudad][0]))
				salida.write('     		</LineString>')
				salida.write('     </Placemark>')

			ciudad_previa = ciudad

		salida.write('     </Document>')
		salida.write(' </kml>')
		print("OK")



def crear_grafo_dir(archivo):
	grafo = Grafo_Dir()
	with open(archivo) as archivo:
		linea = archivo.readline()
		ciudades = linea.rstrip("\n").split(",")
		for ciudad in ciudades:
			grafo.agregar_vertice(ciudad)
		for linea in archivo:
			elementos = linea.rstrip("\n").split(",")
			grafo.agregar_arista(elementos[0],elementos[1])
	return grafo
	
def itinerario_cultural(grafo_dir,grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos):
	orden = orden_topologico(grafo_dir)
	for v in orden[:-1]:
		print(v,",",end="")
	print(orden[-1])	
	actual = orden[0]
	for v in orden[1:]:
		camino_escalas(grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos,actual,v)
		actual = v		


#................................    FUNCION PRINCIPAL  ................................#

def separar_parametros(parametros):
	para = []
	comando = ""
	for i in range(len(parametros)):
		if parametros[i] == " ":
			para.append(comando)
			para.append(parametros[i+1:])	
			break
		comando += parametros[i]

	if len(para) == 0:
		para.append(comando)
	return para	
			
def main():
	archivo1 = sys.argv[1]
	archivo2 = sys.argv[2]
	grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos,coordenadas = crear_grafo(archivo1,archivo2)
	ultimo_comando = []
	while True:
		try:
			comando = input()
		except EOFError:
			break	

		parametros = separar_parametros(comando.rstrip("\n"))
		if parametros[0] == "listar_operaciones":
			listar_operaciones()
		
		elif parametros[0] == "camino_escalas":
			origen,destino = parametros[1].split(",")
			ultimo_comando = camino_escalas(grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos,origen,destino)
		
		elif parametros[0] == "camino_mas":
			para = parametros[1].split(",")
			modo = 0
			if para[0] == "barato":
				modo = 1
			origen = para[1]
			destino = para[2]
			ultimo_comando = camino_mas(grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos,modo,origen,destino)

		elif parametros[0] == "centralidad":
			n = int(parametros[1])
			centralidad(grafo,n,1)
		
		elif parametros[0] == "centralidad_aproximada":
			n = int(parametros[1])
			centralidad(grafo,n,2)
		elif parametros[0] == "pagerank":
			n = int(parametros[1])
			centralidad(grafo,n,3)
		elif parametros[0] == "recorrer_mundo":
			origen = parametros[1]
			ultimo_comando = recorrer_mundo(grafo,ciudades_a_aeropuertos,aeropuertos_a_ciudades,origen)	

		elif parametros[0] == "recorrer_mundo_aprox":
			origen = parametros[1]
			ultimo_comando,minimo = recorrer_mundo_aprox(grafo,ciudades_a_aeropuertos,aeropuertos_a_ciudades,origen,True)
		
		elif parametros[0] == "exportar_kml":
			archivo = parametros[1]
			exportar_KML(archivo,aeropuertos_a_ciudades,coordenadas,ultimo_comando)
		
		elif parametros[0] == "nueva_aerolinea":
			archivo = parametros[1]
			nueva_aerolinea(archivo,grafo)

		elif parametros[0] == "vacaciones":
			para = parametros[1].split(",")
			origen = para[0]
			n = int(para[1])
			ultimo_comando = viaje_n_lugares(grafo,ciudades_a_aeropuertos,aeropuertos_a_ciudades,origen,n)

		elif parametros[0] == "itinerario_cultural":
			archivo = parametros[1]
			grafo_dir = crear_grafo_dir(archivo)
			itinerario_cultural(grafo_dir,grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos)
				

main()

