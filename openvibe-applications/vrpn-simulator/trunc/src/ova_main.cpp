#include <gtk/gtk.h>

#include <gdk/gdk.h>

#include <vrpn_Button.h>
#include <vrpn_Analog.h>
#include <vrpn_Connection.h>

#include <iostream>

#define _vrpn_peripheral_name_ "openvibe-vrpn@localhost"

// #define _DEBUG

::vrpn_Connection* g_pConnection=NULL;
::vrpn_Button_Server* g_pButtonServer=NULL;
::vrpn_Analog_Server* g_pAnalogServer=NULL;
int g_iAnalogCount=0;
int g_iButtonCount=0;

typedef union
{
	gpointer pUserData;
	int iData;
} TUserData;

void fScrollCB(::GtkRange* pRange, gpointer pUserData)
{
	TUserData  l_oUserData;
	l_oUserData.pUserData=pUserData;
	g_pAnalogServer->channels()[l_oUserData.iData]=gtk_range_get_value(pRange);

#if defined _DEBUG
	std::cout << (int)(pUserData) << " value changed\n";
#endif
}

void fSwitchCB(::GtkToggleButton* pTogglebutton, gpointer pUserData)
{
	TUserData  l_oUserData;
	l_oUserData.pUserData=pUserData;
	g_pButtonServer->set_button(l_oUserData.iData, gtk_toggle_button_get_active(pTogglebutton));

#if defined _DEBUG
	std::cout << (int)(pUserData) << " toggled\n";
#endif
}

void fConnectCB(::GtkWidget* pWidget, gpointer data)
{
	if(GTK_IS_RANGE(pWidget))
	{
		g_signal_connect(G_OBJECT(pWidget), "value-changed", G_CALLBACK(fScrollCB), (void*)g_iAnalogCount);
		g_iAnalogCount++;
	}

	if(GTK_IS_TOGGLE_BUTTON(pWidget))
	{
		g_signal_connect(G_OBJECT(pWidget), "toggled", G_CALLBACK(fSwitchCB), (void*)g_iButtonCount);
		g_iButtonCount++;
	}
}

gboolean fIdleApplicationLoop(gpointer pUserData)
{
	g_pButtonServer->mainloop();
	g_pAnalogServer->report_changes();
	g_pAnalogServer->mainloop();
	g_pConnection->mainloop();
	return TRUE;
}

int main(int argc, char ** argv)
{
	gtk_init(&argc, &argv);
	// g_pConnection=new ::vrpn_Connection;
	g_pConnection=vrpn_create_server_connection();
	g_pButtonServer=new ::vrpn_Button_Server(_vrpn_peripheral_name_, g_pConnection, 10);
	g_pAnalogServer=new ::vrpn_Analog_Server(_vrpn_peripheral_name_, g_pConnection);
	g_pAnalogServer->setNumChannels(10);

	::GtkBuilder* l_pInterface=gtk_builder_new(); // glade_xml_new("../share/openvibe-applications/vrpn-simulator/interface.ui", "window", NULL);
	gtk_builder_add_from_file(l_pInterface, "../share/openvibe-applications/vrpn-simulator/interface.ui", NULL);

	::GtkWidget* l_pMainWindow=GTK_WIDGET(gtk_builder_get_object(l_pInterface, "window"));
	::GtkWidget* l_pHBoxButton=GTK_WIDGET(gtk_builder_get_object(l_pInterface, "hbox_button"));
	::GtkWidget* l_pHBoxAnalog=GTK_WIDGET(gtk_builder_get_object(l_pInterface, "hbox_analog"));

	g_signal_connect(G_OBJECT(l_pMainWindow), "destroy", gtk_main_quit, NULL);
	gtk_container_foreach(GTK_CONTAINER(l_pHBoxButton), fConnectCB, NULL);
	gtk_container_foreach(GTK_CONTAINER(l_pHBoxAnalog), fConnectCB, NULL);
	gtk_builder_connect_signals(l_pInterface, NULL);

	std::cout << "got " << g_iAnalogCount << " analogs...\n";
	std::cout << "got " << g_iButtonCount << " buttons...\n";

	g_idle_add(fIdleApplicationLoop, NULL);

	gtk_widget_show(l_pMainWindow);
	gtk_main();

	delete g_pAnalogServer;
	delete g_pButtonServer;
	delete g_pConnection;

	return 0;
}
