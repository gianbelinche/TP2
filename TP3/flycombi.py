from grafo import *
from grafo_funciones_aux import *

def crear_grafo(archivo1,archivo2):
	grafo = Grafo()
	with open(archivo1),open(archivo2) as aeropuertos,vuelos:
		for linea in aeropuertos:
			elementos = linea.rstrip("\n").split(",")
			ciudad = (elementos[0],0) #0 para ciudad
			aeropuerto = (elementos[1],1) #1 para aeropuerto
			grafo.agregar_vertice(ciudad) 
			grafo.agregar_vertice(aeropuerto) 
			grafo.agregar_arista(ciudad,aeropuerto,(0,0))

		for linea in vuelos:
			elementos = linea.rstrip("\n").split(",")
			aeropuerto_i = (elementos[0],1)
			aeropuerto_j = (elementos[1],1)
			tiempo = elementos[2]
			precio = elementos[3]
			peso = (tiempo,precio)
			grafo.agregar_arista(aeropuerto_i,aeropuerto_j,peso)

	return grafo		

def listar_operaciones():
	print()
	print()

def main(cosas raras que no recuerdo como hacer):
	grafo = crear_grafo(archivo1,archivo2)
	while True:
		comando = input()
		parametros = comandos.rstrip("\n").split(" ")
		if parametros[0] == "listar_operaciones":
			listar_operaciones()
		elif parametros[0] == "....":
			if se cumplen los parametros:
				llamar_funcion()
		else:
			error

		cuando mierda lo corto???	



