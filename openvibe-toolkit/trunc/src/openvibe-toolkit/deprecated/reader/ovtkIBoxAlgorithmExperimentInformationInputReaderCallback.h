#ifndef __OpenViBEToolkit_Reader_IBoxAlgorithmExperimentInformationInputReaderCallback_H__
#define __OpenViBEToolkit_Reader_IBoxAlgorithmExperimentInformationInputReaderCallback_H__

#include "ovtkIBoxAlgorithmEBMLInputReaderCallback.h"

namespace OpenViBEToolkit
{
	class OVTK_API IBoxAlgorithmExperimentInformationInputReaderCallback : public OpenViBEToolkit::IBoxAlgorithmEBMLInputReaderCallback
	{
	public:

		enum
		{
			Value_ExperimentIdentifier, // unsigned integer
			Value_SubjectIdentifier,    // unsigned integer
			Value_SubjectAge,           // unsigned integer
			Value_SubjectSex,           // unsigned integer
			Value_LaboratoryIdentifier, // unsigned integer
			Value_TechnicianIdentifier, // unsigned integer

			Value_ExperimentDate,       // string
			Value_SubjectName,          // string
			Value_LaboratoryName,       // string
			Value_TechnicianName,       // string
		};

		class OVTK_API ICallback
		{
		public:
			virtual void setValue(const OpenViBE::uint32 ui32ValueIdentifier, const OpenViBE::uint64 ui64Value)=0;
			virtual void setValue(const OpenViBE::uint32 ui32ValueIdentifier, const char* sValue)=0;
			virtual ~ICallback(void) { }
		};

		template <class COwnerClass>
		class TCallbackProxy1 : public OpenViBEToolkit::IBoxAlgorithmExperimentInformationInputReaderCallback::ICallback
		{
		public:
			TCallbackProxy1(
				COwnerClass& rOwnerObject,
				void (COwnerClass::*mfpSetValueI)(const OpenViBE::uint32 ui32ValueIdentifier, const OpenViBE::uint64 ui64Value),
				void (COwnerClass::*mfpSetValueS)(const OpenViBE::uint32 ui32ValueIdentifier, const char* sValue))
				:m_rOwnerObject(rOwnerObject)
				,m_mfpSetValueI(mfpSetValueI)
				,m_mfpSetValueS(mfpSetValueS)
			{
			}
			virtual void mfpSetValue(const OpenViBE::uint32 ui32ValueIdentifier, const OpenViBE::uint64 ui64Value)
			{
				if(m_mfpSetValueI)
				{
					(m_rOwnerObject.*m_mfpSetValueI)(ui32ValueIdentifier, ui64Value);
				}
			}
			virtual void mfpSetValue(const OpenViBE::uint32 ui32ValueIdentifier, const char* sValue)
			{
				if(m_mfpSetValueS)
				{
					(m_rOwnerObject.*m_mfpSetValueS)(ui32ValueIdentifier, sValue);
				}
			}
		protected:
			COwnerClass& m_rOwnerObject;
			void (COwnerClass::*m_mfpSetValueI)(const OpenViBE::uint32 ui32ValueIdentifier, const OpenViBE::uint64 ui64Value);
			void (COwnerClass::*m_mfpSetValueS)(const OpenViBE::uint32 ui32ValueIdentifier, const char* sValue);
		};

		_IsDerivedFromClass_(OpenViBEToolkit::IBoxAlgorithmEBMLInputReaderCallback, OVTK_ClassId_);
	};

	OVTK_API OpenViBEToolkit::IBoxAlgorithmExperimentInformationInputReaderCallback* createBoxAlgorithmExperimentInformationInputReaderCallback(OpenViBEToolkit::IBoxAlgorithmExperimentInformationInputReaderCallback::ICallback& rCallback);
	OVTK_API void releaseBoxAlgorithmExperimentInformationInputReaderCallback(OpenViBEToolkit::IBoxAlgorithmExperimentInformationInputReaderCallback* pBoxAlgorithmExperimentInformationInputReaderCallback);
};

#endif // __OpenViBEToolkit_Reader_IBoxAlgorithmExperimentInformationInputReaderCallback_H__
