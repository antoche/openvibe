#include "ovpCBoxAlgorithmPython.h"

#if defined TARGET_HAS_ThirdPartyPython

#include <iostream>



using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Python;

using namespace OpenViBEToolkit;

OpenViBE::boolean CBoxAlgorithmPython::m_bPythonInitialized = false;
uint32 CBoxAlgorithmPython::m_ui32PythonBoxInstanceCount = 0;
PyObject *CBoxAlgorithmPython::m_pMainModule = NULL;
PyObject *CBoxAlgorithmPython::m_pMainDictionnary = NULL;
PyObject *CBoxAlgorithmPython::m_pOVStreamedMatrixHeader = NULL;
PyObject *CBoxAlgorithmPython::m_pOVStreamedMatrixBuffer = NULL;
PyObject *CBoxAlgorithmPython::m_pOVStreamedMatrixEnd = NULL;
PyObject *CBoxAlgorithmPython::m_pOVSignalHeader = NULL;
PyObject *CBoxAlgorithmPython::m_pOVSignalBuffer = NULL;
PyObject *CBoxAlgorithmPython::m_pOVSignalEnd = NULL;
PyObject *CBoxAlgorithmPython::m_pOVStimulationHeader = NULL;
PyObject *CBoxAlgorithmPython::m_pOVStimulation = NULL;
PyObject *CBoxAlgorithmPython::m_pOVStimulationSet = NULL;
PyObject *CBoxAlgorithmPython::m_pOVStimulationEnd = NULL;
PyObject *CBoxAlgorithmPython::m_pOVBuffer = NULL;
PyObject *CBoxAlgorithmPython::m_pExecFileFunction = NULL;
PyObject *CBoxAlgorithmPython::m_pSysStdout = NULL;
PyObject *CBoxAlgorithmPython::m_pSysStderr = NULL;

OpenViBE::boolean CBoxAlgorithmPython::logSysStdout(void)
{
	//New reference
	PyObject* l_pPyStringToLog = PyObject_CallMethod(m_pSysStdout, (char *) "getvalue", NULL);
	if (l_pPyStringToLog == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to call sys.stdout.getvalue().\n";
		Py_CLEAR(l_pPyStringToLog);
		return false;
	}
	
	char *l_StringToLog = PyString_AsString(l_pPyStringToLog);
	if(l_StringToLog == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to convert l_pPyStringToLog to (char *).\n";
		Py_CLEAR(l_pPyStringToLog);
		return false;
	}
	Py_CLEAR(l_pPyStringToLog);
	
	if( strlen(const_cast<char *>(l_StringToLog)) > 0)
	{
		this->getLogManager() << LogLevel_Info << l_StringToLog;
		
		PyObject* l_pResult = PyObject_CallMethod(m_pSysStdout, (char *) "flush", NULL);
		if (l_pResult == NULL)
		{
			this->getLogManager() << LogLevel_Error << "Failed to call sys.stdout.flush().\n";
			Py_CLEAR(l_pResult);
			return false;
		}
		Py_CLEAR(l_pResult);
	}
	return true;
}

OpenViBE::boolean CBoxAlgorithmPython::logSysStderr(void)
{
	//New reference
	PyObject* l_pPyStringToLog = PyObject_CallMethod(m_pSysStderr, (char *) "getvalue", NULL);
	if (l_pPyStringToLog == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to call sys.stderr.getvalue().\n";
		Py_CLEAR(l_pPyStringToLog);
		return false;
	}
	
	char *l_StringToLog = PyString_AsString(l_pPyStringToLog);
	if(l_StringToLog == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to convert l_pPyStringToLog to (char *).\n";
		Py_CLEAR(l_pPyStringToLog);
		return false;
	}
	Py_CLEAR(l_pPyStringToLog);
	
	if( strlen(const_cast<char *>(l_StringToLog)) > 0)
	{
		this->getLogManager() << LogLevel_Error << l_StringToLog;
		
		PyObject* l_pResult = PyObject_CallMethod(m_pSysStderr, (char *) "flush", NULL);
		if (l_pResult == NULL)
		{
			this->getLogManager() << LogLevel_Error << "Failed to call sys.stderr.flush().\n";
			Py_CLEAR(l_pResult);
			return false;
		}
		Py_CLEAR(l_pResult);
	}
	return true;
}

uint64 CBoxAlgorithmPython::getClockFrequency(void)
{
	getLogManager() << LogLevel_Trace << "Clock frequency requested at time " << getPlayerContext().getCurrentTime() << "\n";

	return m_ui64ClockFrequency<<32;
}

void CBoxAlgorithmPython::buildPythonSettings(void)
{  
  IBox * l_rStaticBoxContext=getBoxAlgorithmContext()->getStaticBoxContext();
  for (uint32 i=2; i<l_rStaticBoxContext->getSettingCount(); i++)
  {
    CString l_sName;
    l_rStaticBoxContext->getSettingName(i, l_sName);

    CString l_sValue;
    l_rStaticBoxContext->getSettingValue(i, l_sValue);

    PyDict_SetItemString(m_pBoxSetting, l_sName, PyString_FromString(l_sValue));
  }
}

OpenViBE::boolean CBoxAlgorithmPython::initializePythonSafely()
{
	if (m_bPythonInitialized)
	{
		return true;
	}

	m_bPythonInitialized = true;

	PyRun_SimpleString(
				"import sys\n"
				"sys.path.append('../share/openvibe-plugins/python')\n"
				"sys.argv = [\"openvibe\"]\n"
				//"import openvibe\n"
				//"from StimulationsCodes import *\n"
				);

	//Borrowed reference
	m_pMainModule = PyImport_AddModule("__main__");
	//Borrowed reference
	m_pMainDictionnary = PyModule_GetDict(m_pMainModule);

	//Execute the script which contains the different classes to interact with OpenViBE
	//New reference
	PyObject *l_pScriptFile = PyFile_FromString((char *) "../share/openvibe-plugins/python/openvibe.py", (char *) "r");
	if (l_pScriptFile == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to open " << "../share/openvibe-plugins/python/openvibe.py" << ".\n";
		Py_CLEAR(l_pScriptFile);
		return false;
	}

	if (PyRun_SimpleFile(PyFile_AsFile(l_pScriptFile), "../share/openvibe-plugins/python/openvibe.py") == -1)
	{
		this->getLogManager() << LogLevel_Error << "Failed to run " << "../share/openvibe-plugins/python/openvibe.py" << ".\n";
		Py_CLEAR(l_pScriptFile);
		return false;
	}
	Py_CLEAR(l_pScriptFile);

	//Borrowed reference
	m_pSysStdout = PySys_GetObject((char *) "stdout");
	if (m_pSysStdout == NULL)
	{
		this->getLogManager() << LogLevel_Error << "sys.stdout does not exist.\n";
		return false;
	}
	//Borrowed reference
	m_pSysStderr = PySys_GetObject((char *) "stderr");
	if (m_pSysStderr == NULL)
	{
		this->getLogManager() << LogLevel_Error << "sys.stderr does not exist.\n";
		return false;
	}

	//Borrowed reference
	m_pExecFileFunction = PyDict_GetItemString(m_pMainDictionnary, "execfileHandlingException");
	if (m_pExecFileFunction == NULL  )
	{
		this->getLogManager() << LogLevel_Error << "openvibe.py doesn't have a execfileHandlingException function, quitting"  << ".\n";
		return false;
	}

	if (!PyCallable_Check(m_pExecFileFunction)){
		this->getLogManager() << LogLevel_Error << "openvibe.py doesn't have a execfileHandlingException function callable, "  << ".\n";
		return false;
	}

	//New reference
	m_pOVStreamedMatrixHeader = PyDict_GetItemString(m_pMainDictionnary, "OVStreamedMatrixHeader");
	if (m_pOVStreamedMatrixHeader == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVStreamedMatrixHeader\".\n";
		Py_CLEAR(m_pOVStreamedMatrixHeader);
		return false;
	}


	//New reference
	m_pOVStreamedMatrixBuffer = PyDict_GetItemString(m_pMainDictionnary, "OVStreamedMatrixBuffer");
	if (m_pOVStreamedMatrixBuffer == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVStreamedMatrixBuffer\".\n";
		Py_CLEAR(m_pOVStreamedMatrixHeader);
		Py_CLEAR(m_pOVStreamedMatrixBuffer);
		return false;
	}

	//New reference
	m_pOVStreamedMatrixEnd = PyDict_GetItemString(m_pMainDictionnary, "OVStreamedMatrixEnd");
	if (m_pOVStreamedMatrixEnd == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVStreamedMatrixEnd\".\n";
		Py_CLEAR(m_pOVStreamedMatrixHeader);
		Py_CLEAR(m_pOVStreamedMatrixBuffer);
		Py_CLEAR(m_pOVStreamedMatrixEnd);
		return false;
	}

	//New reference
	m_pOVSignalHeader = PyDict_GetItemString(m_pMainDictionnary, "OVSignalHeader");
	if (m_pOVSignalHeader == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVSignalHeader\".\n";
		Py_CLEAR(m_pOVStreamedMatrixHeader);
		Py_CLEAR(m_pOVStreamedMatrixBuffer);
		Py_CLEAR(m_pOVStreamedMatrixEnd);
		Py_CLEAR(m_pOVSignalHeader);
		return false;
	}

	//New reference
	m_pOVSignalBuffer = PyDict_GetItemString(m_pMainDictionnary, "OVSignalBuffer");
	if (m_pOVSignalBuffer == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVSignalBuffer\".\n";
		Py_CLEAR(m_pOVStreamedMatrixHeader);
		Py_CLEAR(m_pOVStreamedMatrixBuffer);
		Py_CLEAR(m_pOVStreamedMatrixEnd);
		Py_CLEAR(m_pOVSignalHeader);
		Py_CLEAR(m_pOVSignalBuffer);
		return false;
	}

	//New reference
	m_pOVSignalEnd = PyDict_GetItemString(m_pMainDictionnary, "OVSignalEnd");
	if (m_pOVSignalEnd == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVSignalEnd\".\n";
		Py_CLEAR(m_pOVStreamedMatrixHeader);
		Py_CLEAR(m_pOVStreamedMatrixBuffer);
		Py_CLEAR(m_pOVStreamedMatrixEnd);
		Py_CLEAR(m_pOVSignalHeader);
		Py_CLEAR(m_pOVSignalBuffer);
		Py_CLEAR(m_pOVSignalEnd);
		return false;
	}

	//New reference
	m_pOVStimulationHeader = PyDict_GetItemString(m_pMainDictionnary, "OVStimulationHeader");
	if (m_pOVStimulationHeader == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVStimulationHeader\".\n";
		Py_CLEAR(m_pOVStreamedMatrixHeader);
		Py_CLEAR(m_pOVStreamedMatrixBuffer);
		Py_CLEAR(m_pOVStreamedMatrixEnd);
		Py_CLEAR(m_pOVSignalHeader);
		Py_CLEAR(m_pOVSignalBuffer);
		Py_CLEAR(m_pOVSignalEnd);
		Py_CLEAR(m_pOVStimulationHeader);
		return false;
	}

	//New reference
	m_pOVStimulation = PyDict_GetItemString(m_pMainDictionnary, "OVStimulation");
	if (m_pOVStimulation == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVStimulation\".\n";
		Py_CLEAR(m_pOVStreamedMatrixHeader);
		Py_CLEAR(m_pOVStreamedMatrixBuffer);
		Py_CLEAR(m_pOVStreamedMatrixEnd);
		Py_CLEAR(m_pOVSignalHeader);
		Py_CLEAR(m_pOVSignalBuffer);
		Py_CLEAR(m_pOVSignalEnd);
		Py_CLEAR(m_pOVStimulationHeader);
		Py_CLEAR(m_pOVStimulation);
		return false;
	}

	//New reference
	m_pOVStimulationSet = PyDict_GetItemString(m_pMainDictionnary, "OVStimulationSet");
	if (m_pOVStimulationSet == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVStimulationSet\".\n";
		Py_CLEAR(m_pOVStreamedMatrixHeader);
		Py_CLEAR(m_pOVStreamedMatrixBuffer);
		Py_CLEAR(m_pOVStreamedMatrixEnd);
		Py_CLEAR(m_pOVSignalHeader);
		Py_CLEAR(m_pOVSignalBuffer);
		Py_CLEAR(m_pOVSignalEnd);
		Py_CLEAR(m_pOVStimulationHeader);
		Py_CLEAR(m_pOVStimulation);
		Py_CLEAR(m_pOVStimulationSet);
		return false;
	}

	//New reference
	m_pOVStimulationEnd = PyDict_GetItemString(m_pMainDictionnary, "OVStimulationEnd");
	if (m_pOVStimulationEnd == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVStimulationEnd\".\n";
		Py_CLEAR(m_pOVStreamedMatrixHeader);
		Py_CLEAR(m_pOVStreamedMatrixBuffer);
		Py_CLEAR(m_pOVStreamedMatrixEnd);
		Py_CLEAR(m_pOVSignalHeader);
		Py_CLEAR(m_pOVSignalBuffer);
		Py_CLEAR(m_pOVSignalEnd);
		Py_CLEAR(m_pOVStimulationHeader);
		Py_CLEAR(m_pOVStimulation);
		Py_CLEAR(m_pOVStimulationSet);
		Py_CLEAR(m_pOVStimulationEnd);
		return false;
	}

	//New reference
	m_pOVBuffer = PyDict_GetItemString(m_pMainDictionnary, "OVBuffer");
	if (m_pOVBuffer == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVBuffer\".\n";
		Py_CLEAR(m_pOVStreamedMatrixHeader);
		Py_CLEAR(m_pOVStreamedMatrixBuffer);
		Py_CLEAR(m_pOVStreamedMatrixEnd);
		Py_CLEAR(m_pOVSignalHeader);
		Py_CLEAR(m_pOVSignalBuffer);
		Py_CLEAR(m_pOVSignalEnd);
		Py_CLEAR(m_pOVStimulationHeader);
		Py_CLEAR(m_pOVStimulation);
		Py_CLEAR(m_pOVStimulationSet);
		Py_CLEAR(m_pOVStimulationEnd);
		Py_CLEAR(m_pOVBuffer);
		return false;
	}

	this->getLogManager() << LogLevel_Info << "Python Interpreter initialized\n";

return true;
}

