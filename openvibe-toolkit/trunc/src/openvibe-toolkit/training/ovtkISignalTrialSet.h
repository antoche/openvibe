#ifndef __OpenViBEToolkit_Offline_ISignalTrialSet_H__
#define __OpenViBEToolkit_Offline_ISignalTrialSet_H__

#include "../ovtkIObject.h"

namespace OpenViBEToolkit
{
	class ISignalTrial;

	class OVTK_API ISignalTrialSet : public OpenViBEToolkit::IObject
	{
	public:

		virtual OpenViBE::boolean addSignalTrial(OpenViBEToolkit::ISignalTrial& rSignalTrial)=0;
		virtual OpenViBE::boolean clear(void)=0;

		virtual OpenViBE::uint32 getSignalTrialCount(void) const=0;
		virtual OpenViBEToolkit::ISignalTrial& getSignalTrial(OpenViBE::uint32 ui32Index) const=0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_);
	};

	extern OVTK_API OpenViBEToolkit::ISignalTrialSet* createSignalTrialSet(void);
	extern OVTK_API void releaseSignalTrialSet(OpenViBEToolkit::ISignalTrialSet* pSignalTrialSet);
};

#endif // __OpenViBEToolkit_Offline_ISignalTrialSet_H__
