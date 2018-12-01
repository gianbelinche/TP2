from grafo import *
from grafo_funciones_aux import *
import sys

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

#................................       COMANDOS        ................................#

def listar_operaciones():
	print("camino_escalas")       #1
	print("camino_mas")           #1
	print("centralidad")          #3
	print("nueva_aerolinea")      #2
	print("recorrer_mundo_aprox") #1
	print("vacaciones")           #3
	print("exportar_kml")         #1

def recorrido_esquematizar(recorrido):
	s = ""
	for punto in recorrido:
		s += "{} -> ".format(punto)
	return s[:-4]

def camino_escalas(grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos,origen,destino):
	caminos = []
	for aeropuerto_a in ciudades_a_aeropuertos[origen]:
		padres,orden = bfs(grafo,aeropuerto_a)
		camino = []
		for aeropuerto_b in ciudades_a_aeropuertos[destino]:
			actual = aeropuerto_b
			i = 0
			while actual:
				camino.append(actual)
				actual = padres[actual]
				i += 1
			caminos.append((camino,i))	

	maxi = 0
	cam = []		
	for camino in caminos:
		if camino[1] > maxi:
			maxi = camino[1]
			cam = camino[0]			
				
	print(recorrido_esquematizar(cam[::-1]))			

	return cam[::-1]

def centralidad(grafo,n):
	centralidad = betweeness_centrality(grafo)
	valores = list(centralidad.items())
	valores.sort(key=seg_elemento,reverse=True)
	s = ""
	for i in range(n):
		s += "{}, ".format(valores[i][0])
	print(s[:-2])	

def recorrer_mundo_aprox(grafo,ciudades_a_aeropuertos,aeropuertos_a_ciudades,origen,imprimir):
	recorridos = []
	for aeropuerto in ciudades_a_aeropuertos[origen]:
		camino = []
		visitados = {}
		precio = 0
		vuelos = len(ciudades_a_aeropuertos) 
		camino.append(aeropuerto)
		cantidad = 1
		visitados[origen] = True
		actual = aeropuerto
		anterior = None
		prox = None	
		contiguos = {}
		for v in grafo:
			contiguos[v] = []

		while cantidad != vuelos:
			mini = math.inf
			for ady in grafo.adyacentes(actual):
				if prox == None or (grafo.ver_peso(actual,ady)[0] < mini and ady != anterior and ady not in contiguos[actual]):
					mini = grafo.ver_peso(actual,ady)[0]
					prox = ady
					
			if prox not in grafo.adyacentes(actual):
				for ady in grafo.adyacentes(actual):
					if prox == None or (grafo.ver_peso(actual,ady)[0] < mini and ady != anterior):
						mini = grafo.ver_peso(actual,ady)[0]
						prox = ady
			contiguos[actual] += [prox]		
			anterior = actual				
			actual = prox
			camino.append(actual)
			precio += mini
			if aeropuertos_a_ciudades[actual] not in visitados:
				visitados[aeropuertos_a_ciudades[actual]] = True
				cantidad += 1
		recorridos.append((camino,precio))	

	minimo = math.inf
	cam = []
	for camino in recorridos:
		if camino[1] < minimo:
			cam = camino[0]
			minimo = camino[1]

	if imprimir:
		print(recorrido_esquematizar(cam))	
		print("Costo: {}".format(minimo))

	return cam,minimo

def camino_mas(grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos,modo,origen,destino):
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

	while len(rec) != 0:
		s += "{} -> ".format(rec[-1])
		rec2.append(rec.pop())

	print(mostrar_recorrido[:-4])

	return rec2
			
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
			ultima_ciudad = None
			camino_ciudades = []

			for aeropuerto in camino_aeropuertos:
				if(ultima_ciudad != aeropuertos_a_ciudades[aeropuerto]):
					ultima_ciudad = aeropuertos_a_ciudades[aeropuerto]
					camino_ciudades.append(ultima_ciudad)

			print(recorrido_esquematizar(camino_ciudades))	
			return camino_ciudades
	
	print("No se encontro recorrido")

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


def exportar_KML(archivo, aeropuertos_a_ciudades,coordenadas, recorrido):
	ciudades_registradas = {}
	ciudad_previa = None
	with open(archivo, "w") as salida:
		salida.write( '<?xml version="1.0" encoding="UTF-8"?>')
		salida.write(' <kml xmlns="http://www.opengis.net/kml/2.2">')
		salida.write('     <Document>')

		for v in recorrido:
			ciudad = v#aeropuertos_a_ciudades[v]
			if (ciudad not in ciudades_registradas):
				ciudades_registradas[ciudad] = True
				salida.write('     <Placemark>')
				salida.write('     	<name> {} </name>'.format(ciudad))
				salida.write('     		<point>')
				salida.write('  			<coordinates> {},{} </coordinates>'.format(coordenadas[ciudad][0],coordenadas[ciudad][1]))
				salida.write('     		</point>')
				salida.write('     </Placemark>')

			if(ciudad_previa != None):
				salida.write('     <Placemark>')
				salida.write('     		<LineString>')
				salida.write('  			<coordinates> {},{} {},{} </coordinates>'.format(coordenadas[ciudad_previa][0],coordenadas[ciudad_previa][1],coordenadas[ciudad][0],coordenadas[ciudad][1]))
				salida.write('     		</LineString>')
				salida.write('     </Placemark>')

			ciudad_previa = ciudad

		salida.write('     </Document>')
		salida.write(' </kml>')
		print("OK")


