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
		
			
class StarField(OVProcess):
	def __init__(self, num_stars, max_depth):
		OVProcess.__init__(self)
		self.pygame_is_running = False
		self.screen = None
		self.clock = None
		self.num_stars = num_stars
		self.max_depth = max_depth
		self.stars = []
		for i in range(self.num_stars):
			star = [randrange(-25,25), randrange(-25,25), randrange(1, self.max_depth)]
			self.stars.append(star)
		
	def initialize(self):
		pygame.init()
		self.pygame_is_running = True
		self.screen = pygame.display.set_mode((640, 480))
		pygame.display.set_caption("3D Starfield Simulation")
		self.clock = pygame.time.Clock()

	def move_and_draw_stars(self):
		origin_x = self.screen.get_width() / 2
		origin_y = self.screen.get_height() / 2
		for star in self.stars:
			star[2] -= 0.19
			if star[2] <= 0:
				star[0] = randrange(-25,25)
				star[1] = randrange(-25,25)
				star[2] = self.max_depth
			k = 128.0 / star[2]
			x = int(star[0] * k + origin_x)
			y = int(star[1] * k + origin_y)
			if 0 <= x < self.screen.get_width() and 0 <= y < self.screen.get_height():
				size = (1 - float(star[2]) / self.max_depth) * 5
				shade = (1 - float(star[2]) / self.max_depth) * 255
				self.screen.fill((shade,shade,shade),(x,y,size,size))

	def process(self):
		if self.pygame_is_running:
			self.clock.tick(50)
			for event in pygame.event.get():
				if event.type == pygame.QUIT:
					pygame.quit()
					self.pygame_is_running = False
					return
			self.screen.fill((0,0,0))
			self.move_and_draw_stars()
			pygame.display.flip()
		
	def uninitialize(self):
		if self.pygame_is_running:
			pygame.quit()
			self.pygame_is_running = False

class MyOVBox(OVBox):
	def __init__(self):
		OVBox.__init__(self)
		self.p = None
	def initialize(self):
		self.p = StarField(512, 32)
		self.p.start()
	
	def process(self):
		pass 
	
	def uninitialize(self):
		self.p.stop()
		self.p.join()				

if __name__ == '__main__':
	box = MyOVBox()