OpenViBE::boolean CBoxAlgorithmPython::initialize(void)
{
	if (!initializePythonSafely())
	{
		return false;
	}

	m_ui32PythonBoxInstanceCount++;

	//Initialize the clock frequency of the box depending on the first setting of the box
	CString l_sSettingValue;
	getStaticBoxContext().getSettingValue(0, l_sSettingValue);
	m_ui64ClockFrequency=::atoi(l_sSettingValue.toASCIIString());

	getStaticBoxContext().getSettingValue(1, m_sScriptFilename);
	if(strlen(m_sScriptFilename.toASCIIString()) == 0)
	{
		this->getLogManager() << LogLevel_Error << "You have to choose a script.\n";
		return false;
	}

	//Create the decoders for the inputs
	IBox& l_rStaticBoxContext = this->getStaticBoxContext();
	OpenViBE::CIdentifier l_oTypeIdentifier;
	for(uint32 input=0; input<l_rStaticBoxContext.getInputCount(); input++)
	{
		l_rStaticBoxContext.getInputType(input, l_oTypeIdentifier);
		if (l_oTypeIdentifier == OV_TypeId_StreamedMatrix)
		{
			m_vDecoders.push_back( new TStreamedMatrixDecoder <CBoxAlgorithmPython> (*this) );
		}
		else if (l_oTypeIdentifier == OV_TypeId_Signal)
		{
			m_vDecoders.push_back( new TSignalDecoder <CBoxAlgorithmPython> (*this) );
		}
		else if (l_oTypeIdentifier == OV_TypeId_FeatureVector)
		{
			m_vDecoders.push_back( new TFeatureVectorDecoder <CBoxAlgorithmPython> (*this) );
		}
		else if (l_oTypeIdentifier == OV_TypeId_Spectrum)
		{
			m_vDecoders.push_back( new TSpectrumDecoder <CBoxAlgorithmPython> (*this) );
		}
		else if (l_oTypeIdentifier == OV_TypeId_ChannelLocalisation)
		{
			m_vDecoders.push_back( new TChannelLocalisationDecoder <CBoxAlgorithmPython> (*this) );
		}
		else if (l_oTypeIdentifier == OV_TypeId_Stimulations)
		{
			m_vDecoders.push_back( new TStimulationDecoder <CBoxAlgorithmPython> (*this) );
		}
		else if (l_oTypeIdentifier == OV_TypeId_ExperimentationInformation)
		{
			m_vDecoders.push_back( new TExperimentInformationDecoder <CBoxAlgorithmPython> (*this) );
		}
		else
		{
			this->getLogManager() << LogLevel_Error << "Codec to decode " << l_oTypeIdentifier.toString() << " is not implemented.\n";
			return false;
		}
	}

	//Create the encoders for the outputs
	for(uint32 output=0; output<l_rStaticBoxContext.getOutputCount(); output++)
	{
		l_rStaticBoxContext.getOutputType(output, l_oTypeIdentifier);
		if (l_oTypeIdentifier == OV_TypeId_StreamedMatrix)
		{
			m_vEncoders.push_back( new TStreamedMatrixEncoder <CBoxAlgorithmPython> (*this) );
		}
		else if (l_oTypeIdentifier == OV_TypeId_Signal)
		{
			m_vEncoders.push_back( new TSignalEncoder <CBoxAlgorithmPython> (*this) );
		}
		else if (l_oTypeIdentifier == OV_TypeId_FeatureVector)
		{
			m_vEncoders.push_back( new TFeatureVectorEncoder <CBoxAlgorithmPython> (*this) );
		}
		else if (l_oTypeIdentifier == OV_TypeId_Spectrum)
		{
			m_vEncoders.push_back( new TSpectrumEncoder <CBoxAlgorithmPython> (*this) );
		}
		else if (l_oTypeIdentifier == OV_TypeId_ChannelLocalisation)
		{
			m_vEncoders.push_back( new TChannelLocalisationEncoder <CBoxAlgorithmPython> (*this) );
		}
		else if (l_oTypeIdentifier == OV_TypeId_Stimulations)
		{
			m_vEncoders.push_back( new TStimulationEncoder <CBoxAlgorithmPython> (*this) );
		}
		else if (l_oTypeIdentifier == OV_TypeId_ExperimentationInformation)
		{
			m_vEncoders.push_back( new TExperimentInformationEncoder <CBoxAlgorithmPython> (*this) );
		}
		else
		{
			this->getLogManager() << LogLevel_Error << "Codec to encode " << l_oTypeIdentifier.toString() << " is not implemented.\n";
			return false;
		}
	}



	m_bInitializeSucceeded = false;
	m_pBoxInitialize = NULL ;
	m_pBoxProcess = NULL;
	m_pBoxUninitialize = NULL ;


	//New reference
	PyObject *l_pTemporyPyObject = Py_BuildValue("s,O", m_sScriptFilename.toASCIIString(), m_pMainDictionnary);
	//New reference
	PyObject *l_pResult = PyObject_CallObject(m_pExecFileFunction, l_pTemporyPyObject);
	if (l_pResult == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to run " << m_sScriptFilename << ".\n";
		Py_CLEAR(l_pTemporyPyObject);
		Py_CLEAR(l_pResult);
		return false;
	}
	Py_CLEAR(l_pTemporyPyObject);
	Py_CLEAR(l_pResult);


	/*
	PyObject *l_pScriptFile = PyFile_FromString((char *) m_sScriptFilename.toASCIIString(), (char *) "r");
	if (l_pScriptFile == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to open " << m_sScriptFilename.toASCIIString() << ".\n";
		Py_CLEAR(l_pScriptFile);
		return false;
	}

	if (PyRun_SimpleFile(PyFile_AsFile(l_pScriptFile), m_sScriptFilename.toASCIIString()) == -1)
	{
		this->getLogManager() << LogLevel_Error << "Failed to run " << m_sScriptFilename.toASCIIString() << ".\n";
		Py_CLEAR(l_pScriptFile);
		return false;
	}
	Py_CLEAR(l_pScriptFile);
	*/

	//New reference
	m_pBox = PyObject_GetAttrString(m_pMainModule, "box"); // la box qui vient juste d'etre creee
	if (m_pBox == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load \"box\" object.\n";
		Py_CLEAR(m_pBox);
		return false;
	}
	//New reference
	m_pBoxInput = PyObject_GetAttrString(m_pBox, "input");
	if (m_pBoxInput == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load \"box.input\".\n";
		Py_CLEAR(m_pBox);
		Py_CLEAR(m_pBoxInput);
		return false;
	}
	//New reference
	m_pBoxOutput = PyObject_GetAttrString(m_pBox, "output");
	if (m_pBoxOutput == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load \"box.output\".\n";
		Py_CLEAR(m_pBox);
		Py_CLEAR(m_pBoxInput);
		Py_CLEAR(m_pBoxOutput);
		return false;
	}

	CString l_sInputOutputType;
	for(uint32 input=0; input<l_rStaticBoxContext.getInputCount(); input++)
	{
		OpenViBE::CIdentifier l_oTypeIdentifier;
		l_rStaticBoxContext.getInputType(input, l_oTypeIdentifier);
		if (l_oTypeIdentifier == OV_TypeId_StreamedMatrix)
		{
			l_sInputOutputType = "StreamedMatrix";
		}
		else if (l_oTypeIdentifier == OV_TypeId_Signal)
		{
			l_sInputOutputType = "Signal";
		}
		else if (l_oTypeIdentifier == OV_TypeId_FeatureVector)
		{
			l_sInputOutputType = "FeatureVector";
		}
		else if (l_oTypeIdentifier == OV_TypeId_Spectrum)
		{
			l_sInputOutputType = "Spectrum";
		}
		else if (l_oTypeIdentifier == OV_TypeId_ChannelLocalisation)
		{
			l_sInputOutputType = "ChannelLocalisation";
		}
		else if (l_oTypeIdentifier == OV_TypeId_Stimulations)
		{
			l_sInputOutputType = "Stimulations";
		}
		else if (l_oTypeIdentifier == OV_TypeId_ExperimentationInformation)
		{
			l_sInputOutputType = "ExperimentationInformation";
		}
		//New reference
		PyObject* l_pResult = PyObject_CallMethod(m_pBox, (char *) "addInput", "s", const_cast <char*>(l_sInputOutputType.toASCIIString()));
		if (l_pResult == NULL)
		{
			this->getLogManager() << LogLevel_Error << "Failed to call box.addInput().\n";
			Py_CLEAR(m_pBox);
			Py_CLEAR(m_pBoxInput);
			Py_CLEAR(m_pBoxOutput);
			Py_CLEAR(l_pResult);
			return false;
		}
		Py_CLEAR(l_pResult);
	}

	for(uint32 output=0; output<l_rStaticBoxContext.getOutputCount(); output++)
	{
		OpenViBE::CIdentifier l_oTypeIdentifier;
		l_rStaticBoxContext.getOutputType(output, l_oTypeIdentifier);
		if (l_oTypeIdentifier == OV_TypeId_StreamedMatrix)
		{
			l_sInputOutputType = "StreamedMatrix";
		}
		else if (l_oTypeIdentifier == OV_TypeId_Signal)
		{
			l_sInputOutputType = "Signal";
		}
		else if (l_oTypeIdentifier == OV_TypeId_FeatureVector)
		{
			l_sInputOutputType = "FeatureVector";
		}
		else if (l_oTypeIdentifier == OV_TypeId_Spectrum)
		{
			l_sInputOutputType = "Spectrum";
		}
		else if (l_oTypeIdentifier == OV_TypeId_ChannelLocalisation)
		{
			l_sInputOutputType = "ChannelLocalisation";
		}
		else if (l_oTypeIdentifier == OV_TypeId_Stimulations)
		{
			l_sInputOutputType = "Stimulations";
		}
		else if (l_oTypeIdentifier == OV_TypeId_ExperimentationInformation)
		{
			l_sInputOutputType = "ExperimentationInformation";
		}
		//New reference
		PyObject* l_pResult = PyObject_CallMethod(m_pBox, (char *) "addOutput", "s", const_cast <char*>(l_sInputOutputType.toASCIIString()));
		if (l_pResult == NULL)
		{
			this->getLogManager() << LogLevel_Error << "Failed to call box.addOutput().\n";
			Py_CLEAR(m_pBox);
			Py_CLEAR(m_pBoxInput);
			Py_CLEAR(m_pBoxOutput);
			Py_CLEAR(l_pResult);
			return false;
		}
		Py_CLEAR(l_pResult);
	}

	//New reference
	m_pBoxSetting = PyObject_GetAttrString(m_pBox,"setting");
	if (m_pBoxSetting == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load \"box.setting\".\n";
		Py_CLEAR(m_pBox);
		Py_CLEAR(m_pBoxInput);
		Py_CLEAR(m_pBoxOutput);
		Py_CLEAR(m_pBoxSetting);
		return false;
	}
	buildPythonSettings();

	if (!PyObject_HasAttrString(m_pBox, "_clock"))
	{
		this->getLogManager() << LogLevel_Error << "Failed to initialize \"box._clock\" attribute because it does not exist.\n";
		Py_CLEAR(m_pBox);
		Py_CLEAR(m_pBoxInput);
		Py_CLEAR(m_pBoxOutput);
		Py_CLEAR(m_pBoxSetting);
		return false;
	}
	//New reference
	PyObject *l_pBoxClock = PyInt_FromLong((long) m_ui64ClockFrequency);
	if (l_pBoxClock == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to convert m_ui64ClockFrequency into PyInt.\n";
		Py_CLEAR(m_pBox);
		Py_CLEAR(m_pBoxInput);
		Py_CLEAR(m_pBoxOutput);
		Py_CLEAR(m_pBoxSetting);
		Py_CLEAR(l_pBoxClock);
		return false;
	}
	if (PyObject_SetAttrString(m_pBox, "_clock", l_pBoxClock) == -1)
	{
		this->getLogManager() << LogLevel_Error << "Failed to initialize \"box._clock\" attribute.\n";
		Py_CLEAR(m_pBox);
		Py_CLEAR(m_pBoxInput);
		Py_CLEAR(m_pBoxOutput);
		Py_CLEAR(m_pBoxSetting);
		Py_CLEAR(l_pBoxClock);
		return false;
	}
	Py_CLEAR(l_pBoxClock);

	if (!PyObject_HasAttrString(m_pBox, "_currentTime"))
	{
		this->getLogManager() << LogLevel_Error << "Failed to initialize \"box._currentTime\" attribute because it does not exist.\n";
		Py_CLEAR(m_pBox);
		Py_CLEAR(m_pBoxInput);
		Py_CLEAR(m_pBoxOutput);
		Py_CLEAR(m_pBoxSetting);
		return false;
	}
	//New reference
	m_pBoxCurrentTime = PyFloat_FromDouble((double)(this->getPlayerContext().getCurrentTime()>>22) / 1024.);
	if (m_pBoxCurrentTime == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to convert the current time into a PyFloat.\n";
		Py_CLEAR(m_pBox);
		Py_CLEAR(m_pBoxInput);
		Py_CLEAR(m_pBoxOutput);
		Py_CLEAR(m_pBoxSetting);
		Py_CLEAR(m_pBoxCurrentTime);
		return false;
	}
	if (PyObject_SetAttrString(m_pBox, "_currentTime", m_pBoxCurrentTime) == -1)
	{
		this->getLogManager() << LogLevel_Error << "Failed to initialize \"box._currentTime\" attribute.\n";
		Py_CLEAR(m_pBox);
		Py_CLEAR(m_pBoxInput);
		Py_CLEAR(m_pBoxOutput);
		Py_CLEAR(m_pBoxSetting);
		Py_CLEAR(m_pBoxCurrentTime);
		return false;
	}

	if (!PyObject_HasAttrString(m_pBox, "realInitialize"))
	{
		this->getLogManager() << LogLevel_Error << "No realInitialize.\n";
		Py_CLEAR(m_pBox);
		Py_CLEAR(m_pBoxInput);
		Py_CLEAR(m_pBoxOutput);
		Py_CLEAR(m_pBoxSetting);
		Py_CLEAR(m_pBoxCurrentTime);
		return false;
	}

	//New reference
	m_pBoxInitialize = PyObject_GetAttrString(m_pBox, "realInitialize");
	if (m_pBoxInitialize == NULL)
	{
		this->getLogManager() << LogLevel_ImportantWarning << "Failed to load \"box.realInitialize\" function.\n";
	}
	else if (!PyCallable_Check(m_pBoxInitialize))
	{
		this->getLogManager() << LogLevel_ImportantWarning << "\"box.realInitialize\" is not callable.\n";
	}

	//New reference
	m_pBoxProcess = PyObject_GetAttrString(m_pBox, "realProcess");
	if (m_pBoxProcess == NULL)
	{
		this->getLogManager() << LogLevel_ImportantWarning << "Failed to load \"box.realProcess\" function.\n";
	}
	else if (!PyCallable_Check(m_pBoxProcess))
	{
		this->getLogManager() << LogLevel_ImportantWarning << "\"box.realProcess\" is not callable.\n";
	}

	//New reference
	m_pBoxUninitialize = PyObject_GetAttrString(m_pBox, "realUninitialize");
	if (m_pBoxUninitialize == NULL)
	{
		this->getLogManager() << LogLevel_ImportantWarning << "Failed to load \"box.realUninitialize\" function.\n";
	}
	else if (!PyCallable_Check(m_pBoxUninitialize))
	{
		this->getLogManager() << LogLevel_ImportantWarning << "\"box.realUninitialize\" is not callable.\n";
	}

	//Execute the initialize function defined in the python user script
	if (m_pBoxInitialize && PyCallable_Check(m_pBoxInitialize))
	{
		//New reference
		PyObject *l_pResult = PyObject_CallObject(m_pBoxInitialize, NULL);
		OpenViBE::boolean l_bLogSysStdoutError = logSysStdout(); // souci car la si l'init plante pas de sortie au bon endroit
		OpenViBE::boolean l_bLogSysStderrError = logSysStderr();
		if ((l_pResult == NULL) || (!l_bLogSysStdoutError) || (!l_bLogSysStderrError))
		{
			if(l_pResult == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to call \"box.__initialize\" function.\n";
			}
			if(!l_bLogSysStdoutError)
			{
				this->getLogManager() << LogLevel_Error << "logSysStdout() failed during box.__initialization.\n";
			}
			if(!l_bLogSysStderrError)
			{
				this->getLogManager() << LogLevel_Error << "logSysStderr() failed during box.__initialization.\n";
			}
			Py_CLEAR(m_pBox);
			Py_CLEAR(m_pBoxInput);
			Py_CLEAR(m_pBoxOutput);
			Py_CLEAR(m_pBoxCurrentTime);
			Py_CLEAR(m_pBoxSetting);
			Py_CLEAR(m_pBoxInitialize);
			Py_CLEAR(m_pBoxProcess);
			Py_CLEAR(m_pBoxUninitialize);
			Py_CLEAR(l_pResult);
			return false;
		}
		Py_CLEAR(l_pResult);
	}

	m_bInitializeSucceeded = true;
	return true;
}

