#include "ovasCConfigurationTMSIRefa32B.h"

#if defined OVAS_OS_Windows

#include <windows.h>
#include <iostream>

#define boolean OpenViBE::boolean

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEAcquisitionServer;

GtkListStore        *m_pListStoreSlaves;
GtkTreeView         *m_pViewSlaves;
GtkListStore        *m_pListStoreList;
GtkTreeView         *m_pViewList;

static void button_remove_slave_device(::GtkButton* pButton, void* pUserData)
{
#if defined _DEBUG_Callbacks_
	cout << "button_remove_slave_device" << endl;
#endif
	static_cast<CConfigurationTMSIRefa32B*>(pUserData)->buttonRemoveSlaveDevice();
}

static void button_add_slave_device(::GtkButton* pButton, void* pUserData)
{
#if defined _DEBUG_Callbacks_
	cout << "button_add_slave_device" << endl;
#endif
	static_cast<CConfigurationTMSIRefa32B*>(pUserData)->buttonAddSlaveDevice();
}

CConfigurationTMSIRefa32B::CConfigurationTMSIRefa32B(const char* sGtkBuilderFileName)
	:CConfigurationBuilder(sGtkBuilderFileName)
{
}

boolean CConfigurationTMSIRefa32B::setDeviceList(const std::vector <std::string> deviceList,std::string *deviceMaster,std::vector<std::string> *deviceSlaves )
{
	m_vDeviceList=deviceList;
	m_sDeviceMaster=deviceMaster;
	m_vDeviceSlaves=deviceSlaves;
	m_vDeviceSlavesTemp.clear();
	for(uint32 i=0;i<(*m_vDeviceSlaves).size();i++)
	{
		m_vDeviceSlavesTemp.push_back((*m_vDeviceSlaves)[i]);
	}

	return true;
}

boolean CConfigurationTMSIRefa32B::preConfigure(void)
{
	if(!CConfigurationBuilder::preConfigure())
	{
		return false;
	}
	::GtkComboBox* l_pComboBox=GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_device"));

	int l_iCount=-1;
	boolean l_bSelected=false;

	// autodetection of the connected device
	::GtkListStore* l_pListStoreComboBox= GTK_LIST_STORE(gtk_combo_box_get_model(l_pComboBox));

	for(uint32 i=0; i<m_vDeviceList.size(); i++)
	{
		GtkTreeIter    iter;
		gtk_list_store_append(l_pListStoreComboBox,&iter);
		gtk_list_store_set (l_pListStoreComboBox, &iter,0, m_vDeviceList[i].c_str(),-1);
		//::gtk_combo_box_append_text(l_pComboBox,m_vDeviceList[i].c_str());

		if(m_sDeviceMaster!=NULL&&m_vDeviceList[i].compare(*m_sDeviceMaster)==0)
		{
			l_iCount=i;
			l_bSelected=true;
		}

	}
	gtk_combo_box_set_model(l_pComboBox,GTK_TREE_MODEL(l_pListStoreComboBox));
	if(l_bSelected)
	{
		::gtk_combo_box_set_active(l_pComboBox, l_iCount);
	}
	else if(!l_bSelected && m_vDeviceList.size()>0)
	{
		::gtk_combo_box_set_active(l_pComboBox, 0);
	}

	//devices Slaves
	GtkTreeViewColumn   *colSlaves;
	GtkCellRenderer     *rendererSlaves;

	m_pViewSlaves = GTK_TREE_VIEW(gtk_builder_get_object(m_pBuilderConfigureInterface, "Devices SlavesTree"));

	rendererSlaves = gtk_cell_renderer_text_new();

	colSlaves= gtk_tree_view_column_new_with_attributes("Devices Slaves",
		  rendererSlaves, "text", 0, NULL);
	gtk_tree_view_append_column(m_pViewSlaves, colSlaves);

	m_pListStoreSlaves = gtk_list_store_new(1, G_TYPE_STRING);

	gtk_tree_view_set_model(m_pViewSlaves, GTK_TREE_MODEL(m_pListStoreSlaves));

	g_object_unref(m_pListStoreSlaves); /* destroy model with view */

	gtk_tree_selection_set_mode(gtk_tree_view_get_selection(m_pViewSlaves),GTK_SELECTION_SINGLE);

	//device List
	GtkTreeViewColumn   *colList;
	GtkCellRenderer     *rendererList;

	m_pViewList = GTK_TREE_VIEW(gtk_builder_get_object(m_pBuilderConfigureInterface, "Device ListTree"));

	rendererList = gtk_cell_renderer_text_new();

	colList = gtk_tree_view_column_new_with_attributes("Devices List", rendererList, "text", 0, NULL);
	gtk_tree_view_append_column(m_pViewList, colList);

	m_pListStoreList = gtk_list_store_new(1, G_TYPE_STRING);

	gtk_tree_view_set_model(m_pViewList, GTK_TREE_MODEL(m_pListStoreList));

	g_object_unref(m_pListStoreList); /* destroy model with view */

	gtk_tree_selection_set_mode(gtk_tree_view_get_selection(m_pViewList), GTK_SELECTION_SINGLE);

	//init list

	for(uint32 i=0;i<m_vDeviceList.size();i++)
	{
		bool find=false;
		uint32 j=0;
		while(!find&&j<(*m_vDeviceSlaves).size())
		{
			find=(*m_vDeviceSlaves)[j].compare(m_vDeviceList[i])==0;
			j++;
		}
		if(find)
		{
			GtkTreeIter    iter;
			gtk_list_store_append(m_pListStoreSlaves,&iter);
			gtk_list_store_set (m_pListStoreSlaves, &iter,0, m_vDeviceList[i].c_str(),-1);
		}
		else
		{
			GtkTreeIter    iter;
			gtk_list_store_append(m_pListStoreList, &iter);
			gtk_list_store_set (m_pListStoreList, &iter,0, m_vDeviceList[i].c_str(),-1);
		}
	}

	// Connects custom GTK signals
	g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface, "button_add_slave_device"),   "pressed", G_CALLBACK(button_add_slave_device),   this);
	g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface, "button_remove_slave_device"),  "pressed", G_CALLBACK(button_remove_slave_device),  this);
	gtk_builder_connect_signals(m_pBuilderConfigureInterface, NULL);
	return true;
}

