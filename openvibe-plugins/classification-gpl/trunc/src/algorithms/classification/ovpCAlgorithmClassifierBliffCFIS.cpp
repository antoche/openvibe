#include "ovpCAlgorithmClassifierBliffCFIS.h"

#if defined TARGET_HAS_ThirdPartyBLiFF

#include <FeatureVector.h>
#include <FeatVecSet.h>
#include <CFIS.h>

#include <fstream>
#include <map>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Local;

using namespace OpenViBEToolkit;

using namespace std;

#define _ParameterFile_ "./CAlgorithmClassifierBliffCFIS-config.tmp"

boolean CAlgorithmClassifierBliffCFIS::initialize(void)
{
	if(!CAlgorithmClassifier::initialize())
	{
		return false;
	}

	ip_f64Radius.initialize(this->getInputParameter(OVP_Algorithm_CAlgorithmClassifierCFIS_InputParameterId_Radius));
	ip_ui64Mode.initialize(this->getInputParameter(OVP_Algorithm_CAlgorithmClassifierCFIS_InputParameterId_Mode));
	ip_ui64OutputMode.initialize(this->getInputParameter(OVP_Algorithm_CAlgorithmClassifierCFIS_InputParameterId_OutputMode));

	ip_f64Radius=0.5;
	ip_ui64Mode=OVP_TypeId_CFISMode_TwoSidedGaussian.toUInteger();
	ip_ui64OutputMode=OVP_TypeId_CFISOutputMode_ClassMembership.toUInteger();

	return true;
}

boolean CAlgorithmClassifierBliffCFIS::uninitialize(void)
{
	ip_ui64OutputMode.uninitialize();
	ip_ui64Mode.uninitialize();
	ip_f64Radius.uninitialize();

	return CAlgorithmClassifier::uninitialize();
}

boolean CAlgorithmClassifierBliffCFIS::train(const IFeatureVectorSet& rFeatureVectorSet)
{
	bliff::FeatVecSet l_oFeatureVectorSet;
	for(uint32 i=0; i<rFeatureVectorSet.getFeatureVectorCount(); i++)
	{
		const IFeatureVector& l_rFeatureVector=rFeatureVectorSet.getFeatureVector(i);
		bliff::FeatureVector l_oFeatureVector(l_rFeatureVector.getSize(), l_rFeatureVector.getLabel());
		for(uint32 j=0; j<l_rFeatureVector.getSize(); j++)
		{
			l_oFeatureVector[j]=l_rFeatureVector[j];
		}
		l_oFeatureVectorSet.addFeatVec(l_oFeatureVector);
	}

	bliff::CFIS l_oBliffCFISClassifier;

	//setting the mode
	if(ip_ui64Mode == OVP_TypeId_CFISMode_SimpleGaussian.toUInteger())
	{
		l_oBliffCFISClassifier.setMode(SIMPLE_MODE);
	}
	else if(ip_ui64Mode == OVP_TypeId_CFISMode_TwoSidedGaussian.toUInteger())
	{
		l_oBliffCFISClassifier.setMode(TWO_SIDED_MODE);
	}
	else
	{
		this->getLogManager() << LogLevel_Warning << "Unhandled CFIS mode " << ip_ui64Mode << " (" << this->getTypeManager().getEnumerationEntryNameFromValue(OVP_TypeId_CFISMode, ip_ui64Mode) << "\n";
	}

	//setting the radius
	l_oBliffCFISClassifier.setRadius(ip_f64Radius);

	// l_oBliffCFISClassifier.kFoldTest(l_oFeatureVectorSet, 10, 10);
	// this->getLogManager() << LogLevel_Trace << "BLiFF KFoldTest returned : " << (float64)l_oBliffCFISClassifier.getAccuracy() << "%\n";

	l_oBliffCFISClassifier.train(l_oFeatureVectorSet);

	l_oBliffCFISClassifier.saveParams(_ParameterFile_);

	FILE* l_pFile=::fopen(_ParameterFile_, "rb");
	if(l_pFile)
	{
		size_t l_iFileLen, l_iReadLen;
		::fseek(l_pFile, 0, SEEK_END);
		l_iFileLen=::ftell(l_pFile);
		::fseek(l_pFile, 0, SEEK_SET);
		m_oConfiguration.setSize(l_iFileLen, true);
		l_iReadLen=::fread(m_oConfiguration.getDirectPointer(), 1, l_iFileLen, l_pFile);
		::fclose(l_pFile);
	}
	this->getLogManager() << LogLevel_Error << (char*)m_oConfiguration.getDirectPointer() << "\n";

	l_oBliffCFISClassifier.test(l_oFeatureVectorSet);
	this->getLogManager() << LogLevel_Trace << "BLiFF Test returned : " << (float64)l_oBliffCFISClassifier.getAccuracy() << "%\n";
	this->getLogManager() << LogLevel_Trace << "True positive rate  : " << (float64)l_oBliffCFISClassifier.getTPRate() << "%\n";
	this->getLogManager() << LogLevel_Trace << "False positive rate : " << (float64)l_oBliffCFISClassifier.getFPRate() << "%\n";
	// this->getLogManager() << LogLevel_Trace << "Confusion matrix    : " << (float64)l_oBliffCFISClassifier.getConfusionMatrix() << "\n";

	return true;
}

