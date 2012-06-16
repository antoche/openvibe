#if 0
#ifndef __OpenViBEPlugins_SignalProcessing_Algorithms_Filter_CApplySpatialFilter_H__
#define __OpenViBEPlugins_SignalProcessing_Algorithms_Filter_CApplySpatialFilter_H__

#include "../../ovp_defines.h"

#include <openvibe/ov_all.h>

#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CApplySpatialFilter : virtual public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_ApplySpatialFilter);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_oSignalIntputMatrixHandle;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_oFilterCoefficientsInputMatrixHandle;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_oSignalOutputMatrixHandle;

			OpenViBE::boolean m_bActive;
			OpenViBE::CMatrix* m_vInputMatrixCache;
		};

		class CApplySpatialFilterDesc : virtual public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Apply spatial filter"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM/U821"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Filtering"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_ApplySpatialFilter; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CApplySpatialFilter(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmProto) const
			{
				rAlgorithmProto.addInputParameter(OVP_Algorithm_ApplySpatialFilter_InputParameterId_SignalMatrix,             "Signal Input matrix",   OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addInputParameter(OVP_Algorithm_ApplySpatialFilter_InputParameterId_FilterCoefficientsMatrix, "Spatial Filter matrix", OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_ApplySpatialFilter_OutputParameterId_FilteredSignalMatrix,   "Signal Output matrix",  OpenViBE::Kernel::ParameterType_Matrix);

				rAlgorithmProto.addInputTrigger(OVP_Algorithm_ApplySpatialFilter_InputTriggerId_Initialize,  "Initialize");
				rAlgorithmProto.addInputTrigger(OVP_Algorithm_ApplySpatialFilter_InputTriggerId_ApplyFilter, "Apply filter");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_ApplySpatialFilterDesc);
		};
	};
};

#endif // __OpenViBEPlugins_SignalProcessing_Algorithms_Filter_CApplySpatialFilter_H__
#endif
