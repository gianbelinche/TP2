from random import randint

class Grafo:

	def __init__ (self):
		self.datos = {}
		self.cantidad = 0

	def agregar_vertice(self,v):
		if v in self.datos:
			return False
		self.datos[v] = {}
		self.cantidad += 1
		return True

	def agregar_arista(self,v,w,peso):
		if v not in self.datos or w not in self.datos:
			return False
		self.datos[v][w] = peso
		self.datos[w][v] = peso
		return True

	def estan_conectados(self,v,w):
		if v not in self.datos or w not in self.datos:
			return False
		return w in self.datos[v]
		
	def sacar_arista(self,v,w):
		if v not in self.datos or w not in self.datos:
			return False
		self.datos[v].pop(w)
		self.datos[w].pop(v)
		return True

	def sacar_vertice(self,v):
		if v not in self.datos:
			return False
		for w in self.datos[v]:
			self.datos[w].pop(v)
		self.datos.pop(v)
		self.cantidad -= 1
		return True

	def adyacentes(self,v):
		return list(self.datos.get(v,[]))

	def ver_peso(self,v,w):
		if v not in self.datos or w not in self.datos:
			return None
		return self.datos[v][w]

	def __len__ (self):
		return self.cantidad

	def vertice_random(self):
		i = randint(0,self.cantidad)
		return list(self.datos)[i]

		
	def __iter__ (self):
		return _IterGrafo(self.cantidad,list(self.datos))


class _IterGrafo:
	def __init__ (self,cantidad,lista):
		self.cantidad = cantidad
		self.pos = 0
		self.lista = lista

	def __next__(self):
		if self.pos >= self.cantidad:
			raise StopIteration()
		self.pos += 1
		return self.lista[self.pos-1]		

				