OpenViBE::boolean CBoxAlgorithmPython::uninitialize(void)
{
	for(uint32 i = 0; i < m_vDecoders.size(); i++)
	{
		m_vDecoders[i]->uninitialize();
	}

	for(uint32 i = 0; i < m_vEncoders.size(); i++)
	{
		m_vEncoders[i]->uninitialize();
	}

	if (m_bInitializeSucceeded)
	{ // we call this uninit only if init had succeeded
		//Execute the uninitialize function defined in the python script
		// il y a un souci ici si le script n'a pas été chargé ça ne passe pas
		if (m_pBoxUninitialize && PyCallable_Check(m_pBoxUninitialize))
		{
			//New reference
			PyObject *l_pResult = PyObject_CallObject(m_pBoxUninitialize, NULL);
			OpenViBE::boolean l_bLogSysStdoutError = logSysStdout();
			OpenViBE::boolean l_bLogSysStderrError = logSysStderr();
			if ((l_pResult == NULL) || (!l_bLogSysStdoutError) || (!l_bLogSysStderrError))
			{
				if(l_pResult == NULL)
				{
					this->getLogManager() << LogLevel_Error << "Failed to call \"box.__uninitialize\" function.\n";
				}
				if(!l_bLogSysStdoutError)
				{
					this->getLogManager() << LogLevel_Error << "logSysStdout() failed during box.__uninitialization.\n";
				}
				if(!l_bLogSysStderrError)
				{
					this->getLogManager() << LogLevel_Error << "logSysStderr() failed during box.__uninitialization.\n";
				}
				Py_CLEAR(m_pBox);
				Py_CLEAR(m_pBoxInput);
				Py_CLEAR(m_pBoxOutput);
				Py_CLEAR(m_pBoxCurrentTime);
				Py_CLEAR(m_pBoxSetting);
				Py_CLEAR(m_pBoxInitialize);
				Py_CLEAR(m_pBoxProcess);
				Py_CLEAR(m_pBoxUninitialize);
				Py_CLEAR(l_pResult);
				return false;
			}
			Py_CLEAR(l_pResult);
		}

		Py_CLEAR(m_pBox);
		Py_CLEAR(m_pBoxInput);
		Py_CLEAR(m_pBoxOutput);
		Py_CLEAR(m_pBoxCurrentTime);
		Py_CLEAR(m_pBoxSetting);
		Py_CLEAR(m_pBoxInitialize);
		Py_CLEAR(m_pBoxProcess);
		Py_CLEAR(m_pBoxUninitialize);
	}

	m_ui32PythonBoxInstanceCount--;

	if (m_ui32PythonBoxInstanceCount == 0 && m_bPythonInitialized)
	{

		Py_CLEAR(m_pOVStreamedMatrixHeader);
		Py_CLEAR(m_pOVStreamedMatrixBuffer);
		Py_CLEAR(m_pOVStreamedMatrixEnd);
		Py_CLEAR(m_pOVSignalHeader);
		Py_CLEAR(m_pOVSignalBuffer);
		Py_CLEAR(m_pOVSignalEnd);
		Py_CLEAR(m_pOVStimulationHeader);
		Py_CLEAR(m_pOVStimulation);
		Py_CLEAR(m_pOVStimulationSet);
		Py_CLEAR(m_pOVStimulationEnd);
		Py_CLEAR(m_pOVBuffer);


		//Py_Finalize();
		m_bPythonInitialized = false;
		this->getLogManager() << LogLevel_Info << "Python Interpreter uninitialized\n";
	}

	return true;
}

