#ifndef __OpenViBEPlugins_Samples_CCrashingBox_H__
#define __OpenViBEPlugins_Samples_CCrashingBox_H__

#include "ovp_defines.h"

#include <openvibe/ov_all.h>
#include <ebml/IReader.h>
#include <ebml/IReaderHelper.h>
#include <iostream>
#include <vector>
#include <cmath>

namespace OpenViBEPlugins
{
	namespace Samples
	{
		class CCrashingBox : public OpenViBE::Plugins::IBoxAlgorithm
		{
		public:

			virtual void release(void)
			{
				delete this;
			}

			virtual OpenViBE::boolean initialize(
				OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext)
			{
				throw 0;
				return true;
			}

			virtual OpenViBE::boolean uninitialize(
				OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext)
			{
				int one=(int)1.0;
				int zero=(int)sin(0.0);
				int division_by_zero=one/zero;
				return division_by_zero?true:false;
			}

			virtual OpenViBE::boolean processInput(
				OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext,
				OpenViBE::uint32 ui32InputIndex)
			{
				rBoxAlgorithmContext.markAlgorithmAsReadyToProcess();
				return true;
			}

			virtual OpenViBE::boolean process(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext)
			{
				*((int*)NULL)=0;

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_CrashingBox)
		};

		class CCrashingBoxDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Crashing box"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("A box which code launches exceptions"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("This box illustrates the behavior of the platform given a crashing plugin code"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Samples"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_CrashingBox; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Samples::CCrashingBox(); }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput("an input", OV_UndefinedIdentifier);

				return true;
			}

			virtual OpenViBE::CString getStockItemName(void) const
			{
				return OpenViBE::CString("gtk-cancel");
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_CrashingBoxDesc)
		};
	};
};

#endif // __OpenViBEPlugins_Samples_CCrashingBox_H__
