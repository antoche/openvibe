#ifndef __OpenViBEToolkit_Reader_TBoxAlgorithmExperimentInformationInputReaderCallback_INL__
#define __OpenViBEToolkit_Reader_TBoxAlgorithmExperimentInformationInputReaderCallback_INL__

#include "ovtkIBoxAlgorithmExperimentInformationInputReaderCallback.h"
#include "ovtkTBoxAlgorithmEBMLInputReaderCallback.inl"

namespace OpenViBEToolkit {

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
class TBoxAlgorithmExperimentInformationInputReaderCallback : public TBoxAlgorithmEBMLInputReaderCallback <IBaseInterface>
{
private:

	typedef TBoxAlgorithmExperimentInformationInputReaderCallback <IBaseInterface> CThisClass;
	typedef TBoxAlgorithmEBMLInputReaderCallback <IBaseInterface> CThisClassParent;

public:

	TBoxAlgorithmExperimentInformationInputReaderCallback(void);
	virtual ~TBoxAlgorithmExperimentInformationInputReaderCallback(void);
	virtual void setExperimentInformationCallback(IBoxAlgorithmExperimentInformationInputReaderCallback::ICallback* pCallback);

	virtual EBML::boolean isMasterChild(const EBML::CIdentifier& rIdentifier);
	virtual void openChild(const EBML::CIdentifier& rIdentifier);
	virtual void processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
	virtual void closeChild(void);

	_IsDerivedFromClass_Final_(CThisClassParent, OVTK_ClassId_);

protected:

	IBoxAlgorithmExperimentInformationInputReaderCallback::ICallback* m_pCallback;

	std::stack<EBML::CIdentifier> m_vNodes;
	EBML::IReaderHelper* m_pReaderHelper;
};

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
TBoxAlgorithmExperimentInformationInputReaderCallback<IBaseInterface>::TBoxAlgorithmExperimentInformationInputReaderCallback(void)
	:m_pCallback(NULL)
	,m_pReaderHelper(NULL)
{
	m_pReaderHelper=EBML::createReaderHelper();
}

template <class IBaseInterface>
TBoxAlgorithmExperimentInformationInputReaderCallback<IBaseInterface>::~TBoxAlgorithmExperimentInformationInputReaderCallback(void)
{
	m_pReaderHelper->release();
	m_pReaderHelper=NULL;
	m_pCallback=NULL;
}

template <class IBaseInterface>
void TBoxAlgorithmExperimentInformationInputReaderCallback<IBaseInterface>::setExperimentInformationCallback(IBoxAlgorithmExperimentInformationInputReaderCallback::ICallback* pCallback)
{
	m_pCallback=pCallback;
}

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
EBML::boolean TBoxAlgorithmExperimentInformationInputReaderCallback<IBaseInterface>::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	     if(rIdentifier==OVTK_NodeId_Header_ExperimentInformation)                              { return true; }
	else if(rIdentifier==OVTK_NodeId_Header_ExperimentInformation_Experiment)                   { return true; }
	else if(rIdentifier==OVTK_NodeId_Header_ExperimentInformation_Experiment_Identifier)        { return false; }
	else if(rIdentifier==OVTK_NodeId_Header_ExperimentInformation_Experiment_Date)              { return false; }
	else if(rIdentifier==OVTK_NodeId_Header_ExperimentInformation_Subject)                      { return true; }
	else if(rIdentifier==OVTK_NodeId_Header_ExperimentInformation_Subject_Identifier)           { return false; }
	else if(rIdentifier==OVTK_NodeId_Header_ExperimentInformation_Subject_Name)                 { return false; }
	else if(rIdentifier==OVTK_NodeId_Header_ExperimentInformation_Subject_Age)                  { return false; }
	else if(rIdentifier==OVTK_NodeId_Header_ExperimentInformation_Subject_Gender)               { return false; }
	else if(rIdentifier==OVTK_NodeId_Header_ExperimentInformation_Context)                      { return true; }
	else if(rIdentifier==OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryIdentifier) { return false; }
	else if(rIdentifier==OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryName)       { return false; }
	else if(rIdentifier==OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianIdentifier) { return false; }
	else if(rIdentifier==OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianName)       { return false; }
	return CThisClassParent::isMasterChild(rIdentifier);
}