OpenViBE::boolean CBoxAlgorithmPython::processClock(CMessageClock& rMessageClock)
{
	this->getLogManager() << LogLevel_Trace << "Received clock message at time " << rMessageClock.getTime() << "\n";
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

OpenViBE::boolean CBoxAlgorithmPython::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

OpenViBE::boolean CBoxAlgorithmPython::transferStreamedMatrixInputChunksToPython(uint32 input_index)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	if (!PyList_Check(m_pBoxInput))
	{
		this->getLogManager() << LogLevel_Error << "box.input must be a list.\n";
		return false;
	}

	//Borrowed reference
	PyObject *l_pBuffer = PyList_GetItem(m_pBoxInput, (Py_ssize_t) input_index);
	if (l_pBuffer == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.input[" << input_index << "].\n";
		return false;
	}
	//Expose input streamed matrix chunks to python
	for(uint32 chunk_index=0; chunk_index < l_rDynamicBoxContext.getInputChunkCount(input_index); chunk_index++)
	{
		m_vDecoders[input_index]->decode(input_index, chunk_index);

		if (m_vDecoders[input_index]->isHeaderReceived())
		{
			uint32 l_ui32DimensionCount, l_ui32DimensionSize;
			IMatrix* l_pMatrix = ( (TStreamedMatrixDecoder <CBoxAlgorithmPython> *) m_vDecoders[input_index] )->getOutputMatrix();
			l_ui32DimensionCount = l_pMatrix->getDimensionCount();

			//New reference
			PyObject *l_pDimensionSize = PyList_New(l_ui32DimensionCount);
			if (l_pDimensionSize == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new list l_pDimensionSize.\n";
				Py_CLEAR(l_pDimensionSize);
				return false;
			}

			//New reference
			PyObject *l_pDimensionLabel = PyList_New(0);
			if (l_pDimensionLabel == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new list l_pDimensionLabel.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				return false;
			}

			for (uint32 i=0; i<l_ui32DimensionCount; i++)
			{
				l_ui32DimensionSize = l_pMatrix->getDimensionSize(i);
				if (PyList_SetItem(l_pDimensionSize, i, PyInt_FromLong(l_ui32DimensionSize)) == -1)
				{
					this->getLogManager() << LogLevel_Error << "Failed to set item " << i << " in dimension size list.\n";
					Py_CLEAR(l_pDimensionSize);
					Py_CLEAR(l_pDimensionLabel);
					return false;
				}
				for (uint32 j=0; j<l_ui32DimensionSize; j++)
				{
					if (PyList_Append(l_pDimensionLabel, PyString_FromString(l_pMatrix->getDimensionLabel(i, j))) == -1)
					{
						this->getLogManager() << LogLevel_Error << "Failed to append \"" << l_pMatrix->getDimensionLabel(i, j) << "\" in dimension label list.\n";
						Py_CLEAR(l_pDimensionSize);
						Py_CLEAR(l_pDimensionLabel);
						return false;
					}
				}
			}

			//New reference
			PyObject *l_pArg = PyTuple_New(4);
			if (l_pArg == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 0, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkStartTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 1, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkEndTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 2, l_pDimensionSize) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 2 (dimension size) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 3, l_pDimensionLabel) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 3 (dimension label) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			//New reference
			PyObject *l_pOVStreamedMatrixHeader = PyObject_Call(m_pOVStreamedMatrixHeader, l_pArg, NULL);
			if (l_pOVStreamedMatrixHeader == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new OVStreamedMatrixHeader l_pOVStreamedMatrixHeader.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				Py_CLEAR(l_pOVStreamedMatrixHeader);
				return false;
			}
			Py_CLEAR(l_pDimensionSize);
			Py_CLEAR(l_pDimensionLabel);
			Py_CLEAR(l_pArg);

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject *l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVStreamedMatrixHeader, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append chunk to box.input[" << input_index << "].\n";
				Py_CLEAR(l_pResult);
				Py_CLEAR(l_pOVStreamedMatrixHeader);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVStreamedMatrixHeader);
		}

		if (m_vDecoders[input_index]->isBufferReceived())
		{
			//New reference
			PyObject *l_pArg = PyTuple_New(3);
			if (l_pArg == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 0, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkStartTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 1, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkEndTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 2, PyList_New(0)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 2 (bufferElements) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}

			//New reference
			PyObject *l_pOVStreamedMatrixBuffer = PyObject_Call(m_pOVStreamedMatrixBuffer, l_pArg, NULL);
			if (l_pOVStreamedMatrixBuffer == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new OVStreamedMatrixBuffer l_pOVStreamedMatrixBuffer.\n";
				Py_CLEAR(l_pArg);
				Py_CLEAR(l_pOVStreamedMatrixBuffer);
				return false;
			}
			Py_CLEAR(l_pArg);

			IMatrix* l_pMatrix = ( (TStreamedMatrixDecoder <CBoxAlgorithmPython> *) m_vDecoders[input_index] )->getOutputMatrix();
			float64* l_pBufferBase = l_pMatrix->getBuffer();
			for (uint32 element_index = 0; element_index < l_pMatrix->getBufferElementCount(); element_index++)
			{
				if (PyList_Append(l_pOVStreamedMatrixBuffer, PyFloat_FromDouble(l_pBufferBase[element_index])) == -1)
				{
					this->getLogManager() << LogLevel_Error << "Failed to append element " << element_index << " to l_pOVStreamedMatrixBuffer.\n";
					Py_CLEAR(l_pOVStreamedMatrixBuffer);
					return false;
				}
			}

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject *l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVStreamedMatrixBuffer, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append an OVStreamedMatrixBuffer to box.input[" << input_index << "].\n";
				Py_CLEAR(l_pResult);
				Py_CLEAR(l_pOVStreamedMatrixBuffer);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVStreamedMatrixBuffer);
		}

		if (m_vDecoders[input_index]->isEndReceived())
		{
			//New reference
			PyObject *l_pArg = PyTuple_New(2);
			if (l_pArg == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 0, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkStartTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 1, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkEndTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}

			//New reference
			PyObject *l_pOVStreamedMatrixEnd = PyObject_Call(m_pOVStreamedMatrixEnd, l_pArg, NULL);
			if (l_pOVStreamedMatrixEnd == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new OVStreamedMatrixEnd l_pOVStreamedMatrixEnd.\n";
				Py_CLEAR(l_pArg);
				Py_CLEAR(l_pOVStreamedMatrixEnd);
				return false;
			}
			Py_CLEAR(l_pArg);

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject *l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVStreamedMatrixEnd, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append an OVStreamedMatrixEnd to box.input[" << input_index << "].\n";
				Py_CLEAR(l_pResult);
				Py_CLEAR(l_pOVStreamedMatrixEnd);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVStreamedMatrixEnd);
		}
	}

	return true;
}

