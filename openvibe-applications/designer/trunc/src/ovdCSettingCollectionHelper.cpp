#include "ovdCSettingCollectionHelper.h"

#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEDesigner;
using namespace std;

#include <iostream>
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

namespace
{
	static void collect_widget_cb(::GtkWidget* pWidget, gpointer pUserData)
	{
		static_cast< vector< ::GtkWidget* > *>(pUserData)->push_back(pWidget);
	}

	static void remove_widget_cb(::GtkWidget* pWidget, gpointer pUserData)
	{
		gtk_container_remove(GTK_CONTAINER(pUserData), pWidget);
	}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

	static void on_entry_setting_boolean_edited(::GtkEntry* pEntry, gpointer pUserData)
	{
		vector< ::GtkWidget* > l_vWidget;
		gtk_container_foreach(GTK_CONTAINER(gtk_widget_get_parent(GTK_WIDGET(pEntry))), collect_widget_cb, &l_vWidget);
		::GtkToggleButton* l_pWidget=GTK_TOGGLE_BUTTON(l_vWidget[0]);

		std::string l_sEntryValue=::gtk_entry_get_text(pEntry);
		if(l_sEntryValue=="true")
		{
			::gtk_toggle_button_set_active(l_pWidget, true);
			::gtk_toggle_button_set_inconsistent(l_pWidget, false);
		}
		else if (l_sEntryValue=="false")
		{
			::gtk_toggle_button_set_active(l_pWidget, false);
			::gtk_toggle_button_set_inconsistent(l_pWidget, false);
		}
		else
		{
			::gtk_toggle_button_set_inconsistent(l_pWidget, true);
		}
	}

	static void on_checkbutton_setting_boolean_pressed(::GtkToggleButton* pButton, gpointer pUserData)
	{
		vector< ::GtkWidget* > l_vWidget;
		gtk_container_foreach(GTK_CONTAINER(gtk_widget_get_parent(GTK_WIDGET(pButton))), collect_widget_cb, &l_vWidget);
		::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[1]);

