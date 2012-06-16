#include "ovpCAlgorithmClassifierBliffLDA.h"

#if defined TARGET_HAS_ThirdPartyBLiFF

#include <FeatureVector.h>
#include <FeatVecSet.h>
#include <LDAClassif.h>

#include <fstream>
#include <map>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Local;

using namespace OpenViBEToolkit;

using namespace std;

#define _ParameterFile_ "./CAlgorithmClassifierBliffLDA-config.tmp"

boolean CAlgorithmClassifierBliffLDA::train(const IFeatureVectorSet& rFeatureVectorSet)
{
	float64 l_f64BliffLDASign=1;
	float64 l_f64BliffLDALabel1=rFeatureVectorSet[0].getLabel();
	float64 l_f64BliffLDALabel2=l_f64BliffLDALabel1;
	for(uint32 i=0; i<rFeatureVectorSet.getFeatureVectorCount(); i++)
	{
		const IFeatureVector& l_rFeatureVector=rFeatureVectorSet.getFeatureVector(i);
		if(l_rFeatureVector.getLabel()!=l_f64BliffLDALabel1)
		{
			if(l_f64BliffLDALabel2==l_f64BliffLDALabel1)
			{
				l_f64BliffLDALabel2=l_rFeatureVector.getLabel();
			}
			else if(l_f64BliffLDALabel2!=l_rFeatureVector.getLabel())
			{
				this->getLogManager() << LogLevel_ImportantWarning << "This algorithm can not handle more than 2 classes\n";
				this->getLogManager() << LogLevel_ImportantWarning << "Found at least 3 classes : " << l_f64BliffLDALabel1 << " " << l_f64BliffLDALabel2 << " " << l_rFeatureVector.getLabel() << "\n";
				return false;
			}
		}
	}

	if(l_f64BliffLDALabel1==l_f64BliffLDALabel2)
	{
		this->getLogManager() << LogLevel_ImportantWarning << "This algorithm can not handle less than 2 classes\n";
		return false;
	}

	map<float64, float64> l_vLabel;
	if(l_f64BliffLDALabel2<l_f64BliffLDALabel1)
	{
		l_vLabel[l_f64BliffLDALabel1]=l_f64BliffLDALabel2;
		l_vLabel[l_f64BliffLDALabel2]=l_f64BliffLDALabel1;
		l_f64BliffLDASign=-1;
		this->getLogManager() << LogLevel_Info << "The sign of the BliffLDA coefficients will be changed for better coherency (first class in the negative side of the hyperplane)\n";
	}
	else
	{
		l_vLabel[l_f64BliffLDALabel1]=l_f64BliffLDALabel1;
		l_vLabel[l_f64BliffLDALabel2]=l_f64BliffLDALabel2;
		l_f64BliffLDASign=1;
	}

	bliff::FeatVecSet l_oFeatureVectorSet;
	for(uint32 i=0; i<rFeatureVectorSet.getFeatureVectorCount(); i++)
	{
		const IFeatureVector& l_rFeatureVector=rFeatureVectorSet.getFeatureVector(i);
		bliff::FeatureVector l_oFeatureVector(l_rFeatureVector.getSize(), l_vLabel[l_rFeatureVector.getLabel()]);
		for(uint32 j=0; j<l_rFeatureVector.getSize(); j++)
		{
			l_oFeatureVector[j]=l_rFeatureVector[j];
		}
		l_oFeatureVectorSet.addFeatVec(l_oFeatureVector);
	}

	bliff::CLDAClassifier l_oBliffLDAClassifier;

	// l_oBliffLDAClassifier.kFoldTest(l_oFeatureVectorSet, 10, 10);
	// this->getLogManager() << LogLevel_Trace << "BLiFF KFoldTest returned : " << (float64)l_oBliffLDAClassifier.getAccuracy() << "%\n";

	l_oBliffLDAClassifier.train(l_oFeatureVectorSet);
	vector<double> l_vCoefficients=l_oBliffLDAClassifier.getCoefficients();
	for(size_t j=0; j<l_vCoefficients.size(); j++)
	{
		l_oBliffLDAClassifier.setCoefficient(j, l_f64BliffLDASign*l_vCoefficients[j]);
	}
	l_oBliffLDAClassifier.saveParams(_ParameterFile_);

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

	l_oBliffLDAClassifier.test(l_oFeatureVectorSet);
	this->getLogManager() << LogLevel_Trace << "BLiFF Test returned : " << (float64)l_oBliffLDAClassifier.getAccuracy() << "%\n";
	this->getLogManager() << LogLevel_Trace << "True positive rate  : " << (float64)l_oBliffLDAClassifier.getTPRate() << "%\n";
	this->getLogManager() << LogLevel_Trace << "False positive rate : " << (float64)l_oBliffLDAClassifier.getFPRate() << "%\n";
	// this->getLogManager() << LogLevel_Trace << "Confusion matrix    : " << (float64)l_oBliffLDAClassifier.getConfusionMatrix() << "\n";

	return true;
}

boolean CAlgorithmClassifierBliffLDA::classify(const IFeatureVector& rFeatureVector, float64& rf64Class, IVector& rClassificationValues)
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
	bliff::CLDAClassifier l_oBliffLDAClassifier;
	l_oBliffLDAClassifier.readParams(_ParameterFile_);
	l_vResult=l_oBliffLDAClassifier.classify(l_oFeatureVector);
	l_dResult=l_oBliffLDAClassifier.assign(l_oFeatureVector);

	rf64Class=l_dResult;
	rClassificationValues.setSize(l_vResult.size());
	for(size_t i=0; i<rClassificationValues.getSize(); i++)
	{
		rClassificationValues[i]=-l_vResult[i];
	}

	return true;
}

boolean CAlgorithmClassifierBliffLDA::saveConfiguration(IMemoryBuffer& rMemoryBuffer)
{
	rMemoryBuffer.setSize(0, true);
	rMemoryBuffer.append(m_oConfiguration);
	return true;
}

boolean CAlgorithmClassifierBliffLDA::loadConfiguration(const IMemoryBuffer& rMemoryBuffer)
{
	m_oConfiguration.setSize(0, true);
	m_oConfiguration.append(rMemoryBuffer);
	return true;
}

#endif // TARGET_HAS_ThirdPartyBLiFF
