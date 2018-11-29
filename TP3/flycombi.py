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
			if (w,v,peso) not in aristas or (v,w,peso) not in aristas:
				aristas[(v,w,peso)] = True

	return aristas

def ord_pesos(arista):
	return arista[2][1]

def ordenar_aristas(aristas):
	return sorted(aristas.keys(),key=ord_pesos)

#................................       COMANDOS        ................................#

def listar_operaciones():
	print("camino_escalas")
	print("camino_mas")
	print("centralidad")
	print("nueva_aerolinea")
	print("recorrer_mundo_aprox")
	print("vacaciones")
	print("exportar_kml")


def camino_escalas(grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos,origen,destino): #!!!FUNCIONAAAAAAAA
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
				
	s = ""	
	for aeropuerto in cam[::-1]:
		s += "{} -> ".format(aeropuerto)	
			
	print(s[:-4])			

	return cam[::-1]

def centralidad(grafo,n): #Ya funciona
	centralidad = betweeness_centrality(grafo)
	valores = list(centralidad.items())
	valores.sort(key=seg_elemento,reverse=True)
	s = ""
	for i in range(n):
		s += "{}, ".format(valores[i][0])
	print(s[:-2])	

def recorrer_mundo_aprox(grafo,ciudades_a_aeropuertos,aeropuertos_a_ciudades,origen): #tampoco anda
	recorridos = []
	for aeropuerto in ciudades_a_aeropuertos[origen]:
		camino = []
		visitados = {}
		precio = 0
		vuelos = len(grafo) 
		camino.append(aeropuerto)
		cantidad = 0
		actual = aeropuerto
		anterior = None
		prox = None	
		while cantidad != vuelos:
			mini = math.inf
			for ady in grafo.adyacentes(actual):
				if prox == None or (grafo.ver_peso(actual,ady)[1] < mini and ady != anterior):
					mini = grafo.ver_peso(actual,ady)[1]
					prox = ady
			anterior = actual				
			actual = prox
			camino.append(actual)
			precio += mini
			if actual not in visitados:
				visitados[actual] = True
				cantidad += 1
		recorridos.append((camino,precio))	

	minimo = math.inf
	cam = []
	for camino in recorridos:
		if camino[1] < minimo:
			cam = camino[0]
			minimo = camino[1]

	s = ""
	for aeropuerto in cam:
		s += "{} -> ".format(aeropuerto)
	print(s[:-4])
	print("Costo: {}".format(minimo))

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
			
	s = ""
	rec2 = []	
	while len(rec) != 0:
		s += "{} -> ".format(rec[-1])
		rec2.append(rec.pop())
	print(s[:-4])	

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
	origenx = ciudades_a_aeropuertos[origen][0]
	visitados = set()
	visitados.add(origenx)
	distancia = calcular_distancia(grafo,origenx,escalas) #Acota la cantidad de vertices a considerar
	padres = {}
	padres[origenx] = None

	final = _viaje_n_lugares(grafo,distancia,visitados,padres,origenx,origenx,escalas,escalas)

	if (final == None):
		print("No se encontro recorrido")
	else:
		camino_aeropuertos = reconstruir_camino(padres,final)
		ultima_ciudad = None
		camino_ciudades = []

		for aeropuerto in camino_aeropuertos:
			if(ultima_ciudad != aeropuertos_a_ciudades[aeropuerto]):
				ultima_ciudad = aeropuertos_a_ciudades[aeropuerto]
				camino_ciudades.append(ultima_ciudad)

		print(camino_ciudades)

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
			v,w,peso = arista
			salida.write("{},{},{},{},{}\n".format(v,w,peso[0],peso[1], 1/peso[2]))

	print("OK")


def exportar_KML(archivo, aeropuertos_a_ciudades,coordenadas, recorrido):
	ciudades_registradas = {}
	ciudad_previa = None
	with open(archivo, "w") as salida:
		salida.write( '<?xml version="1.0" encoding="UTF-8"?>')
		salida.write(' <kml xmlns="http://www.opengis.net/kml/2.2">')
		salida.write('     <Document>')
		for v in recorrido:
			ciudad = aeropuertos_a_ciudades[v]
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
			ultimo_comado = camino_escalas(grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos,origen,destino)
		elif parametros[0] == "centralidad":
			n = int(parametros[1])
			centralidad(grafo,n)
		elif parametros[0] == "recorrer_mundo_aprox":
			origen = parametros[1]
			ultimo_comado = recorrer_mundo_aprox(grafo,ciudades_a_aeropuertos,aeropuertos_a_ciudades,origen)
		elif parametros[0] == "camino_mas":
			para = parametros[1].split(",")
			modo = 0
			if para[0] == "barato":
				modo = 1
			origen = para[1]
			destino = para[2]
			ultimo_comado = camino_mas(grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos,modo,origen,destino)
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
			ultimo_comado = viaje_n_lugares(grafo,ciudades_a_aeropuertos,aeropuertos_a_ciudades,origen,n)


main()