		if(::gtk_toggle_button_get_active(pButton))
		{
			::gtk_entry_set_text(l_pWidget, "true");
		}
		else
		{
			::gtk_entry_set_text(l_pWidget, "false");
		}
		::gtk_toggle_button_set_inconsistent(pButton, false);
	}

	static void on_button_setting_integer_pressed(::GtkButton* pButton, gpointer pUserData, gint iOffset)
	{
		const IKernelContext& l_rKernelContext=static_cast < CSettingCollectionHelper* >(pUserData)->m_rKernelContext;

		vector< ::GtkWidget* > l_vWidget;
		gtk_container_foreach(GTK_CONTAINER(gtk_widget_get_parent(GTK_WIDGET(pButton))), collect_widget_cb, &l_vWidget);
		::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);

		char l_sValue[1024];
		int64 l_i64lValue=l_rKernelContext.getConfigurationManager().expandAsInteger(gtk_entry_get_text(l_pWidget), 0);
		l_i64lValue+=iOffset;
		::sprintf(l_sValue, "%lli", l_i64lValue);
		gtk_entry_set_text(l_pWidget, l_sValue);
	}

	static void on_button_setting_integer_up_pressed(::GtkButton* pButton, gpointer pUserData)
	{
		on_button_setting_integer_pressed(pButton, pUserData, 1);
	}

	static void on_button_setting_integer_down_pressed(::GtkButton* pButton, gpointer pUserData)
	{
		on_button_setting_integer_pressed(pButton, pUserData, -1);
	}

	static void on_button_setting_float_pressed(::GtkButton* pButton, gpointer pUserData, gdouble dOffset)
	{
		const IKernelContext& l_rKernelContext=static_cast < CSettingCollectionHelper* >(pUserData)->m_rKernelContext;

		vector< ::GtkWidget* > l_vWidget;
		gtk_container_foreach(GTK_CONTAINER(gtk_widget_get_parent(GTK_WIDGET(pButton))), collect_widget_cb, &l_vWidget);
		::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);

		char l_sValue[1024];
		float64 l_f64lValue=l_rKernelContext.getConfigurationManager().expandAsFloat(gtk_entry_get_text(l_pWidget), 0);
		l_f64lValue+=dOffset;
		::sprintf(l_sValue, "%lf", l_f64lValue);
		gtk_entry_set_text(l_pWidget, l_sValue);
	}

	static void on_button_setting_float_up_pressed(::GtkButton* pButton, gpointer pUserData)
	{
		on_button_setting_float_pressed(pButton, pUserData, 1);
	}

	static void on_button_setting_float_down_pressed(::GtkButton* pButton, gpointer pUserData)
	{
		on_button_setting_float_pressed(pButton, pUserData, -1);
	}

	static void on_button_setting_filename_browse_pressed(::GtkButton* pButton, gpointer pUserData)
	{
		const IKernelContext& l_rKernelContext=static_cast < CSettingCollectionHelper* >(pUserData)->m_rKernelContext;

		vector< ::GtkWidget* > l_vWidget;
		gtk_container_foreach(GTK_CONTAINER(gtk_widget_get_parent(GTK_WIDGET(pButton))), collect_widget_cb, &l_vWidget);
		::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);

		::GtkWidget* l_pWidgetDialogOpen=gtk_file_chooser_dialog_new(
			"Select file to open...",
			NULL,
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

		CString l_sInitialFileName=l_rKernelContext.getConfigurationManager().expand(gtk_entry_get_text(l_pWidget));
		if(g_path_is_absolute(l_sInitialFileName.toASCIIString()))
		{
			gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen), l_sInitialFileName.toASCIIString());
		}
		else
		{
			char* l_sFullPath=g_build_filename(g_get_current_dir(), l_sInitialFileName.toASCIIString(), NULL);
			gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen), l_sFullPath);
			g_free(l_sFullPath);
		}

		gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(l_pWidgetDialogOpen), false);

		if(gtk_dialog_run(GTK_DIALOG(l_pWidgetDialogOpen))==GTK_RESPONSE_ACCEPT)
		{
			char* l_sFileName=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen));
			char* l_pBackslash = NULL;
			while((l_pBackslash = ::strchr(l_sFileName, '\\'))!=NULL)
			{
				*l_pBackslash = '/';
			}
			gtk_entry_set_text(l_pWidget, l_sFileName);
			g_free(l_sFileName);
		}
		gtk_widget_destroy(l_pWidgetDialogOpen);
	}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

	static void on_button_setting_script_edit_pressed(::GtkButton* pButton, gpointer pUserData)
	{
		const IKernelContext& l_rKernelContext=static_cast < CSettingCollectionHelper* >(pUserData)->m_rKernelContext;

		vector< ::GtkWidget* > l_vWidget;
		gtk_container_foreach(GTK_CONTAINER(gtk_widget_get_parent(GTK_WIDGET(pButton))), collect_widget_cb, &l_vWidget);
		::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);

		CString l_sFileName=l_rKernelContext.getConfigurationManager().expand(gtk_entry_get_text(l_pWidget));
		CString l_sEditorCommand=l_rKernelContext.getConfigurationManager().expand("${Designer_ScriptEditorCommand}");

		if(l_sEditorCommand != CString(""))
		{
			CString l_sFullCommand=l_sEditorCommand + CString(" \"") + l_sFileName + CString("\"");
#if defined OVD_OS_Windows
			l_sFullCommand = "START " + l_sFullCommand;
#elif defined OVD_OS_Linux
			l_sFullCommand = l_sFullCommand + " &";
#else
#endif
			if(::system(l_sFullCommand.toASCIIString())<0)
			{
				l_rKernelContext.getLogManager() << LogLevel_Warning << "Could not run command " << l_sFullCommand << "\n";
			}
		}
	}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

	static void on_button_setting_color_choose_pressed(::GtkColorButton* pButton, gpointer pUserData)
	{
		vector< ::GtkWidget* > l_vWidget;
		gtk_container_foreach(GTK_CONTAINER(gtk_widget_get_parent(GTK_WIDGET(pButton))), collect_widget_cb, &l_vWidget);
		::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);
		::GdkColor l_oColor;
		gtk_color_button_get_color(pButton, &l_oColor);

		char l_sBuffer[1024];
		sprintf(l_sBuffer, "%i,%i,%i", (l_oColor.red*100)/65535, (l_oColor.green*100)/65535, (l_oColor.blue*100)/65535);

		gtk_entry_set_text(l_pWidget, l_sBuffer);
	}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

	typedef struct
	{
		float64 fPercent;
		::GdkColor oColor;
		::GtkColorButton* pColorButton;
		::GtkSpinButton* pSpinButton;
	} SColorGradientDataNode;

	typedef struct
	{
		string sGUIFilename;
		::GtkWidget* pDialog;
		::GtkWidget* pContainer;
		::GtkWidget* pDrawingArea;
		vector < SColorGradientDataNode > vColorGradient;
		map < ::GtkColorButton*, uint32 > vColorButtonMap;
		map < ::GtkSpinButton*, uint32 > vSpinButtonMap;
	} SColorGradientData;

	static void on_gtk_widget_destroy_cb(::GtkWidget* pWidget, gpointer pUserData)
	{
		gtk_widget_destroy(pWidget);
	}

	static void on_initialize_color_gradient(::GtkWidget* pWidget, gpointer pUserData);

	static void on_refresh_color_gradient(::GtkWidget* pWidget, ::GdkEventExpose* pEvent, gpointer pUserData)
	{
		SColorGradientData* l_pUserData=static_cast<SColorGradientData*>(pUserData);

		uint32 i;
		uint32 ui32Steps=100;
		gint sizex=0;
		gint sizey=0;
		gdk_drawable_get_size(l_pUserData->pDrawingArea->window, &sizex, &sizey);

		CMatrix l_oGradientMatrix;
		l_oGradientMatrix.setDimensionCount(2);
		l_oGradientMatrix.setDimensionSize(0, 4);
		l_oGradientMatrix.setDimensionSize(1, l_pUserData->vColorGradient.size());
		for(i=0; i<l_pUserData->vColorGradient.size(); i++)
		{
			l_oGradientMatrix[i*4  ]=l_pUserData->vColorGradient[i].fPercent;
			l_oGradientMatrix[i*4+1]=l_pUserData->vColorGradient[i].oColor.red  *100./65535.;
			l_oGradientMatrix[i*4+2]=l_pUserData->vColorGradient[i].oColor.green*100./65535.;
			l_oGradientMatrix[i*4+3]=l_pUserData->vColorGradient[i].oColor.blue *100./65535.;
		}

		CMatrix l_oInterpolatedMatrix;
		OpenViBEToolkit::Tools::ColorGradient::interpolate(l_oInterpolatedMatrix, l_oGradientMatrix, ui32Steps);

		::GdkGC* l_pGC=gdk_gc_new(l_pUserData->pDrawingArea->window);
		::GdkColor l_oColor;

		for(i=0; i<ui32Steps; i++)
		{
			l_oColor.red  =(guint)(l_oInterpolatedMatrix[i*4+1]*65535*.01);
			l_oColor.green=(guint)(l_oInterpolatedMatrix[i*4+2]*65535*.01);
			l_oColor.blue =(guint)(l_oInterpolatedMatrix[i*4+3]*65535*.01);
			gdk_gc_set_rgb_fg_color(l_pGC, &l_oColor);
			gdk_draw_rectangle(
				l_pUserData->pDrawingArea->window,
				l_pGC,
				TRUE,
				(sizex*i)/ui32Steps,
				0,
				(sizex*(i+1))/ui32Steps,
				sizey);
		}
		g_object_unref(l_pGC);
	}

	static void on_color_gradient_spin_button_value_changed(::GtkSpinButton* pButton, gpointer pUserData)
	{
		SColorGradientData* l_pUserData=static_cast<SColorGradientData*>(pUserData);

		gtk_spin_button_update(pButton);

		uint32 i=l_pUserData->vSpinButtonMap[pButton];
		::GtkSpinButton* l_pPrevSpinButton=(i>                                   0?l_pUserData->vColorGradient[i-1].pSpinButton:NULL);
		::GtkSpinButton* l_pNextSpinButton=(i<l_pUserData->vColorGradient.size()-1?l_pUserData->vColorGradient[i+1].pSpinButton:NULL);
		if(!l_pPrevSpinButton)
		{
			gtk_spin_button_set_value(pButton, 0);
		}
		if(!l_pNextSpinButton)
		{
			gtk_spin_button_set_value(pButton, 100);
		}
		if(l_pPrevSpinButton && gtk_spin_button_get_value(pButton) < gtk_spin_button_get_value(l_pPrevSpinButton))
		{
			gtk_spin_button_set_value(pButton, gtk_spin_button_get_value(l_pPrevSpinButton));
		}
		if(l_pNextSpinButton && gtk_spin_button_get_value(pButton) > gtk_spin_button_get_value(l_pNextSpinButton))
		{
			gtk_spin_button_set_value(pButton, gtk_spin_button_get_value(l_pNextSpinButton));
		}

		l_pUserData->vColorGradient[i].fPercent=gtk_spin_button_get_value(pButton);

		on_refresh_color_gradient(NULL, NULL, pUserData);
	}

	static void on_color_gradient_color_button_pressed(::GtkColorButton* pButton, gpointer pUserData)
	{
		SColorGradientData* l_pUserData=static_cast<SColorGradientData*>(pUserData);

		::GdkColor l_oColor;
		gtk_color_button_get_color(pButton, &l_oColor);

		l_pUserData->vColorGradient[l_pUserData->vColorButtonMap[pButton]].oColor=l_oColor;

		on_refresh_color_gradient(NULL, NULL, pUserData);
	}

	static void on_initialize_color_gradient(::GtkWidget* pWidget, gpointer pUserData)
	{
		SColorGradientData* l_pUserData=static_cast<SColorGradientData*>(pUserData);

		gtk_widget_hide(l_pUserData->pContainer);

		gtk_container_foreach(GTK_CONTAINER(l_pUserData->pContainer), on_gtk_widget_destroy_cb, NULL);

		vector < SColorGradientDataNode >::iterator it;

		uint32 i=0;
		uint32 count=l_pUserData->vColorGradient.size();
		l_pUserData->vColorButtonMap.clear();
		l_pUserData->vSpinButtonMap.clear();
		for(it=l_pUserData->vColorGradient.begin(); it!=l_pUserData->vColorGradient.end(); it++, i++)
		{
			::GtkBuilder* l_pBuilderInterface=gtk_builder_new(); // glade_xml_new(l_pUserData->sGUIFilename.c_str(), "setting_editor-color_gradient-hbox", NULL);
			gtk_builder_add_from_file(l_pBuilderInterface, l_pUserData->sGUIFilename.c_str(), NULL);
			gtk_builder_connect_signals(l_pBuilderInterface, NULL);

			::GtkWidget* l_pWidget=GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterface, "setting_editor-color_gradient-hbox"));

			it->pColorButton=GTK_COLOR_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "setting_editor-color_gradient-colorbutton"));
			it->pSpinButton=GTK_SPIN_BUTTON(gtk_builder_get_object(l_pBuilderInterface, "setting_editor-color_gradient-spinbutton"));

			gtk_color_button_set_color(it->pColorButton, &it->oColor);
			gtk_spin_button_set_value(it->pSpinButton, it->fPercent);

			g_signal_connect(G_OBJECT(it->pColorButton), "color-set", G_CALLBACK(on_color_gradient_color_button_pressed), l_pUserData);
			g_signal_connect(G_OBJECT(it->pSpinButton), "value-changed", G_CALLBACK(on_color_gradient_spin_button_value_changed), l_pUserData);

			g_object_ref(l_pWidget);
			gtk_container_remove(GTK_CONTAINER(gtk_widget_get_parent(l_pWidget)), l_pWidget);
			gtk_container_add(GTK_CONTAINER(l_pUserData->pContainer), l_pWidget);
			g_object_unref(l_pWidget);

			g_object_unref(l_pBuilderInterface);

			l_pUserData->vColorButtonMap[it->pColorButton]=i;
			l_pUserData->vSpinButtonMap[it->pSpinButton]=i;
		}

		gtk_spin_button_set_value(l_pUserData->vColorGradient[0].pSpinButton, 0);
		gtk_spin_button_set_value(l_pUserData->vColorGradient[count-1].pSpinButton, 100);

		gtk_widget_show(l_pUserData->pContainer);
	}

	static void on_button_color_gradient_add_pressed(::GtkButton* pButton, gpointer pUserData)
	{
		SColorGradientData* l_pUserData=static_cast<SColorGradientData*>(pUserData);
		l_pUserData->vColorGradient.resize(l_pUserData->vColorGradient.size()+1);
		l_pUserData->vColorGradient[l_pUserData->vColorGradient.size()-1].fPercent=100;
		on_initialize_color_gradient(NULL, pUserData);
		on_refresh_color_gradient(NULL, NULL, pUserData);
	}

	static void on_button_color_gradient_remove_pressed(::GtkButton* pButton, gpointer pUserData)
	{
		SColorGradientData* l_pUserData=static_cast<SColorGradientData*>(pUserData);
		if(l_pUserData->vColorGradient.size() > 2)
		{
			l_pUserData->vColorGradient.resize(l_pUserData->vColorGradient.size()-1);
			l_pUserData->vColorGradient[l_pUserData->vColorGradient.size()-1].fPercent=100;
			on_initialize_color_gradient(NULL, pUserData);
			on_refresh_color_gradient(NULL, NULL, pUserData);
		}
	}

	static void on_button_setting_color_gradient_configure_pressed(::GtkButton* pButton, gpointer pUserData)
	{
		SColorGradientData l_oUserData;

		l_oUserData.sGUIFilename=static_cast < CSettingCollectionHelper* >(pUserData)->m_sGUIFilename.toASCIIString();

		vector< ::GtkWidget* > l_vWidget;
		gtk_container_foreach(GTK_CONTAINER(gtk_widget_get_parent(GTK_WIDGET(pButton))), collect_widget_cb, &l_vWidget);
		::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);

		::GtkBuilder* l_pBuilderInterface=gtk_builder_new(); // glade_xml_new(l_oUserData.sGUIFilename.c_str(), "setting_editor-color_gradient-dialog", NULL);
		gtk_builder_add_from_file(l_pBuilderInterface, l_oUserData.sGUIFilename.c_str(), NULL);
		gtk_builder_connect_signals(l_pBuilderInterface, NULL);

		l_oUserData.pDialog=GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterface, "setting_editor-color_gradient-dialog"));

		CString l_sInitialGradient=static_cast < CSettingCollectionHelper* >(pUserData)->m_rKernelContext.getConfigurationManager().expand(gtk_entry_get_text(l_pWidget));
		CMatrix l_oInitialGradient;

		OpenViBEToolkit::Tools::ColorGradient::parse(l_oInitialGradient, l_sInitialGradient);

		l_oUserData.vColorGradient.resize(l_oInitialGradient.getDimensionSize(1) > 2 ? l_oInitialGradient.getDimensionSize(1) : 2);
		for(uint32 i=0; i<l_oInitialGradient.getDimensionSize(1); i++)
		{
			l_oUserData.vColorGradient[i].fPercent    =l_oInitialGradient[i*4];
			l_oUserData.vColorGradient[i].oColor.red  =(guint)(l_oInitialGradient[i*4+1]*.01*65535.);
			l_oUserData.vColorGradient[i].oColor.green=(guint)(l_oInitialGradient[i*4+2]*.01*65535.);
			l_oUserData.vColorGradient[i].oColor.blue =(guint)(l_oInitialGradient[i*4+3]*.01*65535.);
		}

		l_oUserData.pContainer=GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterface, "setting_editor-color_gradient-vbox"));
		l_oUserData.pDrawingArea=GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterface, "setting_editor-color_gradient-drawingarea"));

		g_signal_connect(G_OBJECT(l_oUserData.pDialog), "show", G_CALLBACK(on_initialize_color_gradient), &l_oUserData);
		g_signal_connect(G_OBJECT(l_oUserData.pDrawingArea), "expose_event", G_CALLBACK(on_refresh_color_gradient), &l_oUserData);
		g_signal_connect(G_OBJECT(gtk_builder_get_object(l_pBuilderInterface, "setting_editor-color_gradient-add_button")), "pressed", G_CALLBACK(on_button_color_gradient_add_pressed), &l_oUserData);
		g_signal_connect(G_OBJECT(gtk_builder_get_object(l_pBuilderInterface, "setting_editor-color_gradient-remove_button")), "pressed", G_CALLBACK(on_button_color_gradient_remove_pressed), &l_oUserData);

		if(gtk_dialog_run(GTK_DIALOG(l_oUserData.pDialog))==GTK_RESPONSE_APPLY)
		{
			CString l_sFinalGradient;
			CMatrix l_oFinalGradient;
			l_oFinalGradient.setDimensionCount(2);
			l_oFinalGradient.setDimensionSize(0, 4);
			l_oFinalGradient.setDimensionSize(1, l_oUserData.vColorGradient.size());
			for(uint32 i=0; i<l_oUserData.vColorGradient.size(); i++)
			{
				l_oFinalGradient[i*4]   = l_oUserData.vColorGradient[i].fPercent;
				l_oFinalGradient[i*4+1] = l_oUserData.vColorGradient[i].oColor.red   * 100. / 65535.;
				l_oFinalGradient[i*4+2] = l_oUserData.vColorGradient[i].oColor.green * 100. / 65535.;
				l_oFinalGradient[i*4+3] = l_oUserData.vColorGradient[i].oColor.blue  * 100. / 65535.;
			}
			OpenViBEToolkit::Tools::ColorGradient::format(l_sFinalGradient, l_oFinalGradient);
			gtk_entry_set_text(l_pWidget, l_sFinalGradient.toASCIIString());
		}

		gtk_widget_destroy(l_oUserData.pDialog);
		g_object_unref(l_pBuilderInterface);
	}
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

