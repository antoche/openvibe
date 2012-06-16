#include "kernel/ovkCKernelContext.h"

#include <openvibe/ov_all.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

namespace OpenViBE
{
	namespace Kernel
	{
		class CKernelDesc : public IKernelDesc
		{
		public:

			virtual IKernelContext* createKernel(const CString& rApplicationName, const CString& rConfigurationFilename)
			{
				return new CKernelContext(NULL, rApplicationName, rConfigurationFilename);
			}

			virtual IKernelContext* createKernel(const IKernelContext& rMasterKernelContext, const CString& rApplicationName, const CString& rConfigurationFilename)
			{
				return new CKernelContext(&rMasterKernelContext, rApplicationName, rConfigurationFilename);
			}

			virtual void releaseKernel(IKernelContext* pKernelContext)
			{
				delete pKernelContext;
			}

			virtual CString getName(void)                { return CString("OpenViBE Kernel Implementation"); }
			virtual CString getAuthorName(void)          { return CString("Yann Renard"); }
			virtual CString getAuthorCompanyName(void)   { return CString("INRIA/IRISA"); }
			virtual CString getShortDescription(void)    { return CString("OpenViBE Kernel Implementation"); }
			virtual CString getDetailedDescription(void) { return CString("OpenViBE Kernel Implementation"); }
			virtual CString getVersion(void)             { return CString("0.5"); }

			_IsDerivedFromClass_Final_(IKernelDesc, OVK_ClassId_KernelDesc);
		};
	};
};

static CKernelDesc gst_oKernelDesc;

#include "system/Time.h"

extern "C"
{

OVK_API boolean onInitialize(void)
{
	return true;
}

OVK_API boolean onGetKernelDesc(IKernelDesc*& rpKernelDesc)
{
	rpKernelDesc=&gst_oKernelDesc;
	return true;
}

OVK_API boolean onUninitialize(void)
{
	return true;
}

};
