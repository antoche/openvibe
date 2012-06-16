from multiprocessing import Process, Queue
import time

class MyOVBox(OVBox):
	def __init__(self):
		OVBox.__init__(self)
		self.p = None
		self.q = None
	
	def f(self, queue):
		while True:
			queue.put('hello')
			time.sleep(1)
	
	def initialize(self):
		print "process initialize!"
		self.q = Queue()
		self.p = Process(target=self.f, args=(self.q,))
		self.p.start()
	
	def process(self):
		for i in range(self.q.qsize()):
			print self.q.get()
	
	def uninitialize(self):
		print "process uninitialize!"
		self.p.terminate()
		self.p.join()				

if __name__ == '__main__':
	box = MyOVBox()