OpenViBE::boolean CBoxAlgorithmPython::transferStreamedMatrixOutputChunksFromPython(uint32 output_index)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	IMatrix* l_pMatrix = ( (TStreamedMatrixEncoder <CBoxAlgorithmPython> *) m_vEncoders[output_index] )->getInputMatrix();

	if (!PyList_Check(m_pBoxOutput))
	{
		this->getLogManager() << LogLevel_Error << "box.output must be a list.\n";
		return false;
	}

	//Borrowed reference
	PyObject *l_pBuffer = PyList_GetItem(m_pBoxOutput, (Py_ssize_t) output_index);
	if (l_pBuffer == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.output[" << output_index << "].\n";
		return false;
	}

	//New reference
	PyObject* l_pBufferLen = PyObject_CallMethod(l_pBuffer, (char *) "__len__", NULL);
	if (l_pBufferLen == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.output["<< output_index << "].__len__().\n";
		Py_CLEAR(l_pBufferLen);
		return false;
	}

	uint32 l_ui32OutputLen = PyInt_AsUnsignedLongMask(l_pBufferLen);
	Py_CLEAR(l_pBufferLen);
	for (uint32 chunk_index=0; chunk_index < l_ui32OutputLen; chunk_index++)
	{
		//New reference
		PyObject* l_pOVChunk = PyObject_CallMethod(l_pBuffer, (char *) "pop", NULL);
		if (l_pOVChunk == NULL)
		{
			this->getLogManager() << LogLevel_Error << "Failed to get item " << chunk_index << " of box.output["<< output_index << "].\n";
			Py_CLEAR(l_pOVChunk);
			return false;
		}

		if (PyObject_IsInstance(l_pOVChunk, m_pOVStreamedMatrixHeader) == 1)
		{
			uint32 l_ui32DimensionCount, l_ui32DimensionSize, l_ui32DimensionIndex, l_ui32DimensionEntryIndex;

			//New reference
			PyObject *l_pDimensionCount = PyObject_CallMethod(l_pOVChunk, (char *) "getDimensionCount", NULL);
			if (l_pDimensionCount == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to call chunk's getDimensionCount method.\n";
				Py_CLEAR(l_pDimensionCount);
				Py_CLEAR(l_pOVChunk);
				return false;
			}
			l_ui32DimensionCount = PyInt_AsUnsignedLongMask(l_pDimensionCount);
			l_pMatrix->setDimensionCount(l_ui32DimensionCount);
			Py_CLEAR(l_pDimensionCount);

			//New reference
			PyObject *l_pDimensionSize = PyObject_GetAttrString(l_pOVChunk, "dimensionSizes");

			//New reference
			PyObject *l_pDimensionLabel = PyObject_GetAttrString(l_pOVChunk, "dimensionLabels");

			uint32 offset = 0;
			for (l_ui32DimensionIndex=0; l_ui32DimensionIndex < l_ui32DimensionCount; l_ui32DimensionIndex++)
			{
				l_ui32DimensionSize = PyInt_AsUnsignedLongMask(PyList_GetItem(l_pDimensionSize, (Py_ssize_t) l_ui32DimensionIndex));
				l_pMatrix->setDimensionSize(l_ui32DimensionIndex, l_ui32DimensionSize);
				for (l_ui32DimensionEntryIndex=0; l_ui32DimensionEntryIndex < l_ui32DimensionSize; l_ui32DimensionEntryIndex++)
				{
					l_pMatrix->setDimensionLabel(l_ui32DimensionIndex, l_ui32DimensionEntryIndex, PyString_AsString(PyList_GetItem(l_pDimensionLabel, offset + l_ui32DimensionEntryIndex)));
				}
				offset = offset + l_ui32DimensionSize;
			}
			Py_CLEAR(l_pDimensionSize);
			Py_CLEAR(l_pDimensionLabel);

			m_vEncoders[output_index]->encodeHeader(output_index);

			//New reference
			PyObject *l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64 l_ui64StartTime = (uint64)(PyFloat_AsDouble(l_pStartTime) * 1024.)<<22;
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject *l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64 l_ui64EndTime = (uint64)(PyFloat_AsDouble(l_pEndTime) * 1024.)<<22;
			Py_CLEAR(l_pEndTime);

			l_rDynamicBoxContext.markOutputAsReadyToSend(output_index, l_ui64StartTime, l_ui64EndTime);
		}

		else if (PyObject_IsInstance(l_pOVChunk, m_pOVStreamedMatrixBuffer) == 1)
		{
			float64* l_pBufferBase = l_pMatrix->getBuffer();
			for (uint32 i=0; i < l_pMatrix->getBufferElementCount(); i++)
			{
				l_pBufferBase[i] = PyFloat_AsDouble(PyList_GetItem(l_pOVChunk, i));
			}

			//New reference
			PyObject *l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64 l_ui64StartTime = (uint64)(PyFloat_AsDouble(l_pStartTime) * 1024.)<<22;
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject *l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64 l_ui64EndTime = (uint64)(PyFloat_AsDouble(l_pEndTime) * 1024.)<<22;
			Py_CLEAR(l_pEndTime);

			m_vEncoders[output_index]->encodeBuffer(output_index);
			l_rDynamicBoxContext.markOutputAsReadyToSend(output_index, l_ui64StartTime, l_ui64EndTime);
		}

		else if (PyObject_IsInstance(l_pOVChunk, m_pOVStreamedMatrixEnd) == 1)
		{
			//New reference
			PyObject *l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64 l_ui64StartTime = (uint64)(PyFloat_AsDouble(l_pStartTime) * 1024.)<<22;
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject *l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64 l_ui64EndTime = (uint64)(PyFloat_AsDouble(l_pEndTime) * 1024.)<<22;
			Py_CLEAR(l_pEndTime);

			m_vEncoders[output_index]->encodeEnd(output_index);
			l_rDynamicBoxContext.markOutputAsReadyToSend(output_index, l_ui64StartTime, l_ui64EndTime);
		}

		else
		{
			this->getLogManager() << LogLevel_Error << "Unexpected object type for item " << chunk_index << " in box.output["<< output_index << "].\n";
			Py_CLEAR(l_pOVChunk);
			return false;
		}

		Py_CLEAR(l_pOVChunk);
	}
	return true;
}

