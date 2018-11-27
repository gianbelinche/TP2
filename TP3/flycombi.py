from grafo import *
from grafo_funciones_aux import *



#................................  FUNCIONES AUXILIARES ................................#

def crear_grafo(archivo1,archivo2):
	grafo = Grafo()
	aeropuertos_a_ciudades = {}
	ciudades_a_aeropuertos = {}
	with open(archivo1) as aeropuertos, open(archivo2) as vuelos:
		for linea in aeropuertos:
			elementos = linea.rstrip("\n").split(",")
			ciudad = elementos[0]
			aeropuerto = elementos[1] 
			grafo.agregar_vertice(aeropuerto)
			ciudades_a_aeropuertos[ciudad] = ciudades_a_aeropuertos.get(ciudad,[]) + [aeropuerto]
			aeropuertos_a_ciudades[aeropuerto] = ciudad 

		for linea in vuelos:
			elementos = linea.rstrip("\n").split(",")
			aeropuerto_i = elementos[0]
			aeropuerto_j = elementos[1]
			tiempo = elementos[2]
			precio = elementos[3]
			vuelos_x_año = elementos[4]
			peso = (int(tiempo),int(precio),1/int(vuelos_x_año))
			grafo.agregar_arista(aeropuerto_i,aeropuerto_j,peso)

	return grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos		

def mostrar_grafo(grafo):
	for v in grafo:
		for w in grafo.adyacentes(v):
			print("{} - {} - {}".format(v,grafo.ver_peso(v,w),w)) 

def listar_operaciones():
	print("camino_escalas")
	print()

def crear_conjunto_disjunto(lista):
	conjunto_disjunto = {}
	for v in lista:
		conjunto_disjunto[v] = v

	return conjunto_disjunto

def find(conjunto_disjunto,v)
	if(conjunto_disjunto[v] == v):
		return v
	
	w = _find(conjunto_disjunto,v)
	conjunto_disjunto[v] = w
	return w

def union(conjunto_disjunto,v,w)
	conjunto_disjunto[find(conjunto_disjunto, w)] = v

#................................       COMANDOS        ................................#



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
	while len(rec) != 0:
		s += "{} -> ".format(rec[-1])
		rec.pop()
	print(s[:-4])	
			


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


def nueva_aerolinea(grafo,origen):
	conjunto_disjunto = crear_conjunto_disjunto(grafo.obtener_vertices)
	aristas = ordenar_aristas(grafo.obtener_aristas)

	rutas_a_devolver = []

	for arista in aristas
		if(find(conjunto_disjunto, w) == find(conjunto_disjunto, v))
			continue
		rutas_a_devolver.append(arista)
		union(conjunto_disjunto,w,v)

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
			


def main(): #cosas raras que no recuerdo como hacer
	grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos = crear_grafo("aeropuertos_inventados.csv","vuelos_inventados.csv") #archivo1,archivo2
	while True:
		comando = input()
		parametros = separar_parametros(comando.rstrip("\n"))
		if parametros[0] == "listar_operaciones":
			listar_operaciones()
		elif parametros[0] == "camino_escalas":
			origen,destino = parametros[1].split(",")
			camino_escalas(grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos,origen,destino)	
		elif parametros[0] == "centralidad":
			n = int(parametros[1])
			centralidad(grafo,n)
		elif parametros[0] == "recorrer_mundo_aprox":
			origen = parametros[1]
			recorrer_mundo_aprox(grafo,ciudades_a_aeropuertos,aeropuertos_a_ciudades,origen)
		elif parametros[0] == "camino_mas":
			para = parametros[1].split(",")
			modo = 0
			if para[0] == "barato":
				modo = 1
			origen = para[1]
			destino = para[2]
			camino_mas(grafo,aeropuertos_a_ciudades,ciudades_a_aeropuertos,modo,origen,destino)	

		#else:
			#error

		#cuando mierda lo corto???	

main()


