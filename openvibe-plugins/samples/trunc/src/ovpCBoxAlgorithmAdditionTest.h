#ifndef __SamplePlugin_CBoxAlgorithmAdditionTest_H__
#define __SamplePlugin_CBoxAlgorithmAdditionTest_H__

#include "ovp_defines.h"

#include <openvibe/ov_all.h>

#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace Samples
	{
		class CBoxAlgorithmAdditionTest : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			CBoxAlgorithmAdditionTest(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::Kernel::IMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithmAdditionTest)

		protected:

			OpenViBE::Kernel::ELogLevel m_eLogLevel;

			OpenViBE::int64 m_iInt64_1;
			OpenViBE::int64 m_iInt64_2;
			OpenViBE::int64 m_iInt64_3;
			OpenViBE::int64 m_iInt64_4;

			OpenViBE::Kernel::IAlgorithmProxy* m_pProxy1;
			OpenViBE::Kernel::IAlgorithmProxy* m_pProxy2;
			OpenViBE::Kernel::IAlgorithmProxy* m_pProxy3;
		};

		class CBoxAlgorithmAdditionTestDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Addition Test"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("This box illustrates how an algorithm can be used in a box"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("This specific sample computes 4 random numbers and uses 3 sum operator algorithms in order to get the total"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Samples"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithmAdditionTest; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Samples::CBoxAlgorithmAdditionTest(); }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addSetting("Log level to use", OV_TypeId_LogLevel, "Information");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithmAdditionTestDesc);
		};
	};
};

#endif // __SamplePlugin_CBoxAlgorithmAdditionTest_H__
