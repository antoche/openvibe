#if defined TARGET_HAS_ThirdPartyMatlab

#include "ovpCBoxAlgorithmMatlabScripting.h"


#include <system/Memory.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <mex.h>
#include <engine.h>
#include <string>
#include <ctime>

#if defined TARGET_OS_Windows
	#include <windows.h>
	#include <direct.h>
	#define getCurrentDir _getcwd
#else
	#include <unistd.h>
	#define getCurrentDir getcwd
#endif


// Size of the internal buffer storing matlab messages, in char
#define MATLAB_BUFFER 2048

#define m_pMatlabEngine ((Engine*)m_pMatlabEngineHandle)

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEToolkit;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Matlab;


using namespace std;

#define boolean OpenViBE::boolean

// The CHECK_FAILURE_ROUTINE verify the result of a matlab call (via engine or helper functions) given as argument.
// If the result is false (the matlab call failed), the message msg is printed in the Error Log Level, and the macro returns false.
// The Matlab output buffer is then printed. If an error message is detected in the buffer, the same error message is printed and
// the macro returns false.
#define CHECK_FAILURE_ROUTINE(bResult,msg) \
	if(!bResult)\
	{ \
		this->getLogManager() << LogLevel_Error << msg;\
		return false;\
	} \
	this->printOutputBufferWithFormat();\
	if(m_bErrorDetected)\
	{\
		this->getLogManager() << LogLevel_Error << msg;\
		return false;\
	}

boolean CBoxAlgorithmMatlabScripting::OpenMatlabEngineSafely(void)
{
	this->getLogManager() << LogLevel_Trace << "Trying to open Matlab engine\n";
#if defined TARGET_OS_Linux
	m_pMatlabEngineHandle=::engOpen(m_sMatlabPath.toASCIIString());
#elif defined TARGET_OS_Windows
	__try
	{
		m_pMatlabEngineHandle=::engOpen(NULL);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		this->getLogManager() << LogLevel_Error << "First call to MATLAB engine failed.\n"
			<< "\tTo use this box you must have MATLAB (32 bits version) installed on your computer.\n";
		m_pMatlabEngineHandle = NULL;
		return false;
	}
#else
#endif
	if(!m_pMatlabEngine)
	{
		this->getLogManager() << LogLevel_Error << "Could not open Matlab engine\n";
		return false;
	}
	return true;
}