CSettingCollectionHelper::CSettingCollectionHelper(const IKernelContext& rKernelContext, const char* sGUIFilename)
	:m_rKernelContext(rKernelContext)
	,m_sGUIFilename(sGUIFilename)
{
}

CSettingCollectionHelper::~CSettingCollectionHelper(void)
{
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

CString CSettingCollectionHelper::getSettingWidgetName(const CIdentifier& rTypeIdentifier)
{
	if(rTypeIdentifier==OV_TypeId_Boolean)       return "settings_collection-hbox_setting_boolean";
	if(rTypeIdentifier==OV_TypeId_Integer)       return "settings_collection-hbox_setting_integer";
	if(rTypeIdentifier==OV_TypeId_Float)         return "settings_collection-hbox_setting_float";
	if(rTypeIdentifier==OV_TypeId_String)        return "settings_collection-entry_setting_string";
	if(rTypeIdentifier==OV_TypeId_Filename)      return "settings_collection-hbox_setting_filename";
	if(rTypeIdentifier==OV_TypeId_Script)        return "settings_collection-hbox_setting_script";
	if(rTypeIdentifier==OV_TypeId_Color)         return "settings_collection-hbox_setting_color";
	if(rTypeIdentifier==OV_TypeId_ColorGradient) return "settings_collection-hbox_setting_color_gradient";
	if(m_rKernelContext.getTypeManager().isEnumeration(rTypeIdentifier)) return "settings_collection-comboboxentry_setting_enumeration";
	if(m_rKernelContext.getTypeManager().isBitMask(rTypeIdentifier))     return "settings_collection-table_setting_bitmask";
	return "settings_collection-entry_setting_string";
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

CString CSettingCollectionHelper::getValue(const CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget)
{
	if(!pWidget) return "";
	if(rTypeIdentifier==OV_TypeId_Boolean)       return getValueBoolean(pWidget);
	if(rTypeIdentifier==OV_TypeId_Integer)       return getValueInteger(pWidget);
	if(rTypeIdentifier==OV_TypeId_Float)         return getValueFloat(pWidget);
	if(rTypeIdentifier==OV_TypeId_String)        return getValueString(pWidget);
	if(rTypeIdentifier==OV_TypeId_Filename)      return getValueFilename(pWidget);
	if(rTypeIdentifier==OV_TypeId_Script)        return getValueScript(pWidget);
	if(rTypeIdentifier==OV_TypeId_Color)         return getValueColor(pWidget);
	if(rTypeIdentifier==OV_TypeId_ColorGradient) return getValueColorGradient(pWidget);
	if(m_rKernelContext.getTypeManager().isEnumeration(rTypeIdentifier)) return getValueEnumeration(rTypeIdentifier, pWidget);
	if(m_rKernelContext.getTypeManager().isBitMask(rTypeIdentifier))     return getValueBitMask(rTypeIdentifier, pWidget);
	return getValueString(pWidget);
}

CString CSettingCollectionHelper::getValueBoolean(::GtkWidget* pWidget)
{
	vector< ::GtkWidget* > l_vWidget;
	if(!GTK_IS_CONTAINER(pWidget)) return "false";
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	if(!GTK_IS_ENTRY(l_vWidget[1])) return "false";
	::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[1]);
	return CString(gtk_entry_get_text(l_pWidget));
}

CString CSettingCollectionHelper::getValueInteger(::GtkWidget* pWidget)
{
	vector< ::GtkWidget* > l_vWidget;
	if(!GTK_IS_CONTAINER(pWidget)) return "0";
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	if(!GTK_IS_ENTRY(l_vWidget[0])) return "O";
	::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);
	return CString(gtk_entry_get_text(l_pWidget));
}

