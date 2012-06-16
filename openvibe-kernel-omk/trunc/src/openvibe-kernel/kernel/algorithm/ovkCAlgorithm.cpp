#include "ovkCAlgorithm.h"
#include "ovkCAlgorithmContext.h"
#include "ovkCAlgorithmProto.h"

#include "../../ovk_tools.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace std;

#define _MaxCrash_ 5

#define __proxy_func_0__(_func_, _message_, _before_, _after_) \
	{ \
		if(!m_bActive) \
		{ \
			return false; \
		} \
/* getLogManager() << LogLevel_Debug << "CAlgorithm#1: " << _message_ << "\n"; */ \
		CAlgorithmContext l_oAlgorithmContext(getKernelContext(), *this, m_rAlgorithmDesc); \
		boolean l_bResult=false; \
		try \
		{ \
/* getLogManager() << LogLevel_Debug << "CAlgorithm#2: " << _message_ << "\n"; */ \
			_before_; \
/* getLogManager() << LogLevel_Debug << "CAlgorithm#3: " << _message_ << "\n"; */ \
			l_bResult=m_rAlgorithm._func_(l_oAlgorithmContext); \
/* getLogManager() << LogLevel_Debug << "CAlgorithm#4: " << _message_ << "\n"; */ \
			_after_; \
/* getLogManager() << LogLevel_Debug << "CAlgorithm#5: " << _message_ << "\n"; */ \
		} \
		catch (...) \
		{ \
			this->handleCrash(_message_); \
			return false; \
		} \
/* getLogManager() << LogLevel_Debug << "CAlgorithm#6: " << _message_ << "\n"; */ \
		return l_bResult; \
	}

CAlgorithm::CAlgorithm(const IKernelContext& rKernelContext, IAlgorithm& rAlgorithm, const IAlgorithmDesc& rAlgorithmDesc)
	:TKernelObject < IKernelObject >(rKernelContext)
	,m_pInputConfigurable(NULL)
	,m_pOutputConfigurable(NULL)
	,m_ui32CrashCount(0)
	,m_bActive(true)
	,m_rAlgorithmDesc(rAlgorithmDesc)
	,m_rAlgorithm(rAlgorithm)
{
	m_pInputConfigurable=dynamic_cast<IConfigurable*>(getKernelContext().getKernelObjectFactory().createObject(OV_ClassId_Kernel_Configurable));
	m_pOutputConfigurable=dynamic_cast<IConfigurable*>(getKernelContext().getKernelObjectFactory().createObject(OV_ClassId_Kernel_Configurable));

	// FIXME
	CAlgorithmProto l_oProto(rKernelContext, *this);
	rAlgorithmDesc.getAlgorithmPrototype(l_oProto);
}

CAlgorithm::~CAlgorithm(void)
{
	getKernelContext().getKernelObjectFactory().releaseObject(m_pOutputConfigurable);
	getKernelContext().getKernelObjectFactory().releaseObject(m_pInputConfigurable);
}

IAlgorithm& CAlgorithm::getAlgorithm(void)
{
	return m_rAlgorithm;
}

const IAlgorithm& CAlgorithm::getAlgorithm(void) const
{
	return m_rAlgorithm;
}

const IAlgorithmDesc& CAlgorithm::getAlgorithmDesc(void) const
{
	return m_rAlgorithmDesc;
}

boolean CAlgorithm::addInputParameter(
	const CIdentifier& rInputParameterIdentifier,
	const CString& sInputName,
	const EParameterType eParameterType,
	const CIdentifier& rSubTypeIdentifier)
{
	if(m_pInputConfigurable->getParameter(rInputParameterIdentifier)!=NULL)
	{
		getLogManager() << LogLevel_Warning << "For algorithm " << m_rAlgorithmDesc.getName() << " : Input parameter id " << rInputParameterIdentifier << " already exists\n";
		return false;
	}
	m_pInputConfigurable->createParameter(rInputParameterIdentifier, eParameterType, rSubTypeIdentifier);
	m_vInputParameterName[rInputParameterIdentifier]=sInputName;
	return true;
}

