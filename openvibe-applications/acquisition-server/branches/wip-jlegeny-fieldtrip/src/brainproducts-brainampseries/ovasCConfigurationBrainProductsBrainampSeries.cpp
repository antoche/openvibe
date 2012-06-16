/*
 * Brain Products Brainamp Series driver for OpenViBE
 * Copyright (C) 2010 INRIA - Author : Yann Renard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include "ovasCConfigurationBrainProductsBrainampSeries.h"

#include "../ovasIHeader.h"

#if defined OVAS_OS_Windows

#include "ovasCDriverBrainProductsBrainampSeries.h"

#include <iostream>
#include <windows.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEAcquisitionServer;
#define boolean OpenViBE::boolean

namespace
{
	EParameter toIndex(uint32 ui32Value)
	{
		switch(ui32Value)
		{
			case 1: return DecimationFactor_None;
			case 2: return DecimationFactor_2;
			case 4: return DecimationFactor_4;
			case 5: return DecimationFactor_5;
			case 8: return DecimationFactor_8;
			case 10: return DecimationFactor_10;
			default: return DecimationFactor_None;
		}
		return DecimationFactor_None;
	}

	uint32 toValue(EParameter eParameter)
	{
		switch(eParameter)
		{
			case DecimationFactor_None: return 1;
			case DecimationFactor_2: return 2;
			case DecimationFactor_4: return 4;
			case DecimationFactor_5: return 5;
			case DecimationFactor_8: return 8;
			case DecimationFactor_10: return 10;
			default: return 1;
		}
		return 1;
	}


	static void button_channel_details_pressed_cb(::GtkButton* pButton, void* pUserData)
	{
		static_cast<CConfigurationBrainProductsBrainampSeries*>(pUserData)->buttonChannelDetailsPressedCB();
	}

	static void button_channel_selected_toggled_cb(::GtkToggleButton* pButton, void* pUserData)
	{
		if(::gtk_toggle_button_get_active(pButton))
		{
			::gtk_button_set_label(GTK_BUTTON(pButton), "selected");
		}
		else
		{
			::gtk_button_set_label(GTK_BUTTON(pButton), "unselected");
		}
	}

	static void combobox_device_changed_cb(::GtkComboBox* pComboBox, void* pUserData)
	{
		CConfigurationBrainProductsBrainampSeries* l_pConfiguration=static_cast < CConfigurationBrainProductsBrainampSeries* >(pUserData);
		l_pConfiguration->comboBoxDeviceChangedCB();
	}
}

CConfigurationBrainProductsBrainampSeries::CConfigurationBrainProductsBrainampSeries(
	CDriverBrainProductsBrainampSeries& rDriver,
	const char* sGtkBuilderFileName,
	uint32& rUSBIndex,
	uint32& rui32DecimationFactor,
	EParameter* pChannelSelected,
	EParameter* peLowPassFilterFull,
	EParameter* peResolutionFull,
	EParameter* peDCCouplingFull,
	EParameter& reLowPass,
	EParameter& reResolution,
	EParameter& reDCCoupling,
	EParameter& reImpedance)

	:CConfigurationBuilder(sGtkBuilderFileName)
	,m_rDriver(rDriver)
	,m_rUSBIndex(rUSBIndex)
	,m_rui32DecimationFactor(rui32DecimationFactor)
	,m_peChannelSelected(pChannelSelected)
	,m_peLowPassFilterFull(peLowPassFilterFull)
	,m_peResolutionFull(peResolutionFull)
	,m_peDCCouplingFull(peDCCouplingFull)
	,m_reLowPass(reLowPass)
	,m_reResolution(reResolution)
	,m_reDCCoupling(reDCCoupling)
	,m_reImpedance(reImpedance)
{
}

boolean CConfigurationBrainProductsBrainampSeries::preConfigure(void)
{
	if(!CConfigurationBuilder::preConfigure())
	{
		return false;
	}

	::GtkComboBox* l_pComboBoxDevice=GTK_COMBO_BOX(::gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_device"));
	::GtkComboBox* l_pComboBoxLowPass=GTK_COMBO_BOX(::gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_lowpass_filter"));
	::GtkComboBox* l_pComboBoxDecimation=GTK_COMBO_BOX(::gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_decimation_factor"));
	::GtkComboBox* l_pComboBoxResolution=GTK_COMBO_BOX(::gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_resolution"));
	::GtkComboBox* l_pComboBoxDCCoupling=GTK_COMBO_BOX(::gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_dc_coupling"));
	::GtkComboBox* l_pComboBoxImpedance=GTK_COMBO_BOX(::gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_impedance"));

	g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface, "button_channel_details"), "pressed", G_CALLBACK(button_channel_details_pressed_cb), this);
	g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_device"), "changed", G_CALLBACK(combobox_device_changed_cb), this);

	char l_sBuffer[1024];
	int l_iCount=0;
	boolean l_bSelected=false;

	// autodetection of the connected device(s)
	for(uint32 i=0; i<16; i++)
	{
		if(m_rDriver.getDeviceDetails(i, NULL, NULL))
		{
			::sprintf(l_sBuffer, "USB port %i", i);
			gtk_combo_box_append_text(l_pComboBoxDevice, l_sBuffer);
			{
				if(m_rUSBIndex==i)
				{
					gtk_combo_box_set_active(l_pComboBoxDevice, l_iCount);
					l_bSelected=true;
				}
			}
			l_iCount++;
		}
	}

	if(!l_bSelected && l_iCount != 0)
	{
		::gtk_combo_box_set_active(l_pComboBoxDevice, 0);
	}

	::gtk_combo_box_set_active(l_pComboBoxLowPass, m_reLowPass);
	::gtk_combo_box_set_active(l_pComboBoxDecimation, ::toIndex(m_rui32DecimationFactor));
	::gtk_combo_box_set_active(l_pComboBoxResolution, m_reResolution);
	::gtk_combo_box_set_active(l_pComboBoxDCCoupling, m_reDCCoupling);
	::gtk_combo_box_set_active(l_pComboBoxImpedance, m_reImpedance);

	return true;
}

boolean CConfigurationBrainProductsBrainampSeries::postConfigure(void)
{
	::GtkComboBox* l_pComboBox=GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_device"));

	if(m_bApplyConfiguration)
	{
		::GtkComboBox* l_pComboBoxDevice=GTK_COMBO_BOX(::gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_device"));
		::GtkComboBox* l_pComboBoxLowPass=GTK_COMBO_BOX(::gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_lowpass_filter"));
		::GtkComboBox* l_pComboBoxDecimation=GTK_COMBO_BOX(::gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_decimation_factor"));
		::GtkComboBox* l_pComboBoxResolution=GTK_COMBO_BOX(::gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_resolution"));
		::GtkComboBox* l_pComboBoxDCCoupling=GTK_COMBO_BOX(::gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_dc_coupling"));
		::GtkComboBox* l_pComboBoxImpedance=GTK_COMBO_BOX(::gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_impedance"));

		uint32 l_ui32USBIndex=0;
		const char* l_sUSBIndex=::gtk_combo_box_get_active_text(l_pComboBoxDevice);
		if(l_sUSBIndex)
		{
			if(::sscanf(l_sUSBIndex, "USB port %i", &l_ui32USBIndex)==1)
			{
				m_rUSBIndex=l_ui32USBIndex;
			}
		}

		m_reLowPass=EParameter(::gtk_combo_box_get_active(l_pComboBoxLowPass));
		m_rui32DecimationFactor=(::toValue(EParameter(::gtk_combo_box_get_active(l_pComboBoxDecimation))));
		m_reResolution=EParameter(::gtk_combo_box_get_active(l_pComboBoxResolution));
		m_reDCCoupling=EParameter(::gtk_combo_box_get_active(l_pComboBoxDCCoupling));
		m_reImpedance=EParameter(::gtk_combo_box_get_active(l_pComboBoxImpedance));
	}

	if(!CConfigurationBuilder::postConfigure())
	{
		return false;
	}
	return true;
}

void CConfigurationBrainProductsBrainampSeries::buttonChannelDetailsPressedCB(void)
{
	::GtkBuilder* l_pBuilder=::gtk_builder_new();
	::gtk_builder_add_from_file(l_pBuilder, m_sGtkBuilderFileName.c_str(), NULL);

	::GtkDialog* l_pDialogChannelDetails=GTK_DIALOG(::gtk_builder_get_object(l_pBuilder, "dialog_channel_details"));
	::GtkTable* l_pDialogChannelDetailsTable=GTK_TABLE(::gtk_builder_get_object(l_pBuilder, "table_content"));

	::GtkTreeModel* l_pTreeModelLowPassFilterFull=GTK_TREE_MODEL(::gtk_builder_get_object(l_pBuilder, "model_lowpass_filter_full"));
	::GtkTreeModel* l_pTreeModelResolutionFull=GTK_TREE_MODEL(::gtk_builder_get_object(l_pBuilder, "model_resolution_full"));
	::GtkTreeModel* l_pTreeModelDCCouplingFull=GTK_TREE_MODEL(::gtk_builder_get_object(l_pBuilder, "model_dc_coupling_full"));

	::gtk_table_resize(l_pDialogChannelDetailsTable, 1, 5);
	::gtk_table_resize(l_pDialogChannelDetailsTable, 1+m_pHeader->getChannelCount(), 5);
	for(uint32 i=1; i<1+m_pHeader->getChannelCount(); i++)
	{
		char l_sIndex[1024];
		::sprintf(l_sIndex, "%i:", i);
		::GtkLabel* l_pWidgetIndex=GTK_LABEL(::gtk_label_new(l_sIndex));
		::gtk_label_set_justify(l_pWidgetIndex, GTK_JUSTIFY_LEFT);
		::gtk_widget_show(GTK_WIDGET(l_pWidgetIndex));

		::GtkLabel* l_pWidgetChannelName=GTK_LABEL(::gtk_label_new(m_pHeader->getChannelName(i-1)));
		::gtk_label_set_justify(l_pWidgetChannelName, GTK_JUSTIFY_LEFT);
		::gtk_widget_show(GTK_WIDGET(l_pWidgetChannelName));

		::GtkBox* l_pWidgetChannel=GTK_BOX(::gtk_hbox_new(false, 2));
		::gtk_box_pack_start(l_pWidgetChannel, GTK_WIDGET(l_pWidgetIndex), true, true, 0);
		::gtk_box_pack_start(l_pWidgetChannel, GTK_WIDGET(l_pWidgetChannelName), true, true, 0);
		::gtk_widget_show(GTK_WIDGET(l_pWidgetChannel));
		::gtk_table_attach_defaults(l_pDialogChannelDetailsTable, GTK_WIDGET(l_pWidgetChannel), 0, 1, i, i+1);

		::GtkToggleButton* l_pWidgetChannelSelected=GTK_TOGGLE_BUTTON(::gtk_check_button_new());
		::gtk_toggle_button_set_active(l_pWidgetChannelSelected, m_peChannelSelected[i-1]==Channel_Selected);
		::gtk_widget_show(GTK_WIDGET(l_pWidgetChannelSelected));
		::gtk_table_attach_defaults(l_pDialogChannelDetailsTable, GTK_WIDGET(l_pWidgetChannelSelected), 1, 2, i, i+1);

		::GtkComboBox* l_pWidgetLowPassFilterFull=GTK_COMBO_BOX(::gtk_combo_box_new_text());
		::gtk_combo_box_set_model(l_pWidgetLowPassFilterFull, l_pTreeModelLowPassFilterFull);
		::gtk_combo_box_set_active(l_pWidgetLowPassFilterFull, gint(m_peLowPassFilterFull[i-1]+1));
		::gtk_widget_show(GTK_WIDGET(l_pWidgetLowPassFilterFull));
		::gtk_table_attach_defaults(l_pDialogChannelDetailsTable, GTK_WIDGET(l_pWidgetLowPassFilterFull), 2, 3, i, i+1);

		::GtkComboBox* l_pWidgetResolutionFull=GTK_COMBO_BOX(::gtk_combo_box_new_text());
		::gtk_combo_box_set_model(l_pWidgetResolutionFull, l_pTreeModelResolutionFull);
		::gtk_combo_box_set_active(l_pWidgetResolutionFull, gint(m_peResolutionFull[i-1]+1));
		::gtk_widget_show(GTK_WIDGET(l_pWidgetResolutionFull));
		::gtk_table_attach_defaults(l_pDialogChannelDetailsTable, GTK_WIDGET(l_pWidgetResolutionFull), 3, 4, i, i+1);

		::GtkComboBox* l_pWidgetDCCouplingFull=GTK_COMBO_BOX(::gtk_combo_box_new_text());
		::gtk_combo_box_set_model(l_pWidgetDCCouplingFull, l_pTreeModelDCCouplingFull);
		::gtk_combo_box_set_active(l_pWidgetDCCouplingFull, gint(m_peDCCouplingFull[i-1]+1));
		::gtk_widget_show(GTK_WIDGET(l_pWidgetDCCouplingFull));
		::gtk_table_attach_defaults(l_pDialogChannelDetailsTable, GTK_WIDGET(l_pWidgetDCCouplingFull), 4, 5, i, i+1);
	}

	if(::gtk_dialog_run(l_pDialogChannelDetails)==GTK_RESPONSE_APPLY)
	{
		::GtkTableChild* l_pTableChild=NULL;
		::GList* l_pList=NULL;
		for(l_pList=l_pDialogChannelDetailsTable->children; l_pList; l_pList=l_pList->next)
		{
			l_pTableChild=(::GtkTableChild*)l_pList->data;
			if(l_pTableChild)
			{
				gint i=l_pTableChild->top_attach-1;
				if(i>=0)
				{
					switch(l_pTableChild->left_attach)
					{
						case 1: m_peChannelSelected[i]=EParameter(::gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pTableChild->widget))); break;
						case 2: m_peLowPassFilterFull[i]=EParameter(::gtk_combo_box_get_active(GTK_COMBO_BOX(l_pTableChild->widget))-1); break;
						case 3: m_peResolutionFull[i]=EParameter(::gtk_combo_box_get_active(GTK_COMBO_BOX(l_pTableChild->widget))-1); break;
						case 4: m_peDCCouplingFull[i]=EParameter(::gtk_combo_box_get_active(GTK_COMBO_BOX(l_pTableChild->widget))-1); break;
						default: break;
					}
				}
			}
		}
	}

	::gtk_widget_hide(GTK_WIDGET(l_pDialogChannelDetails));

	g_object_unref(l_pBuilder);
}

void CConfigurationBrainProductsBrainampSeries::comboBoxDeviceChangedCB(void)
{
	::GtkComboBox* l_pComboBoxDevice=GTK_COMBO_BOX(::gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_device"));
	uint32 l_ui32USBIndex=-1;
	const char* l_sUSBIndex=::gtk_combo_box_get_active_text(l_pComboBoxDevice);
	if(l_sUSBIndex)
	{
		if(::sscanf(l_sUSBIndex, "USB port %i", &l_ui32USBIndex)==1)
		{
			uint32 l_pAmplifierType[4];
			uint32 l_ui32AmplifierCount=0;
			if(m_rDriver.getDeviceDetails(l_ui32USBIndex, &l_ui32AmplifierCount, l_pAmplifierType))
			{
				boolean l_bCorrectMontage=true;
				m_pHeader->setChannelCount(l_ui32AmplifierCount*32);
				gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_pNumberOfChannels), l_ui32AmplifierCount*32);

				for(uint32 i=0; i<BrainAmp_MaximumAmplifierCount; i++)
				{
					const char* l_sAmplifierType="";
					char l_sWidgetName[1024];
					::sprintf(l_sWidgetName, "label_multiamp_montage_%i", i+1);
					::GtkLabel* l_pMultiampMontage=GTK_LABEL(::gtk_builder_get_object(m_pBuilderConfigureInterface, l_sWidgetName));
					::gtk_label_set_label(l_pMultiampMontage, ::getDeviceType(l_pAmplifierType[i]));
					if(i>=1 && l_pAmplifierType[i-1]==AmplifierType_None && l_pAmplifierType[i]!=AmplifierType_None)
					{
						l_bCorrectMontage=false;
					}
				}

				::GtkWidget* l_pMontageTable=GTK_WIDGET(::gtk_builder_get_object(m_pBuilderConfigureInterface, "table_multiamp_montage"));
				::GtkWidget* l_pMontageIcon=GTK_WIDGET(::gtk_builder_get_object(m_pBuilderConfigureInterface, "image_montage_status_icon"));
				if(l_bCorrectMontage)
				{
					const char* l_sText="The multi amplifier montage is correct";
					::gtk_image_set_from_stock(GTK_IMAGE(l_pMontageIcon), GTK_STOCK_YES, GTK_ICON_SIZE_BUTTON);
					::gtk_widget_set_tooltip_markup(l_pMontageIcon, l_sText);
					::gtk_widget_set_tooltip_markup(l_pMontageTable, l_sText);
				}
				else
				{
					const char* l_sText="The multi amplifier montage is not correct - avoid empty slots before filled slots";
					::gtk_image_set_from_stock(GTK_IMAGE(l_pMontageIcon), GTK_STOCK_NO, GTK_ICON_SIZE_BUTTON);
					::gtk_widget_set_tooltip_markup(l_pMontageIcon, l_sText);
					::gtk_widget_set_tooltip_markup(l_pMontageTable, l_sText);
				}
			}
		}
	}
}

#endif // OVAS_OS_Windows