CString CSettingCollectionHelper::getValueFloat(::GtkWidget* pWidget)
{
	vector< ::GtkWidget* > l_vWidget;
	if(!GTK_IS_CONTAINER(pWidget)) return "0";
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	if(!GTK_IS_ENTRY(l_vWidget[0])) return "O";
	::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);
	return CString(gtk_entry_get_text(l_pWidget));
}

CString CSettingCollectionHelper::getValueString(::GtkWidget* pWidget)
{
	if(!GTK_IS_ENTRY(pWidget)) return "";
	::GtkEntry* l_pWidget=GTK_ENTRY(pWidget);
	return CString(gtk_entry_get_text(l_pWidget));
}

CString CSettingCollectionHelper::getValueFilename(::GtkWidget* pWidget)
{
	vector< ::GtkWidget* > l_vWidget;
	if(!GTK_IS_CONTAINER(pWidget)) return "";
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	if(!GTK_IS_ENTRY(l_vWidget[0])) return "";
	::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);
	return CString(gtk_entry_get_text(l_pWidget));
}

CString CSettingCollectionHelper::getValueScript(::GtkWidget* pWidget)
{
	vector< ::GtkWidget* > l_vWidget;
	if(!GTK_IS_CONTAINER(pWidget)) return "";
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	if(!GTK_IS_ENTRY(l_vWidget[0])) return "";
	::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);
	return CString(gtk_entry_get_text(l_pWidget));
}

