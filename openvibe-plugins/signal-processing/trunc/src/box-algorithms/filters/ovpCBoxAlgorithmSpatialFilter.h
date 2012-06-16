#ifndef __OpenViBEPlugins_BoxAlgorithm_SpatialFilter_H__
#define __OpenViBEPlugins_BoxAlgorithm_SpatialFilter_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <vector>
/* */
#define OVP_ClassId_BoxAlgorithm_SpatialFilter     OpenViBE::CIdentifier(0xDD332C6C, 0x195B4FD4)
#define OVP_ClassId_BoxAlgorithm_SpatialFilterDesc OpenViBE::CIdentifier(0x72A01C92, 0xF8C1FA24)
/* */
/*
#define OVP_ClassId_BoxAlgorithm_SpatialFilter     OpenViBE::CIdentifier(0xDD332C6C, 0x195B4FD5)
#define OVP_ClassId_BoxAlgorithm_SpatialFilterDesc OpenViBE::CIdentifier(0x72A01C92, 0xF8C1FA25)
*/
namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmSpatialFilter : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SpatialFilter);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pMatrix;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pMemoryBuffer;

			std::vector < OpenViBE::float64 > m_vCoefficient;
/*
			OpenViBE::uint32 m_ui32OutputChannelCount;
			OpenViBE::uint32 m_ui32InputChannelCount;
*/
		};

		class CBoxAlgorithmSpatialFilterListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			virtual OpenViBE::boolean isValidTypeIdentifier(const OpenViBE::CIdentifier& rTypeIdenfitier)
			{
				OpenViBE::boolean l_bValid=false;
				l_bValid|=(rTypeIdenfitier==OV_TypeId_StreamedMatrix);
				l_bValid|=(rTypeIdenfitier==OV_TypeId_Spectrum);
				l_bValid|=(rTypeIdenfitier==OV_TypeId_Signal);
				return l_bValid;
			}

			virtual OpenViBE::boolean onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getInputType(0, l_oTypeIdentifier);
				if(this->isValidTypeIdentifier(l_oTypeIdentifier))
				{
					rBox.setOutputType(0, l_oTypeIdentifier);
				}
				return true;
			}

			virtual OpenViBE::boolean onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(0, l_oTypeIdentifier);
				if(this->isValidTypeIdentifier(l_oTypeIdentifier))
				{
					rBox.setInputType(0, l_oTypeIdentifier);
				}
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmSpatialFilterDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Spatial Filter"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Filtering"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_SpatialFilter; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmSpatialFilter; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmSpatialFilterListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Input Signal",                OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput ("Output Signal",               OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Spatial Fitler Coefficients", OV_TypeId_String,  "1;0;0;0;0;1;0;0;0;0;1;0;0;0;0;1");
				rBoxAlgorithmPrototype.addSetting("Number of Output Channels",   OV_TypeId_Integer, "4");
				rBoxAlgorithmPrototype.addSetting("Number of Input Channels",    OV_TypeId_Integer, "4");
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanModifyInput);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SpatialFilterDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_SpatialFilter_H__