boolean CBoxAlgorithmMatlabScripting::initialize(void)
{
	m_sBoxInstanceVariableName = "OV_BOX_";
	// we add a random identifier
	srand((unsigned int)time(NULL));
	unsigned short int l_ui16Value1=(rand()&0xffff);
	unsigned short int l_ui16Value2=(rand()&0xffff);
	unsigned short int l_ui16Value3=(rand()&0xffff);
	unsigned short int l_ui16Value4=(rand()&0xffff);
	char l_sBuffer[1024];
	sprintf(l_sBuffer,"0x%04X%04X_0x%04X%04X", (int)l_ui16Value1, (int)l_ui16Value2, (int)l_ui16Value3, (int)l_ui16Value4);
	m_sBoxInstanceVariableName = m_sBoxInstanceVariableName + CString(l_sBuffer);

	CString l_sSettingValue;
	getStaticBoxContext().getSettingValue(0, l_sSettingValue);
	m_ui64ClockFrequency=::atoi(l_sSettingValue.toASCIIString());
	
	for(uint32 i=0; i<getStaticBoxContext().getInputCount(); i++)
	{
		CIdentifier l_oInputType;
		getStaticBoxContext().getInputType(i, l_oInputType);
		TDecoder<CBoxAlgorithmMatlabScripting> * l_pDecoder = NULL;
		if(l_oInputType==OV_TypeId_StreamedMatrix)
		{
			l_pDecoder= new TStreamedMatrixDecoder<CBoxAlgorithmMatlabScripting>(*this);
		}
		else if(l_oInputType==OV_TypeId_ChannelLocalisation)
		{
			l_pDecoder= new TChannelLocalisationDecoder<CBoxAlgorithmMatlabScripting>(*this);
		}
		else if(l_oInputType==OV_TypeId_FeatureVector)
		{
			l_pDecoder= new TFeatureVectorDecoder<CBoxAlgorithmMatlabScripting>(*this);
		}
		else if(l_oInputType==OV_TypeId_Spectrum)
		{
			l_pDecoder= new TSpectrumDecoder<CBoxAlgorithmMatlabScripting>(*this);
		}
		else if(l_oInputType==OV_TypeId_Signal)
		{
			l_pDecoder= new TSignalDecoder<CBoxAlgorithmMatlabScripting>(*this);
		}
		else if(l_oInputType==OV_TypeId_Stimulations)
		{
			l_pDecoder= new TStimulationDecoder<CBoxAlgorithmMatlabScripting>(*this);
		}
		else if(l_oInputType==OV_TypeId_ExperimentationInformation)
		{
			l_pDecoder= new TExperimentInformationDecoder<CBoxAlgorithmMatlabScripting>(*this);
		}
		else
		{
			this->getLogManager() << LogLevel_Warning << "Undefined type on input ["<<i<<"].\n";
		}
		if(l_pDecoder != NULL)
		{
			m_mDecoders.insert(make_pair(i,l_pDecoder));
			m_NbInputHeaderSent = 0;
		}
	}

	for(uint32 i=0; i<getStaticBoxContext().getOutputCount(); i++)
	{
		CIdentifier l_oOutputType;
		getStaticBoxContext().getOutputType(i, l_oOutputType);
		TEncoder<CBoxAlgorithmMatlabScripting> * l_pEncoder = NULL;
		if(l_oOutputType==OV_TypeId_StreamedMatrix)
		{
			l_pEncoder= new TStreamedMatrixEncoder<CBoxAlgorithmMatlabScripting>(*this);
		}
		else if(l_oOutputType==OV_TypeId_ChannelLocalisation)
		{
			l_pEncoder= new TChannelLocalisationEncoder<CBoxAlgorithmMatlabScripting>(*this);
		}
		else if(l_oOutputType==OV_TypeId_FeatureVector)
		{
			l_pEncoder= new TFeatureVectorEncoder<CBoxAlgorithmMatlabScripting>(*this);
		}
		else if(l_oOutputType==OV_TypeId_Spectrum)
		{
			l_pEncoder= new TSpectrumEncoder<CBoxAlgorithmMatlabScripting>(*this);
		}
		else if(l_oOutputType==OV_TypeId_Signal)
		{
			l_pEncoder= new TSignalEncoder<CBoxAlgorithmMatlabScripting>(*this);
		}
		else if(l_oOutputType==OV_TypeId_Stimulations)
		{
			l_pEncoder= new TStimulationEncoder<CBoxAlgorithmMatlabScripting>(*this);
		}
		else if(l_oOutputType==OV_TypeId_ExperimentationInformation)
		{
			l_pEncoder= new TExperimentInformationEncoder<CBoxAlgorithmMatlabScripting>(*this);
		}
		else
		{
			this->getLogManager() << LogLevel_Warning << "Undefined type on input ["<<i<<"].\n";
		}
		if(l_pEncoder != NULL)
		{
			m_mEncoders.insert(make_pair(i,l_pEncoder));
			m_mOutputHeaderState.insert(make_pair(i,false));
		}
	}

	getStaticBoxContext().getSettingValue(1, m_sMatlabPath);

#if defined TARGET_OS_Windows
	char * l_sPath = getenv("PATH");
	if(l_sPath == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Could not access the environment variable PATH to add Matlab.\n";
		return false;
	}
	CString l_sCommandForPathModification = l_sPath + m_sMatlabPath+ ";";
	if(_putenv_s("PATH",l_sCommandForPathModification) != 0)
	{
		this->getLogManager() << LogLevel_Error << "Failed to modify the environment PATH with the Matlab path.\n";
		return false;
	}
#endif

	if(!OpenMatlabEngineSafely()) return false;
	
	m_sMatlabBuffer = new char[MATLAB_BUFFER+1];
	m_sMatlabBuffer[MATLAB_BUFFER]='\0';
	::engOutputBuffer(m_pMatlabEngine, m_sMatlabBuffer, MATLAB_BUFFER);
	m_bErrorDetected = false;

	// add the openvibe toolbox to matlab path
	char l_sCurrentDir[FILENAME_MAX];
	if(getCurrentDir(l_sCurrentDir,FILENAME_MAX) == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get the execution directory.\n";
		return false;
	}
	for(uint32 i = 0; i<FILENAME_MAX;i++)
		if(l_sCurrentDir[i] == '\\') l_sCurrentDir[i] = '/';
	this->getConfigurationManager().createConfigurationToken(CString("Path_Bin_Abs"),CString(l_sCurrentDir));
	
	CString l_sOpenvibeToolboxPath = CString("addpath('") + CString(l_sCurrentDir) + CString("/../share/openvibe-plugins/matlab');");
	::engEvalString(m_pMatlabEngine, (const char * )l_sOpenvibeToolboxPath);
	//CHECK_FAILURE_ROUTINE(::engEvalString(m_pMatlabEngine, (const char * )l_sOpenvibeToolboxPath) == 0, "An error occured while adding the path to openvibe toolbox\n");	
	// If there is more than 1 Matlab box in the scenario, the path is set repeatedly
	// resulting in warning messages in the buffer. We don't print them.
	// this->printOutputBufferWithFormat(); 

	getStaticBoxContext().getSettingValue(2, l_sSettingValue); // working directory
	l_sSettingValue=CString("cd ")+l_sSettingValue;
	if(::engEvalString(m_pMatlabEngine, l_sSettingValue.toASCIIString())!=0)
	{
		this->printOutputBufferWithFormat();
		return false;
	}

	// executes the pre-run routine that defines the global identifiers for streams and stimulations codes
	CString l_sDefineFile = CString("run '") + CString(l_sCurrentDir) + CString("/../share/openvibe-plugins/matlab/OV_define.m'");
	if(::engEvalString(m_pMatlabEngine, l_sDefineFile.toASCIIString())!=0)
	{
		this->printOutputBufferWithFormat();
		return false;
	}

	// executes the pre-run routine that construct the ov_box object
	char l_sCreateBoxCommand[128];
	sprintf(l_sCreateBoxCommand, "%s = OV_createBoxInstance(%i,%i)", (const char *) m_sBoxInstanceVariableName, this->getStaticBoxContext().getInputCount(),this->getStaticBoxContext().getOutputCount());
	if(::engEvalString(m_pMatlabEngine, l_sCreateBoxCommand)!=0)
	{
		this->printOutputBufferWithFormat();
		return false;
	}
	
	//First call to a function of the openvibe toolbox
	// if it fails, the toolbox may be not installed
	mxArray * l_pBox = engGetVariable(m_pMatlabEngine,(const char*)m_sBoxInstanceVariableName);
	if(l_pBox == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to create the box instance with OV_createBoxInstance function.\n";
		return false;
	}
	
	getStaticBoxContext().getSettingValue(3,m_sInitializeFunction);
	getStaticBoxContext().getSettingValue(4,m_sProcessFunction);
	getStaticBoxContext().getSettingValue(5,m_sUninitializeFunction);

	CString l_sSettingNames  = "{";
	CString l_sSettingTypes  = "{";
	CString l_sSettingValues = "{";
	CString l_sTemp;
	CIdentifier l_oType;

	for(uint32 i = 6; i < getStaticBoxContext().getSettingCount(); i++)
	{
		// get the setting name
		getStaticBoxContext().getSettingName(i,l_sTemp);
		l_sSettingNames = l_sSettingNames + "'" + l_sTemp + "' ";
		
		//setting type
		getStaticBoxContext().getSettingType(i,l_oType);
		stringstream ss;
		ss << l_oType.toUInteger();
		l_sSettingTypes = l_sSettingTypes + "uint64("+CString(ss.str().c_str()) + ") ";
		
		//setting value, typed
		getStaticBoxContext().getSettingValue(i,l_sTemp);
		if(l_oType == OV_TypeId_Stimulation)
		{
			uint64 l_oStimCode  = FSettingValueAutoCast(*this->getBoxAlgorithmContext(),i);
			stringstream ss1;
			ss1 << l_oStimCode;
			l_sSettingValues = l_sSettingValues +  CString(ss1.str().c_str());
			// we keep the stimulation codes as doubles, to be able to put them in arrays with other doubles (such as timings). 
			// they are still comparable with uint64 matlab values.
		}
		else
		{
			if(l_oType == OV_TypeId_Stimulation || l_oType == OV_TypeId_Boolean || l_oType == OV_TypeId_Integer || l_oType == OV_TypeId_Float)
			{
				l_sSettingValues = l_sSettingValues + l_sTemp + " "; // we store the value, these types are readable by matlab directly
			}
			else
			{
				l_sSettingValues = l_sSettingValues + "'" + l_sTemp + "' "; // we store them as matlab strings using '
			}
		}
		

	}
	l_sSettingNames  = l_sSettingNames + "}";
	l_sSettingTypes = l_sSettingTypes +"}";
	l_sSettingValues = l_sSettingValues +"}";
	
	CString l_sCommand = m_sBoxInstanceVariableName + " = OV_setSettings("+m_sBoxInstanceVariableName+"," + l_sSettingNames + "," + l_sSettingTypes +"," + l_sSettingValues +");";
	//this->getLogManager() << LogLevel_Error << l_sCommand << "\n";
	CHECK_FAILURE_ROUTINE(::engEvalString(m_pMatlabEngine, (const char *)l_sCommand)==0,"Error calling [OV_setSettings]\n");

	// we set the box clock frequency in the box structure, so it's accessible in the user scripts if needed
	getStaticBoxContext().getSettingValue(0,l_sTemp);
	l_sCommand = m_sBoxInstanceVariableName + ".clock_frequency = " + l_sTemp + ";";
	CHECK_FAILURE_ROUTINE(::engEvalString(m_pMatlabEngine, l_sCommand) == 0, "An error occured while setting the clock frequency\n");

	l_sCommand = m_sBoxInstanceVariableName + " = " + m_sInitializeFunction + "(" + m_sBoxInstanceVariableName + ");";
	CHECK_FAILURE_ROUTINE(::engEvalString(m_pMatlabEngine, l_sCommand) == 0, "An error occured while calling the initialize function\n");

	m_oMatlabHelper.setMatlabEngine(m_pMatlabEngine);
	m_oMatlabHelper.setBoxInstanceVariableName(m_sBoxInstanceVariableName);

	return true;
}

