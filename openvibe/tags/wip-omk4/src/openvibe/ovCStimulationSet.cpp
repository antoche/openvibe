#include "ovCStimulationSet.h"

#include <vector>

using namespace OpenViBE;
using namespace std;

namespace OpenViBE
{
	namespace
	{
		typedef struct
		{
			uint64 m_ui64Identifier;
			uint64 m_ui64Date;
			uint64 m_ui64Duration;
		} SStimulation;

		class CStimulationSetImpl : public IStimulationSet
		{
		public:

			virtual const uint64 getStimulationCount(void) const;
			virtual const uint64 getStimulationIdentifier(const uint64 ui64StimulationIndex) const;
			virtual const uint64 getStimulationDate(const uint64 ui64StimulationIndex) const;
			virtual const uint64 getStimulationDuration(const uint64 ui64StimulationIndex) const;

			virtual boolean setStimulationCount(const uint64 ui64StimulationCount);
			virtual boolean setStimulationIdentifier(const uint64 ui64StimulationIndex, const uint64 ui64StimulationIdentifier);
			virtual boolean setStimulationDate(const uint64 ui64StimulationIndex, const uint64 ui64StimulationDate);
			virtual boolean setStimulationDuration(const uint64 ui64StimulationIndex, const uint64 ui64StimulationDuration);

			_IsDerivedFromClass_Final_(IStimulationSet, OV_ClassId_StimulationSetImpl);

		private:

			vector < SStimulation > m_vStimulation;
		};
	};
};

// ________________________________________________________________________________________________________________
//

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
