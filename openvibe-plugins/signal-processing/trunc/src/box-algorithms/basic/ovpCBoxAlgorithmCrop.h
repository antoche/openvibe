#ifndef __OpenViBEPlugins_BoxAlgorithm_Crop_H__
#define __OpenViBEPlugins_BoxAlgorithm_Crop_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>


namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmCrop : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_Crop);

		protected:

			OpenViBE::IMatrix* m_pMatrix;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;
			OpenViBE::float64 m_f64MinCropValue;
			OpenViBE::float64 m_f64MaxCropValue;
			OpenViBE::uint64 m_ui64CropMethod;
		};

		class CBoxAlgorithmCropListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			virtual OpenViBE::boolean onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getInputType(ui32Index, l_oTypeIdentifier);
				if(this->getTypeManager().isDerivedFromStream(l_oTypeIdentifier, OV_TypeId_StreamedMatrix))
				{
					rBox.setOutputType(ui32Index, l_oTypeIdentifier);
				}
				else
				{
					rBox.getOutputType(ui32Index, l_oTypeIdentifier);
					rBox.setInputType(ui32Index, l_oTypeIdentifier);
				}
				return true;
			}

			virtual OpenViBE::boolean onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(ui32Index, l_oTypeIdentifier);
				if(this->getTypeManager().isDerivedFromStream(l_oTypeIdentifier, OV_TypeId_StreamedMatrix))
				{
					rBox.setInputType(ui32Index, l_oTypeIdentifier);
				}
				else
				{
					rBox.getInputType(ui32Index, l_oTypeIdentifier);
					rBox.setOutputType(ui32Index, l_oTypeIdentifier);
				}
				return true;
			};

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmCropDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Crop"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-cut"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_Crop; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmCrop; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmCropListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Input matrix",  OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addOutput ("Output matrix", OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addSetting("Crop method",   OVP_TypeId_CropMethod, OVP_TypeId_CropMethod_MinMax.toString());
				rBoxAlgorithmPrototype.addSetting("Min crop value",    OV_TypeId_Float, "-1");
				rBoxAlgorithmPrototype.addSetting("Max crop value",    OV_TypeId_Float, "1");
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanModifyInput);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_CropDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_Crop_H__