boolean CBoxAlgorithmMatlabScripting::CloseMatlabEngineSafely(void)
{
	if(m_pMatlabEngine == NULL)
	{
		return true;
	}
	this->getLogManager() << LogLevel_Trace << "Trying to close Matlab engine\n";
#if defined TARGET_OS_Windows
	__try
	{
#endif
		if(m_pMatlabEngine)
		{
			if(::engClose(m_pMatlabEngine)!=0)
			{
				this->getLogManager() << LogLevel_ImportantWarning << "Could not close Matlab engine.\n";
			}
		}
#if defined TARGET_OS_Windows
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		this->getLogManager() << LogLevel_Error << "Closing MATLAB engine failed.\n"
			<< "\tTo use this box you must have MATLAB (32 bits version) installed on your computer.\n";

		return false;
	}
#endif

	return true;
}

boolean CBoxAlgorithmMatlabScripting::uninitialize(void)
{
	if(m_pMatlabEngine != NULL)
	{
		CString l_sCommand = m_sBoxInstanceVariableName + " = " + m_sUninitializeFunction + "(" + m_sBoxInstanceVariableName + ");";
		if(::engEvalString(m_pMatlabEngine, l_sCommand) != 0)
		{
			this->getLogManager() << LogLevel_Warning << "An error occured while calling the Uninitialize function ["<<m_sUninitializeFunction<<"]\n";
		}
		this->printOutputBufferWithFormat();
		delete[] m_sMatlabBuffer;
	}

	CloseMatlabEngineSafely();
	
	for(uint32 i = 0; i< m_mDecoders.size(); i++)
	{
		m_mDecoders[i]->uninitialize();
		delete m_mDecoders[i];
	}
	for(uint32 i = 0; i< m_mEncoders.size(); i++)
	{
		m_mEncoders[i]->uninitialize();
		delete m_mEncoders[i];
	}

	return true;
}

