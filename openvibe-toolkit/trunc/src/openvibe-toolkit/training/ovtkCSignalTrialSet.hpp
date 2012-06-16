#ifndef __OpenViBEToolkit_Offline_CSignalTrialSet_HPP__
#define __OpenViBEToolkit_Offline_CSignalTrialSet_HPP__

#include "ovtkISignalTrialSet.h"

#include <vector>

namespace OpenViBEToolkit
{
	class CSignalTrialSet : public OpenViBEToolkit::ISignalTrialSet
	{
	public:

		virtual OpenViBE::boolean addSignalTrial(OpenViBEToolkit::ISignalTrial& rSignalTrial);
		virtual OpenViBE::boolean clear(void);

		virtual OpenViBE::uint32 getSignalTrialCount(void) const;
		virtual OpenViBEToolkit::ISignalTrial& getSignalTrial(OpenViBE::uint32 ui32Index) const;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::ISignalTrialSet, OVTK_ClassId_);

	protected:

		mutable std::vector<OpenViBEToolkit::ISignalTrial*> m_vSignalTrial;
	};

	extern OVTK_API OpenViBEToolkit::ISignalTrialSet* createSignalTrialSet(void);
};

#endif // __OpenViBEToolkit_Offline_CSignalTrialSet_HPP__
