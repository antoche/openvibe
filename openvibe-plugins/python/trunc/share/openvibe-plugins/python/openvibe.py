# -*- coding: utf-8 -*-
#File Name : openvibe.py
#Created By : Aurélien Van Langhenhove

from StimulationsCodes import *
import traceback, collections
from StringIO import StringIO

class NewStd(StringIO):
    def __init__(self):
        StringIO.__init__(self)
    def flush(self):
        self.buf = str()
    def isempty(self):
        if len(self.buf) == 0:
            return True
        else:
            return False

sys.stdout = NewStd()
sys.stderr = NewStd()


def execfileHandlingException(filename, maindictionary):
    print "executing script file"

    try:
        maindictionary['box'] = OVBox(default=True) # on en cree une au cas ou
        #print globals()
        execfile(filename, maindictionary) 
        return 0;
    except:
        print "error"
        print traceback.format_exc()
        return -1;

def decoratorFunction(target):
    """ add a try except block to protect openvibe box in case of exception """
    def wrapper(self):
        try :
            print "using decorator"
            print 'Calling function "%s"' % target.__name__
            return target(self)
        except:
            print traceback.format_exc()

    return wrapper

class OVChunk(object):
    def __init__(self, startTime, endTime):
        self.startTime = startTime
        self.endTime = endTime


class OVStreamedMatrixHeader(OVChunk):
    def __init__(self, startTime, endTime, dimensionSizes, dimensionLabels):
        OVChunk.__init__(self, startTime, endTime)
        self.dimensionSizes = list(dimensionSizes)
        self.dimensionLabels = list(dimensionLabels)
        
    def getDimensionCount(self):
        return len(self.dimensionSizes)
        
    def getBufferElementCount(self):
        elementCount = 0
        for dimension, size in enumerate(self.dimensionSizes):
            if dimension == 0:
                elementCount = int(size)
            else:
                elementCount *= int(size)
        return elementCount

class OVStreamedMatrixBuffer(OVChunk, list):
    def __init__(self, startTime, endTime, bufferElements):
        OVChunk.__init__(self, startTime, endTime)
        list.__init__(self, bufferElements)

class OVStreamedMatrixEnd(OVChunk):
    pass 


    
class OVSignalHeader(OVStreamedMatrixHeader):
    def __init__(self, startTime, endTime, dimensionSizes, dimensionLabels, samplingRate):
        OVStreamedMatrixHeader.__init__(self, startTime, endTime, dimensionSizes, dimensionLabels)
        self.samplingRate = int(samplingRate)
        
class OVSignalBuffer(OVStreamedMatrixBuffer):
    pass 
    
class OVSignalEnd(OVChunk):
    pass 
    


class OVStimulation(object):
    def __init__(self, identifier, date, duration):
        self.identifier = identifier
        self.date = date
        self.duration = duration

class OVStimulationHeader(OVChunk):
    pass

class OVStimulationSet(OVChunk, list):
    def __init__(self, startTime, endTime):
        list.__init__(self)
        OVChunk.__init__(self, startTime, endTime)
        
    def append(self, item):
        if isinstance(item, OVStimulation):
            list.append(self, item)
        else:
            raise TypeError("The item must be an OVStimulation")

class OVStimulationEnd(OVChunk):
    pass 
    


class OVBuffer(object):
    def __init__(self, inputType):
        self.__deque = collections.deque()
        self.__type = inputType
    def __len__(self):
        return len(self.__deque)
    def __getitem__(self, key):
        return self.__deque[key]
    def __setitem__(self, key, item):
        self.__deque[key] = item
    def __delitem__(self, key):
        del self.__deque[key]
    def append(self, toAppend):
        self.__deque.appendleft(toAppend)
    def pop(self):
        return self.__deque.pop()
    def type(self):
        return self.__type


class OVBox(object):
	def __init__(self, default=False):
		self.input = list()
		self.output = list()
		self.setting = dict()
		self.var = dict()
		self._clock = 0
		self._currentTime = 0.
		self.default= default
	def addInput(self, inputType):
		self.input.append(OVBuffer(inputType))
	def addOutput(self, outputType):
		self.output.append(OVBuffer(outputType))
	def getClock(self):
		return self._clock
	def getCurrentTime(self):
		return self._currentTime
	def initialize(self):
		if self.default == True:
			print "The box instance has not been created by user script, using default one from openvibe.py (dummy box)."
		pass
	def process(self):
		pass
	def uninitialize(self):
		pass
	def realInitialize(self):
		try :
			self.initialize()
		except:
			print traceback.format_exc()
	def realProcess(self):
		try :
			self.process()
		except:
			print traceback.format_exc()
	def realUninitialize(self):
		try :
			self.uninitialize()
		except:
			print traceback.format_exc()