template <class IBaseInterface>
void TBoxAlgorithmExperimentInformationInputReaderCallback<IBaseInterface>::openChild(const EBML::CIdentifier& rIdentifier)
{
	m_vNodes.push(rIdentifier);

	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Header_ExperimentInformation)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Experiment)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Experiment_Identifier)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Experiment_Date)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject_Identifier)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject_Name)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject_Age)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject_Gender)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryIdentifier)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryName)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianIdentifier)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianName))
	{
	}
	else
	{
		CThisClassParent::openChild(rIdentifier);
	}
}

template <class IBaseInterface>
void TBoxAlgorithmExperimentInformationInputReaderCallback<IBaseInterface>::processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Header_ExperimentInformation)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Experiment)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Experiment_Identifier)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Experiment_Date)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject_Identifier)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject_Name)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject_Age)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject_Gender)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryIdentifier)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryName)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianIdentifier)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianName))
	{
		if(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Experiment_Identifier)        m_pCallback->setValue(IBoxAlgorithmExperimentInformationInputReaderCallback::Value_ExperimentIdentifier, m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize));
		if(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Experiment_Date)              m_pCallback->setValue(IBoxAlgorithmExperimentInformationInputReaderCallback::Value_ExperimentDate,       m_pReaderHelper->getASCIIStringFromChildData(pBuffer, ui64BufferSize));

		if(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject_Identifier)           m_pCallback->setValue(IBoxAlgorithmExperimentInformationInputReaderCallback::Value_SubjectIdentifier,    m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize));
		if(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject_Name)                 m_pCallback->setValue(IBoxAlgorithmExperimentInformationInputReaderCallback::Value_SubjectName,          m_pReaderHelper->getASCIIStringFromChildData(pBuffer, ui64BufferSize));
		if(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject_Age)                  m_pCallback->setValue(IBoxAlgorithmExperimentInformationInputReaderCallback::Value_SubjectAge,           m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize));
		if(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject_Gender)               m_pCallback->setValue(IBoxAlgorithmExperimentInformationInputReaderCallback::Value_SubjectSex,           m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize));

		if(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryIdentifier) m_pCallback->setValue(IBoxAlgorithmExperimentInformationInputReaderCallback::Value_LaboratoryIdentifier, m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize));
		if(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryName)       m_pCallback->setValue(IBoxAlgorithmExperimentInformationInputReaderCallback::Value_LaboratoryName,       m_pReaderHelper->getASCIIStringFromChildData(pBuffer, ui64BufferSize));
		if(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianIdentifier) m_pCallback->setValue(IBoxAlgorithmExperimentInformationInputReaderCallback::Value_TechnicianIdentifier, m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize));
		if(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianName)       m_pCallback->setValue(IBoxAlgorithmExperimentInformationInputReaderCallback::Value_TechnicianName,       m_pReaderHelper->getASCIIStringFromChildData(pBuffer, ui64BufferSize));
	}
	else
	{
		CThisClassParent::processChildData(pBuffer, ui64BufferSize);
	}
}

template <class IBaseInterface>
void TBoxAlgorithmExperimentInformationInputReaderCallback<IBaseInterface>::closeChild(void)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Header_ExperimentInformation)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Experiment)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Experiment_Identifier)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Experiment_Date)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject_Identifier)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject_Name)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject_Age)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Subject_Gender)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryIdentifier)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryName)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianIdentifier)
	 ||(l_rTop==OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianName))
	{
	}
	else
	{
		CThisClassParent::closeChild();
	}

	m_vNodes.pop();
}

// ________________________________________________________________________________________________________________
//

};

#endif // __OpenViBEToolkit_Reader_TBoxAlgorithmExperimentInformationInputReaderCallback_INL__
