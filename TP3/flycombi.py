from grafo import *
from grafo_funciones_aux import *

def crear_grafo(archivo1,archivo2):
	grafo = Grafo()
	with open(archivo1) as aeropuertos, open(archivo2) as vuelos:
		for linea in aeropuertos:
			elementos = linea.rstrip("\n").split(",")
			ciudad = (elementos[0],0) #0 para ciudad
			aeropuerto = (elementos[1],1) #1 para aeropuerto
			grafo.agregar_vertice(ciudad) 
			grafo.agregar_vertice(aeropuerto) 
			grafo.agregar_arista(ciudad,aeropuerto,(0,0,0))

		for linea in vuelos:
			elementos = linea.rstrip("\n").split(",")
			aeropuerto_i = (elementos[0],1)
			aeropuerto_j = (elementos[1],1)
			tiempo = elementos[2]
			precio = elementos[3]
			vuelos_x_año = elementos[4]
			peso = (tiempo,precio,vuelos_x_año)
			grafo.agregar_arista(aeropuerto_i,aeropuerto_j,peso)

	return grafo		

def listar_operaciones():
	print("camino_escalas")
	print()

def camino_escalas(grafo,origen,destino): #!!!FUNCIONAAAAAAAA
	padres,orden = bfs(grafo,origen)
	camino = []
	actual = destino
	while actual:
		if actual[1] == 1:
			camino.append(actual[0])
		actual = padres[actual]
	s = ""	
	for aeropuerto in camino[::-1]:
		s += "{} -> ".format(aeropuerto)
			
	print(s[:-4])			

def centralidad(grafo,n):
	centralidad = betweeness_centrality(grafo)
	valores = centralidad.items()
	valores.sort(key=seg_elemento)
	s = ""
	for i in range(n):
		s += "{}, ".format(valores[i][0])
	print(s[:-2])	


def main(): #cosas raras que no recuerdo como hacer
	grafo = crear_grafo("aeropuertos_inventados.csv","vuelos_inventados.csv") #archivo1,archivo2
	while True:
		comando = input()
		parametros = comandos.rstrip("\n").split(" ")
		if parametros[0] == "listar_operaciones":
			listar_operaciones()
		elif parametros[0] == "camino_escalas":
			origen,destino = parametos[1].split(",")
			camino_escalas(grafo,(origen,0),(destino,0))	
		elif parametros[0] == "centralidad":
			n = int(parametros[1])
			centralidad(grafo,n)
		#else:
			#error

		#cuando mierda lo corto???	

main()