boolean CConfigurationTMSIRefa32B::postConfigure(void)
{
	::GtkComboBox* l_pComboBox=GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_device"));

	if(m_bApplyConfiguration)
	{
		int l_iUSBIndex=0;
		char* l_sMaster=gtk_combo_box_get_active_text(l_pComboBox);
		*m_sDeviceMaster=(l_sMaster!=NULL)?l_sMaster:"";
		(*m_vDeviceSlaves).clear();
		*m_vDeviceSlaves=m_vDeviceSlavesTemp;
	}

	if(!CConfigurationBuilder::postConfigure())
	{
		return false;
	}
	return true;
}

void CConfigurationTMSIRefa32B::buttonAddSlaveDevice(void)
{
	::GtkTreeIter itList;
	::GtkTreeIter iter;

	::GtkTreeSelection* l_pListTreeViewSelection=gtk_tree_view_get_selection(m_pViewList);

	if(gtk_tree_selection_get_selected(l_pListTreeViewSelection, NULL, &itList))
	{
		char* l_sListDevice=NULL;
		gtk_tree_model_get(GTK_TREE_MODEL(m_pListStoreList), &itList, 0, &l_sListDevice, -1);

		bool find=false;
		for(uint32 i=0;!find&&i<m_vDeviceList.size();i++)
		{
			if(m_vDeviceList[i].compare(l_sListDevice)==0)
			{
				m_vDeviceSlavesTemp.push_back(m_vDeviceList[i]);
				gtk_list_store_remove(m_pListStoreList,&itList);
				gtk_list_store_append(m_pListStoreSlaves,&iter);
				gtk_list_store_set (m_pListStoreSlaves, &iter,0, m_vDeviceList[i].c_str(),-1);
				find=true;
			}
		}
	}
}

void CConfigurationTMSIRefa32B::buttonRemoveSlaveDevice(void)
{
	::GtkTreeIter itSlave;
	::GtkTreeIter iter;

	::GtkTreeSelection* l_pSlaveTreeViewSelection=gtk_tree_view_get_selection(m_pViewSlaves);

	if(gtk_tree_selection_get_selected(l_pSlaveTreeViewSelection, NULL, &itSlave))
	{
		gchar* l_sSlaveDevice=NULL;
		gtk_tree_model_get(GTK_TREE_MODEL(m_pListStoreSlaves), &itSlave, 0, &l_sSlaveDevice, -1);
		gtk_list_store_append(m_pListStoreList,&iter);
		gtk_list_store_set (m_pListStoreList, &iter,0, l_sSlaveDevice,-1);
		gtk_list_store_remove(m_pListStoreSlaves,&itSlave);
		int pos=-1;
		for(uint32 i=0;i<m_vDeviceSlavesTemp.size()&&pos==-1;i++)
		{
			pos=(m_vDeviceSlavesTemp[i].compare(l_sSlaveDevice)==0)?i:-1;
		}
		m_vDeviceSlavesTemp.erase(m_vDeviceSlavesTemp.begin()+pos);
	}
}

#endif // OVAS_OS_Windows
