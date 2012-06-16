#include "ovassvep_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include "ovassvepCApplication.h"
#include "Trainer/ovassvepCTrainerApplication.h"
#include "Shooter/ovassvepCShooterApplication.h"

using namespace OpenViBE;

/**
 */
int main(int argc, char** argv)
{
	
	if (argc != 2)
	{
		printf("Usage : %s <configuration-file>\n", argv[0]);
		exit(1);
	}
	
	// initialize the OpenViBE kernel
	
	OpenViBE::CKernelLoader l_oKernelLoader;
	OpenViBE::CString l_sError;
	OpenViBE::Kernel::IKernelDesc* l_poKernelDesc = NULL;
	OpenViBE::Kernel::IKernelContext* l_poKernelContext = NULL;
	OpenViBE::Kernel::ILogManager* l_poLogManager = NULL;
	OpenViBE::Kernel::IConfigurationManager* l_poConfigurationManager = NULL;


#ifdef OVA_OS_Windows
	std::cout << "[  INF  ] Loading Windows kernel\n";
	if(!l_oKernelLoader.load("../bin/OpenViBE-kernel-dynamic.dll", &l_sError))
#else
	std::cout << "[  INF  ] Loading Linux kernel\n";
	if(!l_oKernelLoader.load("../lib/libOpenViBE-kernel-dynamic.so", &l_sError))
#endif
	{
		std::cout << "[ FAILED ] Error loading kernel (" << l_sError << ")" << "\n";
	}
	else
	{
		std::cout<< "[  INF  ] Kernel module loaded, trying to get kernel descriptor\n";

		l_oKernelLoader.initialize();
		l_oKernelLoader.getKernelDesc(l_poKernelDesc);

		if(!l_poKernelDesc)
		{
			std::cout << "[ FAILED ] No kernel descriptor\n";
		}
		else
		{
			std::cout << "[  INF  ] Got kernel descriptor, trying to create kernel\n";

			l_poKernelContext = l_poKernelDesc->createKernel("ssvep-demo", "../share/openvibe.conf");

			if(!l_poKernelContext)
			{
				std::cout << "[ FAILED ] No kernel created by kernel descriptor\n";
			}
			else
			{
				OpenViBEToolkit::initialize(*l_poKernelContext);

				l_poConfigurationManager = &(l_poKernelContext->getConfigurationManager());
				l_poConfigurationManager->createConfigurationToken("SSVEP_ApplicationDescriptor", CString(argv[1]));
				l_poConfigurationManager->addConfigurationFromFile(l_poConfigurationManager->expand("../share/openvibe-ssvep-demo.conf"));

				l_poLogManager = &(l_poKernelContext->getLogManager());
			}
		}
	}




	OpenViBESSVEP::CApplication* app = NULL;

	CString l_sApplicationType = l_poConfigurationManager->expand("${SSVEP_ApplicationType}");


	(*l_poLogManager) << OpenViBE::Kernel::LogLevel_Info << "Selected Application : " << l_sApplicationType.toASCIIString() << "\n";


	if (l_sApplicationType == CString("trainer"))
	{
		(*l_poLogManager) << OpenViBE::Kernel::LogLevel_Debug << "+ app = new OpenViBESSVEP::CTrainerApplication(...)\n";
		app = new OpenViBESSVEP::CTrainerApplication();
	}
	else if (l_sApplicationType == CString("shooter"))
	{
		(*l_poLogManager) << OpenViBE::Kernel::LogLevel_Debug << "+ app = new OpenViBESSVEP::CShooterApplication(...)\n";
		app = new OpenViBESSVEP::CShooterApplication();
	}
	else
	{
		(*l_poLogManager) << OpenViBE::Kernel::LogLevel_Error << "Wrong application identifier specified\n";

		return 1;
	}

	app->setup(l_poKernelContext);
	app->go();

	(*l_poLogManager) << OpenViBE::Kernel::LogLevel_Debug << "- app\n";
	delete app;

	return 0;
}



