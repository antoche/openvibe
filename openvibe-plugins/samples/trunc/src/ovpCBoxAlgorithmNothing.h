#ifndef __OpenViBEPlugins_BoxAlgorithm_Nothing_H__
#define __OpenViBEPlugins_BoxAlgorithm_Nothing_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_Nothing     OpenViBE::CIdentifier(0x273960C0, 0x485C407C)
#define OVP_ClassId_BoxAlgorithm_NothingDesc OpenViBE::CIdentifier(0x7A6167D9, 0x79070E22)


namespace OpenViBEPlugins
{
	namespace Samples
	{
		class CBoxAlgorithmNothing : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			// virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			// virtual OpenViBE::boolean processEvent(OpenViBE::CMessageEvent& rMessageEvent);
			// virtual OpenViBE::boolean processSignal(OpenViBE::CMessageSignal& rMessageSignal);
			// virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			// virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_Nothing);

		protected:

			// ...
		};

		class CBoxAlgorithmNothingDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Nothing"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("This box does nothing"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Samples"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-about"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_Nothing; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Samples::CBoxAlgorithmNothing; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				// rBoxAlgorithmPrototype.addInput  ("input name", /* input type (OV_TypeId_Signal) */);
				// rBoxAlgorithmPrototype.addOutput ("output name", /* output type (OV_TypeId_Signal) */);
				// rBoxAlgorithmPrototype.addSetting("setting name", /* setting type (OV_TypeId_Integer), "default value" */);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_NothingDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_Nothing_H__
