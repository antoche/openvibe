from math import *
import random, time

class MyOVBox(OVBox):
	def __init__(self):
		OVBox.__init__(self)
		self.channelCount = 0
		self.samplingFrequency = 0
		self.epochSampleCount = 0
		self.startTime = 0
		self.endTime = 0
		self.dimensionSizes = list()
		self.dimensionLabels = list()
		self.timeBuffer = list()
		self.signalBuffer = list()
		self.signalHeader = None
		
	def initialize(self):
		random.seed(time.time())
		self.channelCount = int(self.setting['Channel count'])
		self.samplingFrequency = int(self.setting['Sampling frequency'])
		self.epochSampleCount = int(self.setting['Generated epoch sample count'])
		
		#creation of the signal header
		for i in range(self.channelCount):
			self.dimensionLabels.append( 'Sinus'+str(i) )
		self.dimensionLabels += self.epochSampleCount*['']
		self.dimensionSizes = [self.channelCount, self.epochSampleCount]
		self.signalHeader = OVSignalHeader(0., 0., self.dimensionSizes, self.dimensionLabels, self.samplingFrequency)
		self.output[0].append(self.signalHeader)
		
		#creation of the first signal chunk
		self.endTime = self.epochSampleCount
		self.signalBuffer = [0.]*self.channelCount*self.epochSampleCount
		self.update_timeBuffer()
		self.update_signalBuffer()
		
		
	def updateStartTime(self):
		self.startTime += self.epochSampleCount
		
	def updateEndTime(self):
		self.endTime += self.epochSampleCount
	
	def updateTimeBuffer(self):
		self.timeBuffer = range(self.startTime, self.endTime)
		for instant_index, instant in enumerate(self.timeBuffer):
			self.timeBuffer[instant_index] = float(instant)/self.samplingFrequency
		
		
	def updateSignalBuffer(self):
		for channel in range(self.channelCount):
			for instantIndex, instant in enumerate(self.timeBuffer):
				newSample = 100.*sin(2.*pi*float(channel+1)*instant) + (1000.*random.random()-500.)
				self.signalBuffer[channel*self.epochSampleCount+instantIndex] = newSample
			
	def sendSignalBufferToOpenvibe(self):
		start = self.timeBuffer[0]
		end = self.timeBuffer[-1] + 1.0/self.samplingFrequency
		self.output[0].append( OVSignalBuffer(start, end, self.signalBuffer) )
	
	def process(self):
		start = self.timeBuffer[0]
		end = self.timeBuffer[-1]
		if self.getCurrentTime() >= end:
			self.sendSignalBufferToOpenvibe()
			self.updateStartTime()
			self.updateEndTime()
			self.updateTimeBuffer()
			self.updateSignalBuffer()

	def uninitialize(self):
		end = self.timeBuffer[-1]
		self.output[0].append(OVSignalEnd(end, end))

box = MyOVBox()