CIdentifier CAlgorithm::getNextInputParameterIdentifier(
	const CIdentifier& rPreviousInputParameterIdentifier) const
{
	return m_pInputConfigurable->getNextParameterIdentifier(rPreviousInputParameterIdentifier);
}

IParameter* CAlgorithm::getInputParameter(
	const CIdentifier& rInputParameterIdentifier)
{
	IParameter* l_pParameter=m_pInputConfigurable->getParameter(rInputParameterIdentifier);
	if(!l_pParameter)
	{
		getLogManager() << LogLevel_Warning << "For algorithm " << m_rAlgorithmDesc.getName() << " : Requested null input parameter id " << rInputParameterIdentifier;
		if(m_pOutputConfigurable->getParameter(rInputParameterIdentifier))
		{
			getLogManager() << " Did you mean " << CString("output") << " parameter ?";
		}
		getLogManager() << "\n";
	}
	return l_pParameter;
}

EParameterType CAlgorithm::getInputParameterType(
	const CIdentifier& rInputParameterIdentifier) const
{
	IParameter* l_pParameter=m_pInputConfigurable->getParameter(rInputParameterIdentifier);
	if(!l_pParameter)
	{
		return ParameterType_None;
	}
	return l_pParameter->getType();
}

CString CAlgorithm::getInputParameterName(
	const CIdentifier& rInputParameterIdentifier) const
{
	map<CIdentifier, CString>::const_iterator itName=m_vInputParameterName.find(rInputParameterIdentifier);
	if(itName==m_vInputParameterName.end())
	{
		return "";
	}
	return itName->second;
}

boolean CAlgorithm::removeInputParameter(
	const CIdentifier& rInputParameterIdentifier)
{
	if(!m_pInputConfigurable->removeParameter(rInputParameterIdentifier))
	{
		return false;
	}
	m_vInputParameterName.erase(m_vInputParameterName.find(rInputParameterIdentifier));
	return true;
}

boolean CAlgorithm::addOutputParameter(
	const CIdentifier& rOutputParameterIdentifier,
	const CString& sOutputName,
	const EParameterType eParameterType,
	const CIdentifier& rSubTypeIdentifier)
{
	if(m_pOutputConfigurable->getParameter(rOutputParameterIdentifier)!=NULL)
	{
		getLogManager() << LogLevel_Warning << "For algorithm " << m_rAlgorithmDesc.getName() << " : Output parameter id " << rOutputParameterIdentifier << " already exists\n";
		return false;
	}
	m_pOutputConfigurable->createParameter(rOutputParameterIdentifier, eParameterType, rSubTypeIdentifier);
	m_vOutputParameterName[rOutputParameterIdentifier]=sOutputName;
	return true;
}

CIdentifier CAlgorithm::getNextOutputParameterIdentifier(
	const CIdentifier& rPreviousOutputParameterIdentifier) const
{
	return m_pOutputConfigurable->getNextParameterIdentifier(rPreviousOutputParameterIdentifier);
}

IParameter* CAlgorithm::getOutputParameter(
	const CIdentifier& rOutputParameterIdentifier)
{
	IParameter* l_pParameter=m_pOutputConfigurable->getParameter(rOutputParameterIdentifier);
	if(!l_pParameter)
	{
		getLogManager() << LogLevel_Warning << "For algorithm " << m_rAlgorithmDesc.getName() << " : Requested null output parameter id " << rOutputParameterIdentifier;
		if(m_pInputConfigurable->getParameter(rOutputParameterIdentifier))
		{
			getLogManager() << " Did you mean " << CString("input") << " parameter ?";
		}
		getLogManager() << "\n";
	}
	return l_pParameter;
}

EParameterType CAlgorithm::getOutputParameterType(
	const CIdentifier& rOutputParameterIdentifier) const
{
	IParameter* l_pParameter=m_pOutputConfigurable->getParameter(rOutputParameterIdentifier);
	if(!l_pParameter)
	{
		return ParameterType_None;
	}
	return l_pParameter->getType();
}

