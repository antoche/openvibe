#ifndef __OpenViBEPlugins_BoxAlgorithm_FrequencyBandSelector_H__
#define __OpenViBEPlugins_BoxAlgorithm_FrequencyBandSelector_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <vector>
#include <map>

#define OVP_ClassId_BoxAlgorithm_FrequencyBandSelector     OpenViBE::CIdentifier(0x140C19C6, 0x4E6E187B)
#define OVP_ClassId_BoxAlgorithm_FrequencyBandSelectorDesc OpenViBE::CIdentifier(0x13462C56, 0x794E3C07)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmFrequencyBandSelector : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_FrequencyBandSelector);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pBands;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pBands;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pMemoryBuffer;

			OpenViBE::CMatrix m_oMatrix;
			std::vector < std::pair < OpenViBE::float64, OpenViBE::float64 > > m_vSelected;
			std::vector < OpenViBE::float64 > m_vSelectionFactor;
		};

		class CBoxAlgorithmFrequencyBandSelectorDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Frequency Band Selector"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Preserves some spectrum coefficients and puts the others to zero depending on a list of frequencies / frequency bands to select"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Spectral analysis"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_FrequencyBandSelector; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmFrequencyBandSelector; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Input spectrum",  OV_TypeId_Spectrum);
				rBoxAlgorithmPrototype.addOutput ("Output spectrum", OV_TypeId_Spectrum);
				rBoxAlgorithmPrototype.addSetting("Frequencies to select", OV_TypeId_String, "8-12;16-24");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_FrequencyBandSelectorDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_FrequencyBandSelector_H__