def es_camino_valido(recorrido,grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos):
	ciudades = list(ciudades_a_aeropuertos.keys())
	for v in recorrido:
		try:
			ciudades.remove(aeropuertos_a_ciudades[v])
		except:
			pass
	return len(ciudades) == 0	

def ver_costo_recorrido(recorrido,grafo):
	costo = 0
	anterior = recorrido[0]
	for v in recorrido[1:]:
		costo += grafo.ver_peso(anterior,v)[0]
		anterior = v
	return costo

def me_conviene(recorrido,grafo,minimo):
	costo = ver_costo_recorrido(recorrido,grafo)
	return costo < minimo[0]


def falso_dfs(grafo,v,recorrido,rec_finales,aeropuertos_a_ciudades,ciudades_a_aeropuertos,minimo):
	for w in grafo.adyacentes(v):
		recorrido.append(w)
		if not me_conviene(recorrido,grafo,minimo):
			recorrido.pop()	
		elif es_camino_valido(recorrido,grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos):
			rec_finales.append(recorrido[:])
			minimo[0] = ver_costo_recorrido(recorrido,grafo)
			recorrido.pop()
		else:			
			falso_dfs(grafo,w,recorrido,rec_finales,aeropuertos_a_ciudades,ciudades_a_aeropuertos,minimo)
			recorrido.pop()	

def recorrer_mundo(grafo,ciudades_a_aeropuertos,aeropuertos_a_ciudades,origen):
	rec_finales = []
	rec_parcial,minimo = recorrer_mundo_aprox(grafo,ciudades_a_aeropuertos,aeropuertos_a_ciudades,origen,False)
	for aeropuerto in ciudades_a_aeropuertos[origen]:
		recorrido =  []
		recorrido.append(aeropuerto)
		falso_dfs(grafo,aeropuerto,recorrido,rec_finales,aeropuertos_a_ciudades,ciudades_a_aeropuertos,[minimo])

	maxi = math.inf
	rec =  []	

	for recorrido in rec_finales:
		costo = ver_costo_recorrido(recorrido,grafo)
		if costo < maxi:
			maxi = costo
			rec = recorrido

	print(recorrido_esquematizar(rec))
	print(maxi)
	return rec	
			

def calcular_ordenes(grafo,origen,ordenes,total):
	visitados = set()
	visitados.add(origen)
	cola = Queue()
	cola.put(origen)

	while not cola.empty():
		v = cola.get()
		for w in grafo.adyacentes(v):
			if w not in visitados:
				visitados.add(w)
				if(ordenes[v] + 1 < 5):
					ordenes[w] += ordenes[v] + 1
					cola.put(w)

def betweeness_centrality_aprox(grafo):
	centralidad = {}
	total = 0
	for v in grafo:
		centralidad[v] = 0
		total += 1
	
	for v in grafo:
		if(len(grafo.adyacentes(v)) < 5):
			calcular_ordenes(grafo,v,centralidad,total)

	for v in grafo:
		centralidad[v] *= len(grafo.adyacentes(v))

	return centralidad

def centralidad2(grafo,n):
	centralidad = betweeness_centrality_aprox(grafo)
	valores = list(centralidad.items())
	valores.sort(key=seg_elemento,reverse=True)
	s = ""
	for i in range(n):
		s += "{}, ".format(valores[i][0])
	print(s[:-2])	


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
		elif parametros[0] == "centralidad":
			n = int(parametros[1])
			centralidad(grafo,n)
		elif parametros[0] == "centralidad2":
			n = int(parametros[1])
			centralidad2(grafo,n)
		elif parametros[0] == "recorrer_mundo_aprox":
			origen = parametros[1]
			ultimo_comando,minimo = recorrer_mundo_aprox(grafo,ciudades_a_aeropuertos,aeropuertos_a_ciudades,origen,True)
		elif parametros[0] == "camino_mas":
			para = parametros[1].split(",")
			modo = 0
			if para[0] == "barato":
				modo = 1
			origen = para[1]
			destino = para[2]
			ultimo_comando = camino_mas(grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos,modo,origen,destino)
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
		elif parametros[0] == "recorrer_mundo":
			origen = parametros[1]
			ultimo_comando = recorrer_mundo(grafo,ciudades_a_aeropuertos,aeropuertos_a_ciudades,origen)	


main()