CString CSettingCollectionHelper::getValueColor(::GtkWidget* pWidget)
{
	vector< ::GtkWidget* > l_vWidget;
	if(!GTK_IS_CONTAINER(pWidget)) return "";
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	if(!GTK_IS_ENTRY(l_vWidget[0])) return "";
	::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);
	return CString(gtk_entry_get_text(l_pWidget));
}

CString CSettingCollectionHelper::getValueColorGradient(::GtkWidget* pWidget)
{
	vector< ::GtkWidget* > l_vWidget;
	if(!GTK_IS_CONTAINER(pWidget)) return "";
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	if(!GTK_IS_ENTRY(l_vWidget[0])) return "";
	::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);
	return CString(gtk_entry_get_text(l_pWidget));
}

CString CSettingCollectionHelper::getValueEnumeration(const CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget)
{
	if(!GTK_IS_COMBO_BOX(pWidget)) return "";
	::GtkComboBox* l_pWidget=GTK_COMBO_BOX(pWidget);
	return CString(gtk_combo_box_get_active_text(l_pWidget));
}

CString CSettingCollectionHelper::getValueBitMask(const CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget)
{
	vector< ::GtkWidget* > l_vWidget;
	if(!GTK_IS_CONTAINER(pWidget)) return "";
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	string l_sResult;
	for(unsigned int i=0; i<l_vWidget.size(); i++)
	{
		if(!GTK_IS_TOGGLE_BUTTON(l_vWidget[i])) return "";
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_vWidget[i])))
		{
			if(l_sResult!="")
			{
				l_sResult+=string(1, OV_Value_EnumeratedStringSeparator);
			}
			l_sResult+=gtk_button_get_label(GTK_BUTTON(l_vWidget[i]));
		}
	}

	return CString(l_sResult.c_str());
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

