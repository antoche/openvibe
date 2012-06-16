#include "ovpCAlgorithmClassifierNULL.h"

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <cstring>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Classification;

using namespace OpenViBEToolkit;

CAlgorithmClassifierNULL::CAlgorithmClassifierNULL(void)
{
}


boolean CAlgorithmClassifierNULL::initialize(void)
{
	TParameterHandler < boolean > ip_bParameter1(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter1));
	TParameterHandler < float64 > ip_f64Parameter2(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter2));
	TParameterHandler < uint64 > ip_ui64Parameter3(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter3));

	ip_bParameter1 = true;
	ip_f64Parameter2 = 3.141592654;
	ip_ui64Parameter3 = OVTK_StimulationId_Label_00;

	return CAlgorithmClassifier::initialize();
}

boolean CAlgorithmClassifierNULL::train(const IFeatureVectorSet& rFeatureVectorSet)
{
	TParameterHandler < boolean > ip_bParameter1(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter1));
	TParameterHandler < float64 > ip_f64Parameter2(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter2));
	TParameterHandler < uint64 > ip_ui64Parameter3(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter3));

	this->getLogManager() << LogLevel_Warning << "Parameter 1 : " << ip_bParameter1 << "\n";
	this->getLogManager() << LogLevel_Warning << "Parameter 2 : " << ip_f64Parameter2 << "\n";
	this->getLogManager() << LogLevel_Warning << "Parameter 3 : " << ip_ui64Parameter3 << "\n";

	return true;
}

boolean CAlgorithmClassifierNULL::classify(const IFeatureVector& rFeatureVector, float64& rf64Class, IVector& rClassificationValues)
{
	rf64Class=1+(::rand()%3);
	return true;
}

boolean CAlgorithmClassifierNULL::saveConfiguration(IMemoryBuffer& rMemoryBuffer)
{
	return true;
}

boolean CAlgorithmClassifierNULL::loadConfiguration(const IMemoryBuffer& rMemoryBuffer)
{
	return true;
}
