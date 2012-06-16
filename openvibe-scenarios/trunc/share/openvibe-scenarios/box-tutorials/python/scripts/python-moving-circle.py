from multiprocessing import Process, Event
import time
import pygame
from pygame.locals import *
import math
from random import randint, randrange
import sys
sys.argv = ['openvibe']

class OVProcess(Process):
	def __init__(self):
		Process.__init__(self)
		self.initialized = Event()
		self.stop_asked = Event()
	
	def initialize(self):
		pass 
		
	def process(self):
		pass 
		
	def uninitialize(self):
		pass 
		
	def stop(self):
		self.stop_asked.set()
	
	def run(self):
		self.initialize()
		self.initialized.set()
		while not self.stop_asked.is_set():
			self.process()
		self.uninitialize()
		
class MovingCircle(OVProcess):
	def __init__(self):
		OVProcess.__init__(self)
		self.screen = None
		self.pos = [320,240]
		self.speed = 5
		self.dirx = -self.speed
		self.diry = -self.speed
		
	def initialize(self):
		pygame.init()
		pygame.display.set_caption(self.name)
		self.screen = pygame.display.set_mode((640,480),0,32)

	def process(self):
		begining = time.time()
		self.pos[0] += self.dirx
		self.pos[1] += self.diry
		if self.pos[0] > 640 or self.pos[0] < 0:
			self.dirx = -self.dirx
		if self.pos[1] > 480 or self.pos[1] < 0:
			self.diry = -self.diry
		self.screen.fill((255,255,255))
		pygame.draw.circle(self.screen, (0,0,0), self.pos, 25, 0)
		pygame.display.update()
		while (time.time() - begining) < (1.0/60.0):
			pass
			

class MyOVBox(OVBox):
	def __init__(self):
		OVBox.__init__(self)
		self.p = None
	def initialize(self):
		self.p = MovingCircle()
		self.p.start()
	
	def process(self):
		pass 
	
	def uninitialize(self):
		self.p.stop()
		self.p.join()				

if __name__ == '__main__':
	box = MyOVBox()