void CSettingCollectionHelper::setValue(const CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget, const CString& rValue)
{
	if(!pWidget) return;
	if(rTypeIdentifier==OV_TypeId_Boolean)       return setValueBoolean(pWidget, rValue);
	if(rTypeIdentifier==OV_TypeId_Integer)       return setValueInteger(pWidget, rValue);
	if(rTypeIdentifier==OV_TypeId_Float)         return setValueFloat(pWidget, rValue);
	if(rTypeIdentifier==OV_TypeId_String)        return setValueString(pWidget, rValue);
	if(rTypeIdentifier==OV_TypeId_Filename)      return setValueFilename(pWidget, rValue);
	if(rTypeIdentifier==OV_TypeId_Script)        return setValueScript(pWidget, rValue);
	if(rTypeIdentifier==OV_TypeId_Color)         return setValueColor(pWidget, rValue);
	if(rTypeIdentifier==OV_TypeId_ColorGradient) return setValueColorGradient(pWidget, rValue);
	if(m_rKernelContext.getTypeManager().isEnumeration(rTypeIdentifier)) return setValueEnumeration(rTypeIdentifier, pWidget, rValue);
	if(m_rKernelContext.getTypeManager().isBitMask(rTypeIdentifier))     return setValueBitMask(rTypeIdentifier, pWidget, rValue);
	return setValueString(pWidget, rValue);
}

