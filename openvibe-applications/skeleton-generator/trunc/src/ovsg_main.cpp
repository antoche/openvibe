#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <iostream>

#include "ovsgCDriverSkeletonGenerator.h"
#include "ovsgCBoxAlgorithmSkeletonGenerator.h"

#if defined TARGET_OS_Windows
 #define OVSG_OS_Windows
#elif defined TARGET_OS_Linux
 #define OVSG_OS_Linux
#elif defined TARGET_OS_MacOS
 #define OVSG_OS_MacOS
#else
 #warning "No target operating system defined !"
#endif

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBESkeletonGenerator;

using namespace std;

int main(int argc, char** argv)
{

	CKernelLoader l_oKernelLoader;

	cout<<"[  INF  ] Created kernel loader, trying to load kernel module"<<endl;
	CString m_sError;
#if defined OVSG_OS_Windows
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
			l_pKernelContext=l_pKernelDesc->createKernel("skeleton-generator", "../share/openvibe.conf");
			if(!l_pKernelContext)
			{
				cout<<"[ FAILED ] No kernel created by kernel descriptor"<<endl;
			}
			else
			{
				OpenViBEToolkit::initialize(*l_pKernelContext);
				IConfigurationManager& l_rConfigurationManager=l_pKernelContext->getConfigurationManager();
				l_pKernelContext->getPluginManager().addPluginsFromFiles(l_rConfigurationManager.expand("${Kernel_Plugins}"));
				
				gtk_init(&argc, &argv);

				::GtkBuilder * l_pBuilderInterface = gtk_builder_new(); 
				gtk_builder_add_from_file(l_pBuilderInterface, "../share/openvibe-applications/skeleton-generator/generator-interface.ui", NULL);
				//gtk_builder_connect_signals(l_pBuilderInterface, NULL);

				::GtkWidget * l_pDialog = GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterface, "sg-selection-dialog"));

				gtk_dialog_add_button (GTK_DIALOG (l_pDialog),
					GTK_STOCK_OK,
					GTK_RESPONSE_OK);

				gtk_dialog_add_button (GTK_DIALOG (l_pDialog),
					GTK_STOCK_CANCEL,
					GTK_RESPONSE_CANCEL);

				::GtkWidget * l_pRadioDriver = GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterface, "sg-driver-selection-radio-button"));
				::GtkWidget * l_pRadioAlgo = GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterface, "sg-algo-selection-radio-button"));
				::GtkWidget * l_pRadioBox = GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterface, "sg-box-selection-radio-button"));


				CBoxAlgorithmSkeletonGenerator l_BoxGenerator(*l_pKernelContext,l_pBuilderInterface);
				CDriverSkeletonGenerator l_DriverGenerator(*l_pKernelContext,l_pBuilderInterface);

				gint resp = gtk_dialog_run(GTK_DIALOG(l_pDialog));
				
				if(resp== GTK_RESPONSE_OK)
				{
					gtk_widget_hide(GTK_WIDGET(l_pDialog));

					if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pRadioDriver)))
					{
						l_DriverGenerator.initialize();
					}
					if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pRadioAlgo)))
					{
						std::cout<< "NOT YET AVAILABLE." <<std::endl;
						return 0;
					}
					if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pRadioBox)))
					{
						l_BoxGenerator.initialize();
					}
					gtk_main();
				}
				else
				{
					std::cout<< "User cancelled. Exit." <<std::endl;
					return 0 ;
				}
			}
		}
	}

	l_oKernelLoader.uninitialize();
	l_oKernelLoader.unload();
	return 0;
}