CString CAlgorithm::getOutputParameterName(
	const CIdentifier& rOutputParameterIdentifier) const
{
	map<CIdentifier, CString>::const_iterator itName=m_vOutputParameterName.find(rOutputParameterIdentifier);
	if(itName==m_vOutputParameterName.end())
	{
		return "";
	}
	return itName->second;
}

boolean CAlgorithm::removeOutputParameter(
	const CIdentifier& rOutputParameterIdentifier)
{
	if(!m_pOutputConfigurable->removeParameter(rOutputParameterIdentifier))
	{
		return false;
	}
	m_vOutputParameterName.erase(m_vOutputParameterName.find(rOutputParameterIdentifier));
	return true;
}

boolean CAlgorithm::addInputTrigger(
	const CIdentifier& rInputTriggerIdentifier,
	const CString& rInputTriggerName)
{
	if(m_vInputTrigger.find(rInputTriggerIdentifier)!=m_vInputTrigger.end())
	{
		return false;
	}
	m_vInputTrigger[rInputTriggerIdentifier].first=rInputTriggerName;
	m_vInputTrigger[rInputTriggerIdentifier].second=false;
	return true;
}

CIdentifier CAlgorithm::getNextInputTriggerIdentifier(
	const CIdentifier& rPreviousInputTriggerIdentifier) const
{
	return getNextIdentifier< pair<CString, boolean> >(m_vInputTrigger, rPreviousInputTriggerIdentifier);
}

CString CAlgorithm::getInputTriggerName(
	const CIdentifier& rInputTriggerIdentifier) const
{
	map<CIdentifier, pair<CString, boolean> >::const_iterator itTrigger=m_vInputTrigger.find(rInputTriggerIdentifier);
	if(itTrigger==m_vInputTrigger.end())
	{
		return "";
	}
	return itTrigger->second.first;
}

boolean CAlgorithm::isInputTriggerActive(
	const CIdentifier& rInputTriggerIdentifier) const
{
	map<CIdentifier, pair<CString, boolean> >::const_iterator itTrigger=m_vInputTrigger.find(rInputTriggerIdentifier);
	if(itTrigger==m_vInputTrigger.end())
	{
		return false;
	}
	return itTrigger->second.second;
}

boolean CAlgorithm::activateInputTrigger(
	const CIdentifier& rInputTriggerIdentifier,
	const boolean bTriggerState)
{
	map<CIdentifier, pair<CString, boolean> >::iterator itTrigger=m_vInputTrigger.find(rInputTriggerIdentifier);
	if(itTrigger==m_vInputTrigger.end())
	{
		return false;
	}
	itTrigger->second.second=true;
	return true;
}

boolean CAlgorithm::removeInputTrigger(
	const CIdentifier& rInputTriggerIdentifier)
{
	map<CIdentifier, pair<CString, boolean> >::iterator itTrigger=m_vInputTrigger.find(rInputTriggerIdentifier);
	if(itTrigger==m_vInputTrigger.end())
	{
		return false;
	}
	m_vInputTrigger.erase(itTrigger);
	return true;
}

boolean CAlgorithm::addOutputTrigger(
	const CIdentifier& rOutputTriggerIdentifier,
	const CString& rOutputTriggerName)
{
	if(m_vOutputTrigger.find(rOutputTriggerIdentifier)!=m_vOutputTrigger.end())
	{
		return false;
	}
	m_vOutputTrigger[rOutputTriggerIdentifier].first=rOutputTriggerName;
	m_vOutputTrigger[rOutputTriggerIdentifier].second=false;
	return true;
}

CIdentifier CAlgorithm::getNextOutputTriggerIdentifier(
	const CIdentifier& rPreviousOutputTriggerIdentifier) const
{
	return getNextIdentifier< pair<CString, boolean> >(m_vOutputTrigger, rPreviousOutputTriggerIdentifier);
}