void CSettingCollectionHelper::setValueBoolean(::GtkWidget* pWidget, const CString& rValue)
{
	vector< ::GtkWidget* > l_vWidget;
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	::GtkToggleButton* l_pToggleButtonWidget=GTK_TOGGLE_BUTTON(l_vWidget[0]);
	::GtkEntry* l_pEntryWidget=GTK_ENTRY(l_vWidget[1]);

	if(rValue==CString("true"))
	{
		gtk_toggle_button_set_active(l_pToggleButtonWidget, true);
	}
	else if(rValue==CString("false"))
	{
		gtk_toggle_button_set_active(l_pToggleButtonWidget, false);
	}
	else
	{
		gtk_toggle_button_set_inconsistent(l_pToggleButtonWidget, true);
	}

	gtk_entry_set_text(l_pEntryWidget, rValue);

	g_signal_connect(G_OBJECT(l_vWidget[0]), "toggled", G_CALLBACK(on_checkbutton_setting_boolean_pressed), this);
	g_signal_connect(G_OBJECT(l_vWidget[1]), "changed", G_CALLBACK(on_entry_setting_boolean_edited), this);
}

void CSettingCollectionHelper::setValueInteger(::GtkWidget* pWidget, const CString& rValue)
{
	vector< ::GtkWidget* > l_vWidget;
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);

	g_signal_connect(G_OBJECT(l_vWidget[1]), "clicked", G_CALLBACK(on_button_setting_integer_up_pressed), this);
	g_signal_connect(G_OBJECT(l_vWidget[2]), "clicked", G_CALLBACK(on_button_setting_integer_down_pressed), this);

	gtk_entry_set_text(l_pWidget, rValue);
}

void CSettingCollectionHelper::setValueFloat(::GtkWidget* pWidget, const CString& rValue)
{
	vector< ::GtkWidget* > l_vWidget;
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);

	g_signal_connect(G_OBJECT(l_vWidget[1]), "clicked", G_CALLBACK(on_button_setting_float_up_pressed), this);
	g_signal_connect(G_OBJECT(l_vWidget[2]), "clicked", G_CALLBACK(on_button_setting_float_down_pressed), this);

	gtk_entry_set_text(l_pWidget, rValue);
}

void CSettingCollectionHelper::setValueString(::GtkWidget* pWidget, const CString& rValue)
{
	::GtkEntry* l_pWidget=GTK_ENTRY(pWidget);
	gtk_entry_set_text(l_pWidget, rValue);
}

void CSettingCollectionHelper::setValueFilename(::GtkWidget* pWidget, const CString& rValue)
{
	vector< ::GtkWidget* > l_vWidget;
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);

	g_signal_connect(G_OBJECT(l_vWidget[1]), "clicked", G_CALLBACK(on_button_setting_filename_browse_pressed), this);

	gtk_entry_set_text(l_pWidget, rValue);
}

void CSettingCollectionHelper::setValueScript(::GtkWidget* pWidget, const CString& rValue)
{
	vector< ::GtkWidget* > l_vWidget;
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);

	g_signal_connect(G_OBJECT(l_vWidget[1]), "clicked", G_CALLBACK(on_button_setting_script_edit_pressed), this);
	g_signal_connect(G_OBJECT(l_vWidget[2]), "clicked", G_CALLBACK(on_button_setting_filename_browse_pressed), this);

	gtk_entry_set_text(l_pWidget, rValue);
}

void CSettingCollectionHelper::setValueColor(::GtkWidget* pWidget, const CString& rValue)
{
	vector< ::GtkWidget* > l_vWidget;
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);

	g_signal_connect(G_OBJECT(l_vWidget[1]), "color-set", G_CALLBACK(on_button_setting_color_choose_pressed), this);

	int r=0, g=0, b=0;
	::sscanf(m_rKernelContext.getConfigurationManager().expand(rValue).toASCIIString(), "%i,%i,%i", &r, &g, &b);

	::GdkColor l_oColor;
	l_oColor.red  =(r*65535)/100;
	l_oColor.green=(g*65535)/100;
	l_oColor.blue =(b*65535)/100;
	gtk_color_button_set_color(GTK_COLOR_BUTTON(l_vWidget[1]), &l_oColor);

	gtk_entry_set_text(l_pWidget, rValue);
}

