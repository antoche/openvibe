#include "kernel/ovkCKernel.h"

#include <openvibe/ov_all.h>

#include <OMKSimpleOpenMASK3Loader.h>
#include <OMKObjectDescriptor.h>
#include <OMKController.h>
#include <OMKPluginsLoader.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

namespace OpenViBE
{
	namespace Kernel
	{
		class CKernelDesc : public IKernelDesc
		{
		public:

			virtual void release(void) { }

			virtual IKernel* create(void)
			{
				return new CKernel();
			}

			virtual CString getName(void)                { return CString("OpenViBE Kernel Implementation [Based on OpenMASK]"); }
			virtual CString getAuthorName(void)          { return CString("Yann Renard"); }
			virtual CString getAuthorCompanyName(void)   { return CString("INRIA/IRISA"); }
			virtual CString getShortDescription(void)    { return CString("OpenViBE Kernel Implementation [Based on OpenMASK]"); }
			virtual CString getDetailedDescription(void) { return CString("OpenViBE Kernel Implementation [Based on OpenMASK]"); }
			virtual CString getVersion(void)             { return CString("0.3"); }

			_IsDerivedFromClass_Final_(IKernelDesc, OVK_ClassId_KernelDesc);
		};
	};
};

static CKernelDesc gst_oKernelDesc;

#include "system/Time.h"

namespace
{
	OMK::SimpleOpenMASK3Loader* g_pLoader=NULL;
	OMK::ObjectDescriptor* g_pSimulation=NULL;
	OMK::Controller* g_pController=NULL;
	OMK::ReferenceObjectHandle* g_pControllerHandle=NULL;
}

extern "C"
{

OVK_API boolean onInitialize(void)
{
	g_pLoader=new OMK::SimpleOpenMASK3Loader("../share/openvibe-kernel-omk/cfgnovis.OpenMASK3");
	g_pSimulation=g_pLoader->getRootObjectDescriptor();
	g_pController=new OMK::Controller(*g_pSimulation, 0);
	OMK::PluginsLoader::getInstance().init(*g_pController);

	g_pControllerHandle=dynamic_cast< OMK::ReferenceObjectHandle*>(g_pController->getObjectHandle());
	g_pController->init();
	g_pController->runControllersStep(g_pControllerHandle);

	return true;
}

OVK_API boolean onGetKernelDesc(IKernelDesc*& rpKernelDesc)
{
	rpKernelDesc=&gst_oKernelDesc;
	return true;
}

OVK_API boolean onUninitialize(void)
{
	g_pController->finish();
	g_pController->runControllersStep(g_pControllerHandle);

	delete g_pController;
	delete g_pSimulation;
	delete g_pLoader;

	return true;
}

};
