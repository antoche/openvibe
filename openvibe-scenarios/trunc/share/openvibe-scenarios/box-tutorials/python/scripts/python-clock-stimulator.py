# We construct a box instance that inherits from the basic OVBox class
class MyOVBox(OVBox):
    # the constructor creates the box and initializes object variables
	def __init__(self):
		OVBox.__init__(self)
		self.stimLabel = None
		self.stimCode = None
		
	# the initialize method reads settings and outputs the first header
	def initialize(self):
                # the stim label is taken from the box setting
		self.stimLabel = self.setting['Stimulation']
		# we get the corresponding code using the OpenViBE_stimulation dictionnary
		self.stimCode = OpenViBE_stimulation[self.stimLabel]
		# we append to the box output a stimulation header. This is just a header, dates are 0.
		self.output[0].append(OVStimulationHeader(0., 0.))
	
	def process(self):
                # During each process call we produce a stimulation
                # A stimulation set is a chunk which starts at current time and end time is the time step between two calls
		stimSet = OVStimulationSet(self.getCurrentTime(), self.getCurrentTime()+1./self.getClock())
                # the date of the stimulation is simply the current openvibe time when calling the box process
		stimSet.append(OVStimulation(self.stimCode, self.getCurrentTime(), 0.))
		self.output[0].append(stimSet)

	def uninitialize(self):
                # we send a stream end.
		end = self.getCurrentTime()
		self.output[0].append(OVStimulationEnd(end, end))				

box = MyOVBox()