void CSettingCollectionHelper::setValueColorGradient(::GtkWidget* pWidget, const CString& rValue)
{
	vector< ::GtkWidget* > l_vWidget;
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);

	g_signal_connect(G_OBJECT(l_vWidget[1]), "clicked", G_CALLBACK(on_button_setting_color_gradient_configure_pressed), this);

	gtk_entry_set_text(l_pWidget, rValue);
}

void CSettingCollectionHelper::setValueEnumeration(const CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget, const CString& rValue)
{
	::GtkTreeIter l_oListIter;
	::GtkComboBox* l_pWidget=GTK_COMBO_BOX(pWidget);
	::GtkListStore* l_pList=GTK_LIST_STORE(gtk_combo_box_get_model(l_pWidget));
	uint64 l_ui64Value=m_rKernelContext.getTypeManager().getEnumerationEntryValueFromName(rTypeIdentifier, rValue);
	uint64 i;

#if 0
	if(rTypeIdentifier==OV_TypeId_Stimulation)
	{
#endif
		std::map < CString, uint64 > m_vListEntries;
		std::map < CString, uint64 >::const_iterator it;

		for(i=0; i<m_rKernelContext.getTypeManager().getEnumerationEntryCount(rTypeIdentifier); i++)
		{
			CString l_sEntryName;
			uint64 l_ui64EntryValue;
			if(m_rKernelContext.getTypeManager().getEnumerationEntry(rTypeIdentifier, i, l_sEntryName, l_ui64EntryValue))
			{
				m_vListEntries[l_sEntryName]=l_ui64EntryValue;
			}
		}

		gtk_combo_box_set_wrap_width(l_pWidget, 0);
		gtk_list_store_clear(l_pList);
		for(i=0, it=m_vListEntries.begin(); it!=m_vListEntries.end(); it++, i++)
		{
			gtk_list_store_append(l_pList, &l_oListIter);
			gtk_list_store_set(l_pList, &l_oListIter, 0, it->first.toASCIIString(), -1);

			if(l_ui64Value==it->second)
			{
				gtk_combo_box_set_active(l_pWidget, (gint)i);
			}
		}
#if 0
	}
	else
	{
		gtk_list_store_clear(l_pList);
		for(i=0; i<m_rKernelContext.getTypeManager().getEnumerationEntryCount(rTypeIdentifier); i++)
		{
			CString l_sEntryName;
			uint64 l_ui64EntryValue;
			if(m_rKernelContext.getTypeManager().getEnumerationEntry(rTypeIdentifier, i, l_sEntryName, l_ui64EntryValue))
			{
				gtk_list_store_append(l_pList, &l_oListIter);
				gtk_list_store_set(l_pList, &l_oListIter, 0, l_sEntryName.toASCIIString(), -1);

				if(l_ui64Value==l_ui64EntryValue)
				{
					gtk_combo_box_set_active(l_pWidget, (gint)i);
				}
			}
		}
	}
#endif
	if(gtk_combo_box_get_active(l_pWidget)==-1)
	{
		gtk_list_store_append(l_pList, &l_oListIter);
		gtk_list_store_set(l_pList, &l_oListIter, 0, rValue.toASCIIString(), -1);
		gtk_combo_box_set_active(l_pWidget, (gint)i); // $$$ i should be ok :)
	}
}

void CSettingCollectionHelper::setValueBitMask(const CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget, const CString& rValue)
{
	gtk_container_foreach(GTK_CONTAINER(pWidget), remove_widget_cb, pWidget);

	string l_sValue(rValue);

	gint l_iTableSize=(guint)((m_rKernelContext.getTypeManager().getBitMaskEntryCount(rTypeIdentifier)+1)>>1);
	::GtkTable* l_pBitMaskTable=GTK_TABLE(pWidget);
	gtk_table_resize(l_pBitMaskTable, 2, l_iTableSize);

	for(uint64 i=0; i<m_rKernelContext.getTypeManager().getBitMaskEntryCount(rTypeIdentifier); i++)
	{
		CString l_sEntryName;
		uint64 l_ui64EntryValue;
		if(m_rKernelContext.getTypeManager().getBitMaskEntry(rTypeIdentifier, i, l_sEntryName, l_ui64EntryValue))
		{
			::GtkWidget* l_pSettingButton=::gtk_check_button_new();
			gtk_table_attach_defaults(l_pBitMaskTable, l_pSettingButton, (guint)(i&1), (guint)((i&1)+1), (guint)(i>>1), (guint)((i>>1)+1));
			gtk_button_set_label(GTK_BUTTON(l_pSettingButton), (const char*)l_sEntryName);

			if(l_sValue.find((const char*)l_sEntryName)!=string::npos)
			{
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pSettingButton), true);
			}
		}
	}

/*
 * TODO - Add an entry text somewhere to manage
 * configuration through configuration manager !
 */

	gtk_widget_show_all(GTK_WIDGET(l_pBitMaskTable));
}