OpenViBE::boolean CBoxAlgorithmPython::transferSignalInputChunksToPython(uint32 input_index)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	if (!PyList_Check(m_pBoxInput))
	{
		this->getLogManager() << LogLevel_Error << "box.input must be a list.\n";
		return false;
	}

	//Borrowed reference
	PyObject *l_pBuffer = PyList_GetItem(m_pBoxInput, (Py_ssize_t) input_index);
	if (l_pBuffer == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.input[" << input_index << "].\n";
		return false;
	}
	//Expose input signal chunks to python
	for(uint32 chunk_index=0; chunk_index < l_rDynamicBoxContext.getInputChunkCount(input_index); chunk_index++)
	{
		m_vDecoders[input_index]->decode(input_index, chunk_index);

		if (m_vDecoders[input_index]->isHeaderReceived())
		{
			uint32 l_ui32DimensionCount, l_ui32DimensionSize;
			IMatrix* l_pMatrix = ( (TSignalDecoder <CBoxAlgorithmPython> *) m_vDecoders[input_index] )->getOutputMatrix();
			l_ui32DimensionCount = l_pMatrix->getDimensionCount();

			//New reference
			PyObject *l_pDimensionSize = PyList_New(l_ui32DimensionCount);
			if (l_pDimensionSize == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new list l_pDimensionSize.\n";
				Py_CLEAR(l_pDimensionSize);
				return false;
			}

			//New reference
			PyObject *l_pDimensionLabel = PyList_New(0);
			if (l_pDimensionLabel == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new list l_pDimensionLabel.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				return false;
			}

			for (uint32 i=0; i<l_ui32DimensionCount; i++)
			{
				l_ui32DimensionSize = l_pMatrix->getDimensionSize(i);
				if (PyList_SetItem(l_pDimensionSize, i, PyInt_FromLong(l_ui32DimensionSize)) == -1)
				{
					this->getLogManager() << LogLevel_Error << "Failed to set item " << i << " in dimension size list.\n";
					Py_CLEAR(l_pDimensionSize);
					Py_CLEAR(l_pDimensionLabel);
					return false;
				}
				for (uint32 j=0; j<l_ui32DimensionSize; j++)
				{
					if (PyList_Append(l_pDimensionLabel, PyString_FromString(l_pMatrix->getDimensionLabel(i, j))) == -1)
					{
						this->getLogManager() << LogLevel_Error << "Failed to append \"" << l_pMatrix->getDimensionLabel(i, j) << "\" in dimension label list.\n";
						Py_CLEAR(l_pDimensionSize);
						Py_CLEAR(l_pDimensionLabel);
						return false;
					}
				}
			}

			//New reference
			PyObject *l_pArg = PyTuple_New(5);
			if (l_pArg == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 0, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkStartTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 1, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkEndTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 2, l_pDimensionSize) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 2 (dimension size) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 3, l_pDimensionLabel) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 3 (dimension label) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 4, PyInt_FromLong((long) ( (TSignalDecoder <CBoxAlgorithmPython> *) m_vDecoders[input_index] )->getOutputSamplingRate())) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 4 (samplingRate) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			//New reference
			PyObject *l_pOVSignalHeader = PyObject_Call(m_pOVSignalHeader, l_pArg, NULL);
			if (l_pOVSignalHeader == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new OVSignalHeader l_pOVSignalHeader.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				Py_CLEAR(l_pOVSignalHeader);
				return false;
			}
			Py_CLEAR(l_pDimensionSize);
			Py_CLEAR(l_pDimensionLabel);
			Py_CLEAR(l_pArg);

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject *l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVSignalHeader, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append chunk to box.input[" << input_index << "].\n";
				Py_CLEAR(l_pResult);
				Py_CLEAR(l_pOVSignalHeader);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVSignalHeader);
		}

		if (m_vDecoders[input_index]->isBufferReceived())
		{
			//New reference
			PyObject *l_pArg = PyTuple_New(3);
			if (l_pArg == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 0, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkStartTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (startTime) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 1, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkEndTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (endTime) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 2, PyList_New(0)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 2 (bufferElements) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}

			//New reference
			PyObject *l_pOVSignalBuffer = PyObject_Call(m_pOVSignalBuffer, l_pArg, NULL);
			if (l_pOVSignalBuffer == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new OVSignalBuffer l_pOVSignalBuffer.\n";
				Py_CLEAR(l_pArg);
				Py_CLEAR(l_pOVSignalBuffer);
				return false;
			}
			Py_CLEAR(l_pArg);

			IMatrix* l_pMatrix = ( (TSignalDecoder <CBoxAlgorithmPython> *) m_vDecoders[input_index] )->getOutputMatrix();
			float64* l_pBufferBase = l_pMatrix->getBuffer();
			for (uint32 element_index = 0; element_index < l_pMatrix->getBufferElementCount(); element_index++)
			{
				if (PyList_Append(l_pOVSignalBuffer, PyFloat_FromDouble(l_pBufferBase[element_index])) == -1)
				{
					this->getLogManager() << LogLevel_Error << "Failed to append element " << element_index << " to l_pOVSignalBuffer.\n";
					Py_CLEAR(l_pOVSignalBuffer);
					return false;
				}
			}

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject *l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVSignalBuffer, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append an OVSignalBuffer to box.input[" << input_index << "].\n";
				Py_CLEAR(l_pResult);
				Py_CLEAR(l_pOVSignalBuffer);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVSignalBuffer);
		}

		if (m_vDecoders[input_index]->isEndReceived())
		{
			//New reference
			PyObject *l_pArg = PyTuple_New(2);
			if (l_pArg == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 0, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkStartTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 1, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkEndTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}

			//New reference
			PyObject *l_pOVSignalEnd = PyObject_Call(m_pOVSignalEnd, l_pArg, NULL);
			if (l_pOVSignalEnd == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new m_pOVSignalEnd l_pOVSignalEnd.\n";
				Py_CLEAR(l_pArg);
				Py_CLEAR(l_pOVSignalEnd);
				return false;
			}
			Py_CLEAR(l_pArg);

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject *l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVSignalEnd, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append an OVSignalEnd to box.input[" << input_index << "].\n";
				Py_CLEAR(l_pResult);
				Py_CLEAR(l_pOVSignalEnd);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVSignalEnd);
		}
	}

	return true;
}

OpenViBE::boolean CBoxAlgorithmPython::transferSignalOutputChunksFromPython(uint32 output_index)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	IMatrix* l_pMatrix = ( (TSignalEncoder <CBoxAlgorithmPython> *) m_vEncoders[output_index] )->getInputMatrix();

	if (!PyList_Check(m_pBoxOutput))
	{
		this->getLogManager() << LogLevel_Error << "box.output must be a list.\n";
		return false;
	}

	//Borrowed reference
	PyObject *l_pBuffer = PyList_GetItem(m_pBoxOutput, (Py_ssize_t) output_index);
	if (l_pBuffer == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.output[" << output_index << "].\n";
		return false;
	}

	//New reference
	PyObject* l_pBufferLen = PyObject_CallMethod(l_pBuffer, (char *) "__len__", NULL);
	if (l_pBufferLen == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.output["<< output_index << "].__len__().\n";
		Py_CLEAR(l_pBufferLen);
		return false;
	}

	uint32 l_ui32OutputLen = PyInt_AsUnsignedLongMask(l_pBufferLen);
	Py_CLEAR(l_pBufferLen);
	for (uint32 chunk_index=0; chunk_index < l_ui32OutputLen; chunk_index++)
	{
		//New reference
		PyObject* l_pOVChunk = PyObject_CallMethod(l_pBuffer, (char *) "pop", NULL);
		if (l_pOVChunk == NULL)
		{
			this->getLogManager() << LogLevel_Error << "Failed to get item " << chunk_index << " of box.output["<< output_index << "].\n";
			Py_CLEAR(l_pOVChunk);
			return false;
		}

		if (PyObject_IsInstance(l_pOVChunk, m_pOVSignalHeader) == 1)
		{
			uint32 l_ui32DimensionCount, l_ui32DimensionSize, l_ui32DimensionIndex, l_ui32DimensionEntryIndex;

			//New reference
			PyObject *l_pDimensionCount = PyObject_CallMethod(l_pOVChunk, (char *) "getDimensionCount", NULL);
			if (l_pDimensionCount == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to call chunk's getDimensionCount method.\n";
				Py_CLEAR(l_pDimensionCount);
				Py_CLEAR(l_pOVChunk);
				return false;
			}
			l_ui32DimensionCount = PyInt_AsUnsignedLongMask(l_pDimensionCount);
			l_pMatrix->setDimensionCount(l_ui32DimensionCount);
			Py_CLEAR(l_pDimensionCount);

			//New reference
			PyObject *l_pDimensionSize = PyObject_GetAttrString(l_pOVChunk, "dimensionSizes");

			//New reference
			PyObject *l_pDimensionLabel = PyObject_GetAttrString(l_pOVChunk, "dimensionLabels");

			uint32 offset = 0;
			for (l_ui32DimensionIndex=0; l_ui32DimensionIndex < l_ui32DimensionCount; l_ui32DimensionIndex++)
			{
				l_ui32DimensionSize = PyInt_AsUnsignedLongMask(PyList_GetItem(l_pDimensionSize, (Py_ssize_t) l_ui32DimensionIndex));
				l_pMatrix->setDimensionSize(l_ui32DimensionIndex, l_ui32DimensionSize);
				for (l_ui32DimensionEntryIndex=0; l_ui32DimensionEntryIndex < l_ui32DimensionSize; l_ui32DimensionEntryIndex++)
				{
					l_pMatrix->setDimensionLabel(l_ui32DimensionIndex, l_ui32DimensionEntryIndex, PyString_AsString(PyList_GetItem(l_pDimensionLabel, offset + l_ui32DimensionEntryIndex)));
				}
				offset = offset + l_ui32DimensionSize;
			}
			Py_CLEAR(l_pDimensionSize);
			Py_CLEAR(l_pDimensionLabel);

			//New reference
			PyObject *l_pChunkSamplingRate = PyObject_GetAttrString(l_pOVChunk, "samplingRate");
			if (l_pChunkSamplingRate == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to load signal header sampling rate.\n";
				Py_CLEAR(l_pChunkSamplingRate);
				return false;
			}
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& l_pSamplingRate = ( (TSignalEncoder <CBoxAlgorithmPython> *) m_vEncoders[output_index] )->getInputSamplingRate();
			l_pSamplingRate = (OpenViBE::uint64) PyInt_AsLong(l_pChunkSamplingRate);
			m_vEncoders[output_index]->encodeHeader(output_index);
			Py_CLEAR(l_pChunkSamplingRate);

			//New reference
			PyObject *l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64 l_ui64StartTime = (uint64)(PyFloat_AsDouble(l_pStartTime) * 1024.)<<22;
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject *l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64 l_ui64EndTime = (uint64)(PyFloat_AsDouble(l_pEndTime) * 1024.)<<22;
			Py_CLEAR(l_pEndTime);

			l_rDynamicBoxContext.markOutputAsReadyToSend(output_index, l_ui64StartTime, l_ui64EndTime);
		}

		else if (PyObject_IsInstance(l_pOVChunk, m_pOVSignalBuffer) == 1)
		{
			float64* l_pBufferBase = l_pMatrix->getBuffer();
			for (uint32 i=0; i < l_pMatrix->getBufferElementCount(); i++)
			{
				l_pBufferBase[i] = PyFloat_AsDouble(PyList_GetItem(l_pOVChunk, i));
			}

			//New reference
			PyObject *l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64 l_ui64StartTime = (uint64)(PyFloat_AsDouble(l_pStartTime) * 1024.)<<22;
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject *l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64 l_ui64EndTime = (uint64)(PyFloat_AsDouble(l_pEndTime) * 1024.)<<22;
			Py_CLEAR(l_pEndTime);

			m_vEncoders[output_index]->encodeBuffer(output_index);
			l_rDynamicBoxContext.markOutputAsReadyToSend(output_index, l_ui64StartTime, l_ui64EndTime);
		}

		else if (PyObject_IsInstance(l_pOVChunk, m_pOVSignalEnd) == 1)
		{
			//New reference
			PyObject *l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64 l_ui64StartTime = (uint64)(PyFloat_AsDouble(l_pStartTime) * 1024.)<<22;
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject *l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64 l_ui64EndTime = (uint64)(PyFloat_AsDouble(l_pEndTime) * 1024.)<<22;
			Py_CLEAR(l_pEndTime);

			m_vEncoders[output_index]->encodeEnd(output_index);
			l_rDynamicBoxContext.markOutputAsReadyToSend(output_index, l_ui64StartTime, l_ui64EndTime);
		}

		else
		{
			this->getLogManager() << LogLevel_Error << "Unexpected object type for item " << chunk_index << " in box.output["<< output_index << "].\n";
			Py_CLEAR(l_pOVChunk);
			return false;
		}

		Py_CLEAR(l_pOVChunk);
	}
	return true;
}