CString CAlgorithm::getOutputTriggerName(
	const CIdentifier& rOutputTriggerIdentifier) const
{
	map<CIdentifier, pair<CString, boolean> >::const_iterator itTrigger=m_vOutputTrigger.find(rOutputTriggerIdentifier);
	if(itTrigger==m_vOutputTrigger.end())
	{
		return "";
	}
	return itTrigger->second.first;
}

boolean CAlgorithm::isOutputTriggerActive(
	const CIdentifier& rOutputTriggerIdentifier) const
{
	map<CIdentifier, pair<CString, boolean> >::const_iterator itTrigger=m_vOutputTrigger.find(rOutputTriggerIdentifier);
	if(itTrigger==m_vOutputTrigger.end())
	{
		return false;
	}
	return itTrigger->second.second;
}

boolean CAlgorithm::activateOutputTrigger(
	const CIdentifier& rOutputTriggerIdentifier,
	const boolean bTriggerState)
{
	map<CIdentifier, pair<CString, boolean> >::iterator itTrigger=m_vOutputTrigger.find(rOutputTriggerIdentifier);
	if(itTrigger==m_vOutputTrigger.end())
	{
		return false;
	}
	itTrigger->second.second=true;
	return true;
}

boolean CAlgorithm::removeOutputTrigger(
	const CIdentifier& rOutputTriggerIdentifier)
{
	map<CIdentifier, pair<CString, boolean> >::iterator itTrigger=m_vOutputTrigger.find(rOutputTriggerIdentifier);
	if(itTrigger==m_vOutputTrigger.end())
	{
		return false;
	}
	m_vOutputTrigger.erase(itTrigger);
	return true;
}


boolean CAlgorithm::initialize(void)
{
	__proxy_func_0__(initialize, "initialize callback", , );
}

boolean CAlgorithm::uninitialize(void)
{
	__proxy_func_0__(uninitialize, "uninitialize callback", , );
}

boolean CAlgorithm::process(void)
{
	__proxy_func_0__(process, "process callback", this->setAllOutputTriggers(false), this->setAllInputTriggers(false));
}

boolean CAlgorithm::process(
	const CIdentifier& rTriggerIdentifier)
{
	if(!m_bActive)
	{
		return false;
	}

	if(!this->activateInputTrigger(rTriggerIdentifier, true))
	{
		return false;
	}

	__proxy_func_0__(process, "process callback", this->setAllOutputTriggers(false), this->setAllInputTriggers(false));
}

void CAlgorithm::setAllInputTriggers(const boolean bTriggerStatus)
{
	map<CIdentifier, pair<CString, boolean> >::iterator itTrigger;
	for(itTrigger=m_vInputTrigger.begin(); itTrigger!=m_vInputTrigger.end(); itTrigger++)
	{
		itTrigger->second.second=bTriggerStatus;
	}
}

void CAlgorithm::setAllOutputTriggers(const boolean bTriggerStatus)
{
	map<CIdentifier, pair<CString, boolean> >::iterator itTrigger;
	for(itTrigger=m_vOutputTrigger.begin(); itTrigger!=m_vOutputTrigger.end(); itTrigger++)
	{
		itTrigger->second.second=bTriggerStatus;
	}
}

void CAlgorithm::handleCrash(const char* sWhere)
{
	m_ui32CrashCount++;

	this->getLogManager() << LogLevel_Error << "Plugin code caused crash " << m_ui32CrashCount << " time(s)\n";
	this->getLogManager() << LogLevel_Error << "  [name:" << m_rAlgorithmDesc.getName() << "]\n";
	this->getLogManager() << LogLevel_Error << "  [class identifier:" << m_rAlgorithm.getClassIdentifier() << "]\n";
	this->getLogManager() << LogLevel_Error << "  [location:" << sWhere << "]\n";

	if(m_ui32CrashCount>=_MaxCrash_)
	{
		this->getLogManager() << LogLevel_Fatal << "  This plugin has been disabled !\n";
		m_bActive=false;
	}
}
