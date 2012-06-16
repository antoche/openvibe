#ifndef __OpenViBEPlugins_BoxAlgorithm_LevelMeasure_H__
#define __OpenViBEPlugins_BoxAlgorithm_LevelMeasure_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		class CBoxAlgorithmLevelMeasure : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_LevelMeasure);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamedMatrixDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pLevelMeasure;

			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pStreamedMatrixDecoderMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pStreamedMatrixDecoderMatrix;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pLevelMeasureMatrix;
			OpenViBE::Kernel::TParameterHandler < ::GtkWidget* > op_pLevelMeasureMainWidget;
			OpenViBE::Kernel::TParameterHandler < ::GtkWidget* > op_pLevelMeasureToolbarWidget;

			OpenViBE::IMatrix* m_pMatrix;
		};

		class CBoxAlgorithmLevelMeasureDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Level measure"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Visualisation/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-go-up"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_LevelMeasure; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SimpleVisualisation::CBoxAlgorithmLevelMeasure; }

			virtual OpenViBE::boolean hasFunctionality(OpenViBE::Kernel::EPluginFunctionality ePluginFunctionality) const
			{
				return ePluginFunctionality == OpenViBE::Kernel::PluginFunctionality_Visualization;
			}

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input matrix to display", OV_TypeId_StreamedMatrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_LevelMeasureDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_LevelMeasure_H__