uint64 CBoxAlgorithmMatlabScripting::getClockFrequency(void)
{
	return m_ui64ClockFrequency<<32;
}

boolean CBoxAlgorithmMatlabScripting::processClock(IMessageClock& rMessageClock)
{
	if(!m_pMatlabEngine)
	{
		return true;
	}

	char l_sBuffer[32];
	sprintf(l_sBuffer, "%f",(this->getPlayerContext().getCurrentTime()>>16)/65536.);
	CString l_sCommand = m_sBoxInstanceVariableName + CString(".clock = ") + CString(l_sBuffer) + CString(";");
	if(::engEvalString(m_pMatlabEngine, l_sCommand) != 0)
	{
		this->getLogManager() << LogLevel_Error << "An error occured while updating the box clock.\n";
		return false;
	}

	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmMatlabScripting::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	
	for(uint32 i = 0; i < getStaticBoxContext().getInputCount(); i++)
	{
		for(uint32 j = 0; j < l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
			m_mDecoders[i]->decode(i,j);
			
			CIdentifier l_oType;
			getStaticBoxContext().getInputType(i,l_oType);
			boolean l_bUnkownStream = true;

			if(m_mDecoders[i]->isHeaderReceived())
			{
				if(l_oType == OV_TypeId_StreamedMatrix)
				{
					IMatrix * l_pMatrix = ((TStreamedMatrixDecoder<CBoxAlgorithmMatlabScripting> *) m_mDecoders[i])->getOutputMatrix();
					CHECK_FAILURE_ROUTINE(m_oMatlabHelper.setStreamedMatrixInputHeader(i,l_pMatrix),"Error calling [OV_setStreamMatrixInputHeader]\n");
					
					m_NbInputHeaderSent++;
					l_bUnkownStream = false;
				}

				if(l_oType == OV_TypeId_Signal)
				{
					IMatrix * l_pMatrix = ((TSignalDecoder<CBoxAlgorithmMatlabScripting> *) m_mDecoders[i])->getOutputMatrix();
					uint64 l_ui64SamplingRate = ((TSignalDecoder<CBoxAlgorithmMatlabScripting> *) m_mDecoders[i])->getOutputSamplingRate();
					CHECK_FAILURE_ROUTINE(m_oMatlabHelper.setSignalInputHeader(i,l_pMatrix,l_ui64SamplingRate),"Error calling [OV_setSignalInputHeader]\n");
					
					m_NbInputHeaderSent++;
					l_bUnkownStream = false;
				}

				if(l_oType == OV_TypeId_FeatureVector)
				{
					IMatrix * l_pMatrix = ((TFeatureVectorDecoder<CBoxAlgorithmMatlabScripting> *) m_mDecoders[i])->getOutputMatrix();
					CHECK_FAILURE_ROUTINE(m_oMatlabHelper.setFeatureVectorInputHeader(i,l_pMatrix),"Error calling [OV_setFeatureVectorInputHeader]\n");
					
					m_NbInputHeaderSent++;
					l_bUnkownStream = false;
				}

				if(l_oType == OV_TypeId_Spectrum)
				{
					IMatrix * l_pMatrix    = ((TSpectrumDecoder<CBoxAlgorithmMatlabScripting> *) m_mDecoders[i])->getOutputMatrix();
					IMatrix * l_pFreqBands = ((TSpectrumDecoder<CBoxAlgorithmMatlabScripting> *) m_mDecoders[i])->getOutputMinMaxFrequencyBands();
					CHECK_FAILURE_ROUTINE(m_oMatlabHelper.setSpectrumInputHeader(i,l_pMatrix,l_pFreqBands),"Error calling [OV_setSpectrumInputHeader]\n");
					
					m_NbInputHeaderSent++;
					l_bUnkownStream = false;
				}

				if(l_oType == OV_TypeId_ChannelLocalisation)
				{
					IMatrix * l_pMatrix = ((TChannelLocalisationDecoder<CBoxAlgorithmMatlabScripting> *) m_mDecoders[i])->getOutputMatrix();
					boolean l_bDynamic  = ((TChannelLocalisationDecoder<CBoxAlgorithmMatlabScripting> *) m_mDecoders[i])->getOutputDynamic();
					CHECK_FAILURE_ROUTINE(m_oMatlabHelper.setChannelLocalisationInputHeader(i,l_pMatrix,l_bDynamic),"Error calling [OV_setChannelLocalizationInputHeader]\n");
					
					m_NbInputHeaderSent++;
					l_bUnkownStream = false;
				}

				if(l_oType == OV_TypeId_ExperimentationInformation)
				{
					this->getLogManager() << LogLevel_Warning << "The Experiment Information Stream is not implemented with the Matlab Scripting Box. If this is relevant for your usage, please contact the official development Team.\n";
					m_NbInputHeaderSent++;
					l_bUnkownStream = false;
				}

				if(l_oType == OV_TypeId_Stimulations)
				{
					CHECK_FAILURE_ROUTINE(m_oMatlabHelper.setStimulationsInputHeader(i),"Error calling [OV_setStimulationsInputHeader]\n");
					
					m_NbInputHeaderSent++;
					l_bUnkownStream = false;
				}
			}

			if(m_mDecoders[i]->isBufferReceived())
			{
				// 
				if(l_oType == OV_TypeId_StreamedMatrix || this->getTypeManager().isDerivedFromStream(l_oType, OV_TypeId_StreamedMatrix))
				{
					IMatrix * l_pMatrix = ((TSignalDecoder<CBoxAlgorithmMatlabScripting> *) m_mDecoders[i])->getOutputMatrix();
					uint64 l_ui64StartTime = l_rDynamicBoxContext.getInputChunkStartTime(i,j);
					uint64 l_ui64EndTime = l_rDynamicBoxContext.getInputChunkEndTime(i,j);
					CHECK_FAILURE_ROUTINE(m_oMatlabHelper.addStreamedMatrixInputBuffer(i,l_pMatrix,l_ui64StartTime,l_ui64EndTime),"Error calling [OV_addInputBuffer (Streamed Matrix or child stream)]\n");
					
					l_bUnkownStream = false;
				}

				if(l_oType == OV_TypeId_Stimulations)
				{
					IStimulationSet * l_pStimSet = ((TStimulationDecoder<CBoxAlgorithmMatlabScripting> *) m_mDecoders[i])->getOutputStimulationSet();
					uint64 l_ui64StartTime = l_rDynamicBoxContext.getInputChunkStartTime(i,j);
					uint64 l_ui64EndTime = l_rDynamicBoxContext.getInputChunkEndTime(i,j);
					CHECK_FAILURE_ROUTINE(m_oMatlabHelper.addStimulationsInputBuffer(i,l_pStimSet,l_ui64StartTime,l_ui64EndTime),"Error calling [OV_addInputBuffer (Stimulations)]\n");
					l_bUnkownStream = false;
				}
				
			}

			if(l_bUnkownStream)
			{
				this->getLogManager() << LogLevel_Error << "Unkown Stream Type on input ["<<i<<"].\n";
				return false;
			}
		}
	}

	if(m_NbInputHeaderSent < getStaticBoxContext().getInputCount())
	{
		// not ready to process 
		return true;
	}


	// CALL TO PROCESS FUNCTION
	char l_sBuffer[512];
	sprintf(l_sBuffer,"%s = %s(%s);", (const char*) m_sBoxInstanceVariableName, (const char*)m_sProcessFunction, (const char*) m_sBoxInstanceVariableName);
	CHECK_FAILURE_ROUTINE(::engEvalString(m_pMatlabEngine, l_sBuffer) == 0,"Error calling the Process function.\n");

	
	// Checking every output in the matlab box to copy them in the openvibe box.
	for(uint32 i = 0; i < getStaticBoxContext().getOutputCount(); i++)
	{
		// now we check for pending output chunk to be sent (output type independent call)
		sprintf(l_sBuffer,"OV_PENDING_COUNT_TMP = OV_getNbPendingOutputChunk(%s, %i);",(const char*) m_sBoxInstanceVariableName, i+1);
		CHECK_FAILURE_ROUTINE(::engEvalString(m_pMatlabEngine, l_sBuffer) == 0,"Error calling [OV_getNbPendingOutputChunk].\n");

		mxArray * l_pPending = ::engGetVariable(m_pMatlabEngine,"OV_PENDING_COUNT_TMP");
		double   l_dPending  = *mxGetPr(l_pPending);
		
		CIdentifier l_oType;
		getStaticBoxContext().getOutputType(i,l_oType);

		for(uint32 c = 0; c < (uint32)l_dPending; c++)
		{
			// If no header were ever sent, we need to extract header information in the matlab box
			// This header must have been set prior to sending the very first buffer.
			if(!m_mOutputHeaderState[i])
			{
				boolean l_bUnkownType = true;
				if(l_oType == OV_TypeId_StreamedMatrix)
				{
					((TStreamedMatrixEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputMatrix() = new CMatrix();
					IMatrix * l_pMatrixToSend = ((TStreamedMatrixEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputMatrix();
					CHECK_FAILURE_ROUTINE(m_oMatlabHelper.setStreamedMatrixOutputHeader(i,l_pMatrixToSend),"Error calling [OV_getStreamedMatrixOutputHeader]. Did you correctly set the output header in the matlab structure ?\n");

					l_bUnkownType = false;
				}
				if(l_oType == OV_TypeId_Signal)
				{
					((TSignalEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputMatrix() = new CMatrix();
					IMatrix * l_pMatrixToSend = ((TSignalEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputMatrix();
					uint64 l_ui64SamplingRate =  ((TSignalEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputSamplingRate();
					CHECK_FAILURE_ROUTINE(m_oMatlabHelper.setSignalOutputHeader(i,l_pMatrixToSend,l_ui64SamplingRate),"Error calling [OV_getSignalOutputHeader]. Did you correctly set the output header in the matlab structure ?\n");

					l_bUnkownType = false;
				}

				if(l_oType == OV_TypeId_FeatureVector)
				{
					((TFeatureVectorEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputMatrix() = new CMatrix();
					IMatrix * l_pMatrixToSend = ((TFeatureVectorEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputMatrix();
					CHECK_FAILURE_ROUTINE(m_oMatlabHelper.setFeatureVectorOutputHeader(i,l_pMatrixToSend),"Error calling [OV_getFeatureVectorOutputHeader]. Did you correctly set the output header in the matlab structure ?\n");

					l_bUnkownType = false;
				}

				if(l_oType == OV_TypeId_Spectrum)
				{
					((TSpectrumEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputMatrix() = new CMatrix();
					((TSpectrumEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputMinMaxFrequencyBands() = new CMatrix();
					IMatrix * l_pMatrixToSend = ((TSpectrumEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputMatrix();
					IMatrix * l_pBands        = ((TSpectrumEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputMinMaxFrequencyBands();
					CHECK_FAILURE_ROUTINE(m_oMatlabHelper.setSpectrumOutputHeader(i,l_pMatrixToSend,l_pBands),"Error calling [OV_getSpectrumOutputHeader]. Did you correctly set the output header in the matlab structure ?\n");

					l_bUnkownType = false;
				}

				if(l_oType == OV_TypeId_ChannelLocalisation)
				{
					((TChannelLocalisationEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputMatrix() = new CMatrix();
					IMatrix * l_pMatrixToSend = ((TChannelLocalisationEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputMatrix();
					boolean   l_bDynamic      = ((TChannelLocalisationEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputDynamic();
					CHECK_FAILURE_ROUTINE(m_oMatlabHelper.setChannelLocalisationOutputHeader(i,l_pMatrixToSend,l_bDynamic),"Error calling [OV_getChannelLocalizationOutputHeader]. Did you correctly set the output header in the matlab structure ?\n");

					l_bUnkownType = false;
				}

				if(l_oType == OV_TypeId_ExperimentationInformation)
				{
					this->getLogManager() << LogLevel_Warning << "The Experiment Information Stream is not implemented with the Matlab Scripting Box. If this is relevant for your usage, please contact the official development Team.\n";
					l_bUnkownType = false;
				}

				if(l_oType == OV_TypeId_Stimulations)
				{
					((TStimulationEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputStimulationSet() = new CStimulationSet();
					IStimulationSet * l_pStimSet = ((TStimulationEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputStimulationSet();
					CHECK_FAILURE_ROUTINE(m_oMatlabHelper.setStimulationsOutputHeader(i, l_pStimSet),"Error calling [OV_getStimulationsOutputHeader]. Did you correctly set the output header in the matlab structure ?\n");

					l_bUnkownType = false;
				}

				
				if(l_bUnkownType)
				{
					this->getLogManager() << LogLevel_Error << "Unkown Stream Type on output ["<<i<<"].\n";
					return false;
				}

				m_mEncoders[i]->encodeHeader(i);
				l_rDynamicBoxContext.markOutputAsReadyToSend(i,0,0);


				m_mOutputHeaderState[i] = true;
			}

			
			boolean l_bUnkownType = true;
			uint64 l_ui64StartTime = 0;
			uint64 l_ui64EndTime   = 0;

			if(l_oType == OV_TypeId_StreamedMatrix || this->getTypeManager().isDerivedFromStream(l_oType, OV_TypeId_StreamedMatrix))
			{
				IMatrix * l_pMatrixToSend = ((TStreamedMatrixEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputMatrix();
				CHECK_FAILURE_ROUTINE(m_oMatlabHelper.popStreamedMatrixOutputBuffer(i,l_pMatrixToSend,l_ui64StartTime,l_ui64EndTime),"Error calling [OV_popOutputBufferReshape] for Streamed Matrix stream or child stream.\n");

				l_bUnkownType = false;
			}

			if(l_oType == OV_TypeId_Stimulations)
			{
				IStimulationSet * l_pStimSet = ((TStimulationEncoder<CBoxAlgorithmMatlabScripting> *) m_mEncoders[i])->getInputStimulationSet();
				l_pStimSet->clear();
				CHECK_FAILURE_ROUTINE(m_oMatlabHelper.popStimulationsOutputBuffer(i,l_pStimSet,l_ui64StartTime,l_ui64EndTime),"Error calling [OV_popOutputBuffer] for Stimulation stream.\n");
				l_bUnkownType = false;
			}

			if(l_bUnkownType)
			{
				this->getLogManager() << LogLevel_Error << "Unkown Stream Type on output ["<<i<<"].\n";
				return false;
			}

			m_mEncoders[i]->encodeBuffer(i);
			l_rDynamicBoxContext.markOutputAsReadyToSend(i,l_ui64StartTime,l_ui64EndTime);

		}
		mxDestroyArray(l_pPending);
		
	}

	return true;
}

boolean CBoxAlgorithmMatlabScripting::printOutputBufferWithFormat()
{
	// the buffer for the console
	std::stringstream l_ssMatlabBuffer;
	l_ssMatlabBuffer<<m_sMatlabBuffer;
	if(l_ssMatlabBuffer.str().size()>0)
	{
		size_t l_oErrorIndex=l_ssMatlabBuffer.str().find("??? ");
		size_t l_oWarningIndex=l_ssMatlabBuffer.str().find("Warning: ");
		if(l_oErrorIndex!=0 && l_oWarningIndex!=0)
		{
			this->getLogManager()<<LogLevel_Info<< "\n---- MATLAB BUFFER - INFO ----\n"<<l_ssMatlabBuffer.str().substr(0,(l_oWarningIndex<l_oErrorIndex)?l_oWarningIndex:l_oErrorIndex).c_str()<<"\n";
			//this->getLogManager()<<LogLevel_Info<< "-------- END OF BUFFER --------\n";
		}
		if(l_oWarningIndex!=std::string::npos)
		{
			this->getLogManager()<<LogLevel_Warning<< "\n---- MATLAB BUFFER - WARNING ----\n"<<l_ssMatlabBuffer.str().substr(l_oWarningIndex,l_oErrorIndex).c_str()<<"\n";
			//this->getLogManager()<<LogLevel_Warning<< "-------- END OF BUFFER --------\n";
		}
		if(l_oErrorIndex!=std::string::npos)
		{
			this->getLogManager()<<LogLevel_Error<< "\n---- MATLAB BUFFER - ERROR ! ----\n"<< l_ssMatlabBuffer.str().substr(l_oErrorIndex).c_str()<<"\n";
			//this->getLogManager()<<LogLevel_Error<< "-------- END OF BUFFER --------\n";
			m_bErrorDetected = true;
		}
	}
	return true;
}

#endif // TARGET_HAS_ThirdPartyMatlab