OpenViBE::boolean CBoxAlgorithmPython::transferStimulationInputChunksToPython(uint32 input_index)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	//Borrowed reference
	PyObject *l_pBuffer = PyList_GetItem(m_pBoxInput, (Py_ssize_t) input_index);
	if (l_pBuffer == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.input[" << input_index << "].\n";
		return false;
	}
	for(uint32 chunk_index = 0; chunk_index < l_rDynamicBoxContext.getInputChunkCount(input_index); chunk_index++)
	{
		m_vDecoders[input_index]->decode(input_index, chunk_index);

		if (m_vDecoders[input_index]->isHeaderReceived())
		{
			//New reference
			PyObject *l_pArg = PyTuple_New(2);
			if (l_pArg == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 0, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkStartTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 1, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkEndTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			PyObject *l_pOVStimulationHeader = PyObject_Call(m_pOVStimulationHeader, l_pArg, NULL);
			if (l_pOVStimulationHeader == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new OVStimulationHeader l_pOVStimulationHeader.\n";
				PyErr_Print();
				Py_CLEAR(l_pArg);
				Py_CLEAR(l_pOVStimulationHeader);
				return false;
			}
			Py_CLEAR(l_pArg);

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject* l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVStimulationHeader, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append an OVStimulationHeader to box.input[" << input_index << "].\n";
				Py_CLEAR(l_pResult);
				Py_CLEAR(l_pOVStimulationHeader);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVStimulationHeader);
		}

		if (m_vDecoders[input_index]->isBufferReceived())
		{
			IStimulationSet* l_pStimulationSet = ( (TStimulationDecoder <CBoxAlgorithmPython> *) m_vDecoders[input_index] )->getOutputStimulationSet();

			//New reference
			PyObject *l_pArg = PyTuple_New(2);
			if (l_pArg == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 0, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkStartTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 1, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkEndTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			//New reference
			PyObject *l_pOVStimulationSet = PyObject_Call(m_pOVStimulationSet, l_pArg, NULL);
			if (l_pOVStimulationSet == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new OVStimulationSet l_pOVStimulationSet.\n";
				Py_CLEAR(l_pArg);
				Py_CLEAR(l_pOVStimulationSet);
				return false;
			}
			Py_CLEAR(l_pArg);

			for(uint32 stimulation_index = 0; stimulation_index < l_pStimulationSet->getStimulationCount(); stimulation_index++)
			{
				uint64 l_ui64StimulationIdentifier = l_pStimulationSet->getStimulationIdentifier(stimulation_index);
				uint64 l_ui64StimulationDate = l_pStimulationSet->getStimulationDate(stimulation_index);
				uint64 l_ui64StimulationDuration = l_pStimulationSet->getStimulationDuration(stimulation_index);

				//New reference
				PyObject *l_pArg = PyTuple_New(3);
				if (l_pArg == NULL)
				{
					this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
					Py_CLEAR(l_pArg);
					return false;
				}
				if (PyTuple_SetItem(l_pArg, 0, PyInt_FromLong((long) l_ui64StimulationIdentifier)) != 0)
				{
					this->getLogManager() << LogLevel_Error << "Failed to set item 0 (identifier) in tuple l_pArg.\n";
					Py_CLEAR(l_pArg);
					return false;
				}
				if (PyTuple_SetItem(l_pArg, 1, PyFloat_FromDouble((double)(l_ui64StimulationDate>>22) / 1024.)) != 0)
				{
					this->getLogManager() << LogLevel_Error << "Failed to set item 1 (date) in tuple l_pArg.\n";
					Py_CLEAR(l_pArg);
					return false;
				}
				if (PyTuple_SetItem(l_pArg, 2, PyFloat_FromDouble((double)(l_ui64StimulationDuration>>22) / 1024.)) != 0)
				{
					this->getLogManager() << LogLevel_Error << "Failed to set item 2 (duration) in tuple l_pArg.\n";
					Py_CLEAR(l_pArg);
					return false;
				}
				//New reference
				PyObject *l_pOVStimulation = PyObject_Call(m_pOVStimulation, l_pArg, NULL);
				if (l_pOVStimulation == NULL)
				{
					this->getLogManager() << LogLevel_Error << "Failed to create a new OVStimulation l_pOVStimulation.\n";
					Py_CLEAR(l_pArg);
					Py_CLEAR(l_pOVStimulation);
					return false;
				}
				Py_CLEAR(l_pArg);

				//New reference
				PyObject *l_pMethodToCall = PyString_FromString("append");
				//New reference
				PyObject *l_pResult = PyObject_CallMethodObjArgs(l_pOVStimulationSet, l_pMethodToCall, l_pOVStimulation, NULL);
				Py_CLEAR(l_pMethodToCall);
				if (l_pResult == NULL)
				{
					this->getLogManager() << LogLevel_Error << "Failed to append stimulation to box.input[" << input_index << "].\n";
					Py_CLEAR(l_pResult);
					Py_CLEAR(l_pOVStimulation);
					return false;
				}
				Py_CLEAR(l_pResult);
				Py_CLEAR(l_pOVStimulation);
			}

			//New reference
			PyObject *l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject *l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVStimulationSet, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append stimulation set to box.input[" << input_index << "].\n";
				Py_CLEAR(l_pResult);
				Py_CLEAR(l_pOVStimulationSet);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVStimulationSet);
		}

		if (m_vDecoders[input_index]->isEndReceived())
		{
			//New reference
			PyObject *l_pArg = PyTuple_New(2);
			if (l_pArg == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 0, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkStartTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 1, PyFloat_FromDouble((double)(l_rDynamicBoxContext.getInputChunkEndTime(input_index, chunk_index)>>22) / 1024.)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}

			//New reference
			PyObject *l_pOVStimulationEnd = PyObject_Call(m_pOVStimulationEnd, l_pArg, NULL);
			if (l_pOVStimulationEnd == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new OVStimulationEnd l_pOVStimulationEnd.\n";
				Py_CLEAR(l_pArg);
				Py_CLEAR(l_pOVStimulationEnd);
				return false;
			}
			Py_CLEAR(l_pArg);

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject *l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVStimulationEnd, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append an OVStimulationEnd to box.input[" << input_index << "].\n";
				Py_CLEAR(l_pResult);
				Py_CLEAR(l_pOVStimulationEnd);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVStimulationEnd);
		}
	}
	return true;
}

