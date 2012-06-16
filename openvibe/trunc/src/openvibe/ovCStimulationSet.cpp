#include "ovCStimulationSet.h"

#include <vector>

using namespace OpenViBE;
using namespace std;

namespace OpenViBE
{
	namespace
	{
		class CStimulation
		{
		public:

			CStimulation(void)
				:m_ui64Identifier(0)
				,m_ui64Date(0)
				,m_ui64Duration(0)
			{
			}

			CStimulation(const uint64 ui64Identifier, const uint64 ui64Date, const uint64 ui64Duration)
				:m_ui64Identifier(ui64Identifier)
				,m_ui64Date(ui64Date)
				,m_ui64Duration(ui64Duration)
			{
			}

			uint64 m_ui64Identifier;
			uint64 m_ui64Date;
			uint64 m_ui64Duration;
		};

		class CStimulationSetImpl : public IStimulationSet
		{
		public:

			virtual void clear(void);

			virtual const uint64 getStimulationCount(void) const;
			virtual const uint64 getStimulationIdentifier(const uint64 ui64StimulationIndex) const;
			virtual const uint64 getStimulationDate(const uint64 ui64StimulationIndex) const;
			virtual const uint64 getStimulationDuration(const uint64 ui64StimulationIndex) const;

			virtual boolean setStimulationCount(const uint64 ui64StimulationCount);
			virtual boolean setStimulationIdentifier(const uint64 ui64StimulationIndex, const uint64 ui64StimulationIdentifier);
			virtual boolean setStimulationDate(const uint64 ui64StimulationIndex, const uint64 ui64StimulationDate);
			virtual boolean setStimulationDuration(const uint64 ui64StimulationIndex, const uint64 ui64StimulationDuration);

			virtual uint64 appendStimulation(const uint64 ui64StimulationIdentifier, const uint64 ui64StimulationDate, const uint64 ui64StimulationDuration);
			virtual uint64 insertStimulation(const uint64 ui64StimulationIndex, const uint64 ui64StimulationIdentifier, const uint64 ui64StimulationDate, const uint64 ui64StimulationDuration);
			virtual boolean removeStimulation(const uint64 ui64StimulationIndex);

			_IsDerivedFromClass_Final_(IStimulationSet, OV_ClassId_StimulationSetImpl);

		private:

			vector < CStimulation > m_vStimulation;
		};
	};
};

// ________________________________________________________________________________________________________________
//

void CStimulationSetImpl::clear(void)
{
	m_vStimulation.clear();
}

const uint64 CStimulationSetImpl::getStimulationCount(void) const
{
	return m_vStimulation.size();
}

const uint64 CStimulationSetImpl::getStimulationIdentifier(const uint64 ui64StimulationIndex) const
{
	return m_vStimulation[static_cast<size_t>(ui64StimulationIndex)].m_ui64Identifier;
}

const uint64 CStimulationSetImpl::getStimulationDate(const uint64 ui64StimulationIndex) const
{
	return m_vStimulation[static_cast<size_t>(ui64StimulationIndex)].m_ui64Date;
}

const uint64 CStimulationSetImpl::getStimulationDuration(const uint64 ui64StimulationIndex) const
{
	return m_vStimulation[static_cast<size_t>(ui64StimulationIndex)].m_ui64Duration;
}

boolean CStimulationSetImpl::setStimulationCount(const uint64 ui64StimulationCount)
{
	m_vStimulation.resize(static_cast<size_t>(ui64StimulationCount));
	return true;
}

boolean CStimulationSetImpl::setStimulationIdentifier(const uint64 ui64StimulationIndex, const uint64 ui64StimulationIdentifier)
{
	m_vStimulation[static_cast<size_t>(ui64StimulationIndex)].m_ui64Identifier=ui64StimulationIdentifier;
	return true;
}

boolean CStimulationSetImpl::setStimulationDate(const uint64 ui64StimulationIndex, const uint64 ui64StimulationDate)
{
	m_vStimulation[static_cast<size_t>(ui64StimulationIndex)].m_ui64Date=ui64StimulationDate;
	return true;
}

boolean CStimulationSetImpl::setStimulationDuration(const uint64 ui64StimulationIndex, const uint64 ui64StimulationDuration)
{
	m_vStimulation[static_cast<size_t>(ui64StimulationIndex)].m_ui64Duration=ui64StimulationDuration;
	return true;
}

