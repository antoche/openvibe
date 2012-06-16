#ifndef __SamplePlugin_CAlgorithmAddition_H__
#define __SamplePlugin_CAlgorithmAddition_H__

#include "../ovp_defines.h"

#include <openvibe/ov_all.h>

#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace Samples
	{
		class CAlgorithmAddition : public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_AlgorithmAddition);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::int64 > m_oParameter1;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::int64 > m_oParameter2;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::int64 > m_oParameter3;
		};

		class CAlgorithmAdditionDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Addition"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Computes and outputs the sum of two inputs"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Samples"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_AlgorithmAddition; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Samples::CAlgorithmAddition(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				rAlgorithmPrototype.addInputParameter (OpenViBE::CIdentifier(0,1), "First addition operand",  OpenViBE::Kernel::ParameterType_Integer);
				rAlgorithmPrototype.addInputParameter (OpenViBE::CIdentifier(0,2), "Second addition operand", OpenViBE::Kernel::ParameterType_Integer);
				rAlgorithmPrototype.addOutputParameter(OpenViBE::CIdentifier(0,3), "Addition result",         OpenViBE::Kernel::ParameterType_Integer);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_AlgorithmAdditionDesc);
		};
	};
};

#endif // __SamplePlugin_CAlgorithmAddition_H__