OpenViBE::boolean CBoxAlgorithmPython::transferStimulationOutputChunksFromPython(uint32 output_index)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	IStimulationSet* l_pStimulationSet = ( (TStimulationEncoder <CBoxAlgorithmPython> *) m_vEncoders[output_index] )->getInputStimulationSet();

	if (!PyList_Check(m_pBoxOutput))
	{
		this->getLogManager() << LogLevel_Error << "box.output must be a list.\n";
		return false;
	}
	//Borrowed reference
	PyObject *l_pBuffer = PyList_GetItem(m_pBoxOutput, (Py_ssize_t) output_index);
	if (l_pBuffer == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.output[" << output_index << "].\n";
		return false;
	}

	//New reference
	PyObject* l_pBufferLen = PyObject_CallMethod(l_pBuffer, (char *) "__len__", NULL);
	if (l_pBufferLen == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.output["<< output_index << "].__len__().\n";
		Py_CLEAR(l_pBufferLen);
		return false;
	}
	uint32 l_ui32OutputLen = PyInt_AsUnsignedLongMask(l_pBufferLen);
	Py_CLEAR(l_pBufferLen);
	for (uint32 chunk_index=0; chunk_index < l_ui32OutputLen; chunk_index++)
	{
		//New reference
		PyObject* l_pOVChunk = PyObject_CallMethod(l_pBuffer, (char *) "pop", NULL);
		if (l_pOVChunk == NULL)
		{
			this->getLogManager() << LogLevel_Error << "Failed to get item " << chunk_index << " of box.output["<< output_index << "].\n";
			Py_CLEAR(l_pOVChunk);
			return false;
		}

		if (PyObject_IsInstance(l_pOVChunk, m_pOVStimulationHeader) == 1)
		{
			//New reference
			PyObject *l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64 l_ui64StartTime = (uint64)(PyFloat_AsDouble(l_pStartTime) * 1024.)<<22;
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject *l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64 l_ui64EndTime = (uint64)(PyFloat_AsDouble(l_pEndTime) * 1024.)<<22;
			Py_CLEAR(l_pEndTime);

			l_pStimulationSet->setStimulationCount(0);
			m_vEncoders[output_index]->encodeHeader(output_index);
			l_rDynamicBoxContext.markOutputAsReadyToSend(output_index, l_ui64StartTime, l_ui64EndTime);
		}

		else if (PyObject_IsInstance(l_pOVChunk, m_pOVStimulationSet) == 1)
		{
			//New reference
			PyObject* l_pOVChunkLen = PyObject_CallMethod(l_pOVChunk, (char *) "__len__", NULL);
			if (l_pOVChunkLen == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to get stimulations set length.\n";
				Py_CLEAR(l_pOVChunkLen);
				return false;
			}
			uint32 l_ui32OVChunkLen = PyInt_AsUnsignedLongMask(l_pOVChunkLen);
			Py_CLEAR(l_pOVChunkLen);

			l_pStimulationSet->setStimulationCount(0);
			for (uint32 stim_index=0; stim_index < l_ui32OVChunkLen; stim_index++)
			{
				//New reference
				PyObject* l_pOVStimulation = PyList_GetItem(l_pOVChunk, (Py_ssize_t) stim_index);
				if (l_pOVStimulation == NULL)
				{
					this->getLogManager() << LogLevel_Error << "Failed to get item " << stim_index << " of chunk "<< chunk_index << ".\n";
					Py_CLEAR(l_pOVStimulation);
					return false;
				}
				if (PyObject_IsInstance(l_pOVStimulation, m_pOVStimulation) < 1)
				{
					this->getLogManager() << LogLevel_Error << "Item " << stim_index << " is not an OVStimulation.\n";
					Py_CLEAR(l_pOVStimulation);
					return false;
				}
				//New reference
				PyObject *l_pIdentifier = PyObject_GetAttrString(l_pOVStimulation, "identifier");
				uint64 l_ui64Identifier = (uint64) PyFloat_AsDouble(l_pIdentifier);
				Py_CLEAR(l_pIdentifier);

				//New reference
				PyObject *l_pDate = PyObject_GetAttrString(l_pOVStimulation, "date");
				uint64 l_ui64Date = (uint64)(PyFloat_AsDouble(l_pDate) * 1024.)<<22;
				Py_CLEAR(l_pDate);

				//New reference
				PyObject *l_pDuration = PyObject_GetAttrString(l_pOVStimulation, "duration");
				uint64 l_ui64Duration = (uint64)(PyFloat_AsDouble(l_pDuration) * 1024.)<<22;
				Py_CLEAR(l_pDuration);

				Py_CLEAR(l_pOVStimulation);

				l_pStimulationSet->appendStimulation(l_ui64Identifier, l_ui64Date, l_ui64Duration);
			}

			//New reference
			PyObject *l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64 l_ui64StartTime = (uint64)(PyFloat_AsDouble(l_pStartTime) * 1024.)<<22;
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject *l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64 l_ui64EndTime = (uint64)(PyFloat_AsDouble(l_pEndTime) * 1024.)<<22;
			Py_CLEAR(l_pEndTime);

			m_vEncoders[output_index]->encodeBuffer(output_index);
			l_rDynamicBoxContext.markOutputAsReadyToSend(output_index, l_ui64StartTime, l_ui64EndTime);
		}

		else if (PyObject_IsInstance(l_pOVChunk, m_pOVStimulationEnd) == 1)
		{
			//New reference
			PyObject *l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64 l_ui64StartTime = (uint64)(PyFloat_AsDouble(l_pStartTime) * 1024.)<<22;
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject *l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64 l_ui64EndTime = (uint64)(PyFloat_AsDouble(l_pEndTime) * 1024.)<<22;
			Py_CLEAR(l_pEndTime);

			m_vEncoders[output_index]->encodeEnd(output_index);
			l_rDynamicBoxContext.markOutputAsReadyToSend(output_index, l_ui64StartTime, l_ui64EndTime);
		}

		else
		{
			this->getLogManager() << LogLevel_Error << "Unexpected object type for item " << chunk_index << " in box.output["<< output_index << "].\n";
			Py_CLEAR(l_pOVChunk);
			return false;
		}

		Py_CLEAR(l_pOVChunk);
	}

	return true;
}

OpenViBE::boolean CBoxAlgorithmPython::process(void)
{	
	IBox& l_rStaticBoxContext = this->getStaticBoxContext();
	OpenViBE::CIdentifier l_oTypeIdentifier;

	for(uint32 input=0; input<l_rStaticBoxContext.getInputCount(); input++)
	{
		l_rStaticBoxContext.getInputType(input, l_oTypeIdentifier);
		if (l_oTypeIdentifier == OV_TypeId_StreamedMatrix)
		{
			if (!transferStreamedMatrixInputChunksToPython(input))
			{
				return false;
			}
		}
		else if (l_oTypeIdentifier == OV_TypeId_Signal)
		{
			if (!transferSignalInputChunksToPython(input))
			{
				return false;
			}
		}
		/*
		else if (l_oTypeIdentifier == OV_TypeId_FeatureVector)
		{

		}
		else if (l_oTypeIdentifier == OV_TypeId_Spectrum)
		{

		}
		else if (l_oTypeIdentifier == OV_TypeId_ChannelLocalisation)
		{

		}
		*/
		else if (l_oTypeIdentifier == OV_TypeId_Stimulations)
		{
			if (!transferStimulationInputChunksToPython(input))
			{
				return false;
			}
		}
		/*
		else if (l_oTypeIdentifier == OV_TypeId_ExperimentationInformation)
		{

		}
		*/
		else
		{
			this->getLogManager() << LogLevel_Error << "Codec to decode " << l_oTypeIdentifier.toString() << " is not implemented.\n";
			return false;
		}
	}

	//update the python current time
	m_pBoxCurrentTime = PyFloat_FromDouble((double)(this->getPlayerContext().getCurrentTime()>>22) / 1024.);
	if (m_pBoxCurrentTime == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to convert the current time into a PyFloat during update.\n";
		return false;
	}
	if (PyObject_SetAttrString(m_pBox, "_currentTime", m_pBoxCurrentTime) == -1)
	{
		this->getLogManager() << LogLevel_Error << "Failed to update \"box._currentTime\" attribute.\n";
		return false;
	}

	//call the python process function
	if (m_pBoxProcess && PyCallable_Check(m_pBoxProcess))
	{
		//New reference
		PyObject *l_pResult = PyObject_CallObject(m_pBoxProcess, NULL);
		OpenViBE::boolean l_bLogSysStdoutError = logSysStdout();
		OpenViBE::boolean l_bLogSysStderrError = logSysStderr();
		if ((l_pResult == NULL) || (!l_bLogSysStdoutError) || (!l_bLogSysStderrError))
		{
			if(l_pResult == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Failed to call \"box.__process\" function.\n";
			}
			if(!l_bLogSysStdoutError)
			{
				this->getLogManager() << LogLevel_Error << "logSysStdout() failed during box.__process.\n";
			}
			if(!l_bLogSysStderrError)
			{
				this->getLogManager() << LogLevel_Error << "logSysStderr() failed during box.__process.\n";
			}
			Py_CLEAR(l_pResult);
			return false;
		}
		Py_CLEAR(l_pResult);
	}

	for(uint32 output=0; output<l_rStaticBoxContext.getOutputCount(); output++)
	{
		l_rStaticBoxContext.getOutputType(output, l_oTypeIdentifier);
		if (l_oTypeIdentifier == OV_TypeId_StreamedMatrix)
		{
			if (!transferStreamedMatrixOutputChunksFromPython(output))
			{
				return false;
			}
		}
		else if (l_oTypeIdentifier == OV_TypeId_Signal)
		{
			if (!transferSignalOutputChunksFromPython(output))
			{
				return false;
			}
		}
		/*
		else if (l_oTypeIdentifier == OV_TypeId_FeatureVector)
		{

		}
		else if (l_oTypeIdentifier == OV_TypeId_Spectrum)
		{

		}
		else if (l_oTypeIdentifier == OV_TypeId_ChannelLocalisation)
		{

		}
		*/
		else if (l_oTypeIdentifier == OV_TypeId_Stimulations)
		{
			if (!transferStimulationOutputChunksFromPython(output))
			{
				return false;
			}
		}
		/*
		else if (l_oTypeIdentifier == OV_TypeId_ExperimentationInformation)
		{

		}
		*/
		else
		{
			this->getLogManager() << LogLevel_Error << "Codec to encode " << l_oTypeIdentifier.toString() << " is not implemented.\n";
			return false;
		}
	}

	return true;
}

#endif // TARGET_HAS_ThirdPartyPython