uint64 CStimulationSetImpl::appendStimulation(const uint64 ui64StimulationIdentifier, const uint64 ui64StimulationDate, const uint64 ui64StimulationDuration)
{
	m_vStimulation.push_back(CStimulation(ui64StimulationIdentifier, ui64StimulationDate, ui64StimulationDuration));
	return m_vStimulation.size()-1;
}

uint64 CStimulationSetImpl::insertStimulation(const uint64 ui64StimulationIndex, const uint64 ui64StimulationIdentifier, const uint64 ui64StimulationDate, const uint64 ui64StimulationDuration)
{
	if(ui64StimulationIndex>=m_vStimulation.size())
	{
		return false;
	}
	if(ui64StimulationIndex==m_vStimulation.size())
	{
		m_vStimulation.push_back(CStimulation(ui64StimulationIdentifier, ui64StimulationDate, ui64StimulationDuration));
	}
	else
	{
		m_vStimulation.insert(m_vStimulation.begin()+static_cast<size_t>(ui64StimulationIndex), CStimulation(ui64StimulationIdentifier, ui64StimulationDate, ui64StimulationDuration));
	}
	return true;
}

boolean CStimulationSetImpl::removeStimulation(const uint64 ui64StimulationIndex)
{
	if(ui64StimulationIndex>=m_vStimulation.size())
	{
		return false;
	}
	m_vStimulation.erase(m_vStimulation.begin()+static_cast<size_t>(ui64StimulationIndex));
	return true;
}

// ________________________________________________________________________________________________________________
//

CStimulationSet::CStimulationSet(void)
	:m_pStimulationSetImpl(NULL)
{
	m_pStimulationSetImpl=new CStimulationSetImpl();
}

CStimulationSet::~CStimulationSet(void)
{
	delete m_pStimulationSetImpl;
}

void CStimulationSet::clear(void)
{
	m_pStimulationSetImpl->clear();
}

const uint64 CStimulationSet::getStimulationCount(void) const
{
	return m_pStimulationSetImpl->getStimulationCount();
}

const uint64 CStimulationSet::getStimulationIdentifier(const uint64 ui64StimulationIndex) const
{
	return m_pStimulationSetImpl->getStimulationIdentifier(ui64StimulationIndex);
}

const uint64 CStimulationSet::getStimulationDate(const uint64 ui64StimulationIndex) const
{
	return m_pStimulationSetImpl->getStimulationDate(ui64StimulationIndex);
}

const uint64 CStimulationSet::getStimulationDuration(const uint64 ui64StimulationIndex) const
{
	return m_pStimulationSetImpl->getStimulationDuration(ui64StimulationIndex);
}

boolean CStimulationSet::setStimulationCount(const uint64 ui64StimulationCount)
{
	return m_pStimulationSetImpl->setStimulationCount(ui64StimulationCount);
}

boolean CStimulationSet::setStimulationIdentifier(const uint64 ui64StimulationIndex, const uint64 ui64StimulationIdentifier)
{
	return m_pStimulationSetImpl->setStimulationIdentifier(ui64StimulationIndex, ui64StimulationIdentifier);
}

boolean CStimulationSet::setStimulationDate(const uint64 ui64StimulationIndex, const uint64 ui64StimulationDate)
{
	return m_pStimulationSetImpl->setStimulationDate(ui64StimulationIndex, ui64StimulationDate);
}

boolean CStimulationSet::setStimulationDuration(const uint64 ui64StimulationIndex, const uint64 ui64StimulationDuration)
{
	return m_pStimulationSetImpl->setStimulationDuration(ui64StimulationIndex, ui64StimulationDuration);
}

uint64 CStimulationSet::appendStimulation(const uint64 ui64StimulationIdentifier, const uint64 ui64StimulationDate, const uint64 ui64StimulationDuration)
{
	return m_pStimulationSetImpl->appendStimulation(ui64StimulationIdentifier, ui64StimulationDate, ui64StimulationDuration);
}

uint64 CStimulationSet::insertStimulation(const uint64 ui64StimulationIndex, const uint64 ui64StimulationIdentifier, const uint64 ui64StimulationDate, const uint64 ui64StimulationDuration)
{
	return m_pStimulationSetImpl->insertStimulation(ui64StimulationIndex, ui64StimulationIdentifier, ui64StimulationDate, ui64StimulationDuration);
}

boolean CStimulationSet::removeStimulation(const uint64 ui64StimulationIndex)
{
	return m_pStimulationSetImpl->removeStimulation(ui64StimulationIndex);
}
