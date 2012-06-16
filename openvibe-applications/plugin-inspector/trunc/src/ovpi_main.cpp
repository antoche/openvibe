#include "ovpiCPluginObjectDescEnumAlgorithmGlobalDefinesGenerator.h"
#include "ovpiCPluginObjectDescEnumBoxAlgorithmSnapshotGenerator.h"
#include "ovpiCPluginObjectDescEnumAlgorithmSnapshotGenerator.h"

#include <iostream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace std;

int main(int argc, char ** argv)
{
//___________________________________________________________________//
//                                                                   //


	CKernelLoader l_oKernelLoader;

	cout<<"[  INF  ] Created kernel loader, trying to load kernel module"<<endl;
	CString m_sError;
#if defined OVPI_OS_Windows
	if(!l_oKernelLoader.load("../bin/OpenViBE-kernel-dynamic.dll", &m_sError))
#else
	if(!l_oKernelLoader.load("../lib/libOpenViBE-kernel-dynamic.so", &m_sError))
#endif
	{
			cout<<"[ FAILED ] Error loading kernel ("<<m_sError<<")"<<endl;
	}
	else
	{
		cout<<"[  INF  ] Kernel module loaded, trying to get kernel descriptor"<<endl;
		IKernelDesc* l_pKernelDesc=NULL;
		IKernelContext* l_pKernelContext=NULL;
		l_oKernelLoader.initialize();
		l_oKernelLoader.getKernelDesc(l_pKernelDesc);
		if(!l_pKernelDesc)
		{
			cout<<"[ FAILED ] No kernel descriptor"<<endl;
		}
		else
		{
			cout<<"[  INF  ] Got kernel descriptor, trying to create kernel"<<endl;
			l_pKernelContext=l_pKernelDesc->createKernel("plugin-inspector", "../share/openvibe.conf");
			if(!l_pKernelContext)
			{
				cout<<"[ FAILED ] No kernel created by kernel descriptor"<<endl;
			}
			else
			{
				OpenViBEToolkit::initialize(*l_pKernelContext);

// For Mister Vincent !
#ifdef OVAS_OS_Windows
#ifndef NDEBUG
				//_asm int 3;
#endif
#endif

				IConfigurationManager& l_rConfigurationManager=l_pKernelContext->getConfigurationManager();

				l_pKernelContext->getPluginManager().addPluginsFromFiles(l_rConfigurationManager.expand("${Kernel_Plugins}"));

				//initialise Gtk before 3D context
				gtk_init(&argc, &argv);
				// gtk_rc_parse("../share/openvibe-applications/designer/interface.gtkrc");

				if(l_rConfigurationManager.expandAsBoolean("${Kernel_3DVisualisationEnabled}"))
				{
					l_pKernelContext->getVisualisationManager().initialize3DContext();
				}

				CString l_sGlobalDefinesDirectory          =l_rConfigurationManager.expand("${PluginInspector_DumpGlobalDefinesDirectory}");
				CString l_sAlgorithmSnapshotDirectory      =l_rConfigurationManager.expand("${PluginInspector_DumpAlgorithmSnapshotDirectory}");
				CString l_sAlgorithmDocTemplateDirectory   =l_rConfigurationManager.expand("${PluginInspector_DumpAlgorithmDocTemplateDirectory}");
				CString l_sBoxAlgorithmSnapshotDirectory   =l_rConfigurationManager.expand("${PluginInspector_DumpBoxAlgorithmSnapshotDirectory}");
				CString l_sBoxAlgorithmDocTemplateDirectory=l_rConfigurationManager.expand("${PluginInspector_DumpBoxAlgorithmDocTemplateDirectory}");

				if(l_sGlobalDefinesDirectory!=CString(""))
				{
					CPluginObjectDescEnumAlgorithmGlobalDefinesGenerator l_oGlobalDefinesGenerator(*l_pKernelContext, l_sGlobalDefinesDirectory);
					l_oGlobalDefinesGenerator.enumeratePluginObjectDesc(OV_ClassId_Plugins_AlgorithmDesc);
				}

				if(l_sAlgorithmSnapshotDirectory!=CString("") && l_sAlgorithmDocTemplateDirectory!=CString(""))
				{
					CPluginObjectDescEnumAlgorithmSnapshotGenerator l_oAlgorithmSnapshotGenerator(*l_pKernelContext, l_sAlgorithmSnapshotDirectory, l_sAlgorithmDocTemplateDirectory);
					l_oAlgorithmSnapshotGenerator.enumeratePluginObjectDesc(OV_ClassId_Plugins_AlgorithmDesc);
				}

				if(l_sBoxAlgorithmSnapshotDirectory!=CString("") && l_sBoxAlgorithmDocTemplateDirectory!=CString(""))
				{
					CPluginObjectDescEnumBoxAlgorithmSnapshotGenerator l_oBoxAlgorithmSnapshotGenerator(*l_pKernelContext, l_sBoxAlgorithmSnapshotDirectory, l_sBoxAlgorithmDocTemplateDirectory);
					l_oBoxAlgorithmSnapshotGenerator.enumeratePluginObjectDesc(OV_ClassId_Plugins_BoxAlgorithmDesc);
				}

				cout<<"[  INF  ] Application terminated, releasing allocated objects"<<endl;

				OpenViBEToolkit::uninitialize(*l_pKernelContext);

				l_pKernelDesc->releaseKernel(l_pKernelContext);
			}
		}
		l_oKernelLoader.uninitialize();
		l_oKernelLoader.unload();
	}

	return 0;
}