boolean CAlgorithmClassifierBliffCFIS::classify(const IFeatureVector& rFeatureVector, float64& rf64Class, IVector& rClassificationValues)
{
	bliff::FeatureVector l_oFeatureVector(rFeatureVector.getSize(), 0);
	for(uint32 j=0; j<rFeatureVector.getSize(); j++)
	{
		l_oFeatureVector[j]=rFeatureVector[j];
	}

	FILE* l_pFile=::fopen(_ParameterFile_, "wb");
	::fwrite(m_oConfiguration.getDirectPointer(), m_oConfiguration.getSize(), 1, l_pFile);
	::fclose(l_pFile);

	itpp::Vec<double> l_vResult;
	double l_dResult;
	bliff::CFIS l_oBliffCFISClassifier;

	l_oBliffCFISClassifier.readParams(_ParameterFile_);
	l_vResult=l_oBliffCFISClassifier.classify(l_oFeatureVector);
	l_dResult=l_oBliffCFISClassifier.assign(l_oFeatureVector);

	if(ip_ui64OutputMode == OVP_TypeId_CFISOutputMode_ClassMembership.toUInteger())
	{
		itpp::Vec<double> l_vTmpVec(l_oBliffCFISClassifier.getNbClasses());
		l_vTmpVec.zeros();
		std::vector<double> l_oClassLabels = l_oBliffCFISClassifier.getClassLabels();

		//finding the maximal degree of fulfillment for each class
		for(uint32 j=0; j < l_oBliffCFISClassifier.getNbRules(); j++)
		{
			for(uint32 i=0; i < l_oClassLabels.size(); i++)
			{
				if((l_oBliffCFISClassifier.getRule(j)->getClass() == l_oClassLabels[i]) && (l_vResult[j]>l_vTmpVec[i]))
				{
					l_vTmpVec[i]=l_vResult[j];
				}
			}
		}

		//switching from rule fulfillment to class membership
		l_vResult=l_vTmpVec;
	}

	//converting from BLiFF++ output to OpenViBE output (classification state)
	rf64Class=l_dResult;
	rClassificationValues.setSize(l_vResult.size());
	for(size_t i=0; i<rClassificationValues.getSize(); i++)
	{
		rClassificationValues[i]=l_vResult[i];
	}

	//labelling the element of the output vector
	if(ip_ui64OutputMode == OVP_TypeId_CFISOutputMode_ClassMembership.toUInteger())
	{
		char l_sBuffer[1024];
		std::vector<double> l_vClassLabels = l_oBliffCFISClassifier.getClassLabels();
		for(uint32 i=0; i < rClassificationValues.getSize(); i++)
		{
			sprintf(l_sBuffer, "Class %d membership degree", (uint32)l_vClassLabels[i]);
			rClassificationValues.setElementLabel(i, l_sBuffer);
		}
	}
	else if(ip_ui64OutputMode == OVP_TypeId_CFISOutputMode_RuleFulfillment.toUInteger())
	{
		char l_sBuffer[1024];
		for(uint32 i=0; i < rClassificationValues.getSize(); i++)
		{
			sprintf(l_sBuffer, "Rule %d degree of fulfillment", (uint32)i+1);
			rClassificationValues.setElementLabel(i, l_sBuffer);
		}
	}
	else
	{
		this->getLogManager() << LogLevel_Warning << "Unhandled CFIS output mode " << ip_ui64OutputMode << " (" << this->getTypeManager().getEnumerationEntryNameFromValue(OVP_TypeId_CFISOutputMode, ip_ui64OutputMode) << "\n";
	}

	return true;
}

boolean CAlgorithmClassifierBliffCFIS::saveConfiguration(IMemoryBuffer& rMemoryBuffer)
{
	rMemoryBuffer.setSize(0, true);
	rMemoryBuffer.append(m_oConfiguration);
	return true;
}

boolean CAlgorithmClassifierBliffCFIS::loadConfiguration(const IMemoryBuffer& rMemoryBuffer)
{
	m_oConfiguration.setSize(0, true);
	m_oConfiguration.append(rMemoryBuffer);
	return true;
}

#endif // TARGET_HAS_ThirdPartyBLiFF
