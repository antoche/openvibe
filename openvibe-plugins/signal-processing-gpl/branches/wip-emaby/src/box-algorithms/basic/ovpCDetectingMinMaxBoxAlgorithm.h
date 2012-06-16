#ifndef __OpenViBEPlugins_SignalProcessingGpl_BoxAlgorithms_Filter_CDetectingMinMaxBoxAlgorithm_H__
#define __OpenViBEPlugins_SignalProcessingGpl_BoxAlgorithms_Filter_CDetectingMinMaxBoxAlgorithm_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace SignalProcessingGpl
	{
		class CDetectingMinMaxBoxAlgorithm : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_Box_DetectingMinMaxBoxAlgorithm)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pDetectingMinMax;

			OpenViBE::uint64 m_ui64LastStartTime;
			OpenViBE::uint64 m_ui64LastEndTime;

			OpenViBE::boolean m_bMinFlag;
			OpenViBE::boolean m_bMaxFlag;
		};

		class CDetectingMinMaxBoxAlgorithmDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Min/Max detection - GPL"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM/U821"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString(""); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Box_DetectingMinMaxBoxAlgorithm; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CDetectingMinMaxBoxAlgorithm(); }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput  ("Input epochs",      OV_TypeId_Signal);
				rPrototype.addOutput ("Output epochs",     OV_TypeId_StreamedMatrix);
				rPrototype.addSetting("Min/Max",           OVP_TypeId_MinMax, "Max");
				rPrototype.addSetting("Time window start", OV_TypeId_Float,   "300");
				rPrototype.addSetting("Time window end",   OV_TypeId_Float,   "500");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_Box_DetectingMinMaxBoxAlgorithmDesc)
		};
	};
};

#endif // __OpenViBEPlugins_SignalProcessingGpl_BoxAlgorithms_Filter_CDetectingMinMaxBoxAlgorithm_H__
