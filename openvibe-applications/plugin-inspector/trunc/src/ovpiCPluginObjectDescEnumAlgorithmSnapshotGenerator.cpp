#include "ovpiCPluginObjectDescEnumAlgorithmSnapshotGenerator.h"

#include <system/Time.h>

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <pango/pango.h>

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

namespace
{
	enum
	{
		Color_BackgroundPlayerStarted,
		Color_BoxBackground,
		Color_BoxBackgroundSelected,
		Color_BoxBackgroundMissing,
		Color_BoxBackgroundObsolete,
		Color_BoxBorder,
		Color_BoxBorderSelected,
		Color_BoxInputBackground,
		Color_BoxInputBorder,
		Color_BoxOutputBackground,
		Color_BoxOutputBorder,
		Color_BoxSettingBackground,
		Color_BoxSettingBorder,
		Color_Link,
		Color_LinkSelected,
		Color_SelectionArea,
		Color_SelectionAreaBorder,
	};
/*
	static ::GdkColor colorFromIdentifier(const CIdentifier& rIdentifier)
	{
		::GdkColor l_oGdkColor;
		unsigned int l_ui32Value1=0;
		unsigned int l_ui32Value2=0;
		uint64 l_ui64Result=0;

		sscanf(rIdentifier.toString(), "(0x%08X, 0x%08X)", &l_ui32Value1, &l_ui32Value2);
		l_ui64Result+=l_ui32Value1;
		l_ui64Result<<=32;
		l_ui64Result+=l_ui32Value2;

		l_oGdkColor.pixel=(guint16)0;
		l_oGdkColor.red  =(guint16)(((l_ui64Result    )&0xffff)|0x8000);
		l_oGdkColor.green=(guint16)(((l_ui64Result>>16)&0xffff)|0x8000);
		l_oGdkColor.blue =(guint16)(((l_ui64Result>>32)&0xffff)|0x8000);

		return l_oGdkColor;
	}

	static void gdk_draw_rounded_rectangle(::GdkDrawable* pDrawable, ::GdkGC* pDrawGC, ::gboolean bFill, gint x, gint y, gint width, gint height, gint radius=8)
	{
		if(bFill)
		{
			gdk_draw_rectangle(
				pDrawable,
				pDrawGC,
				TRUE,
				x+radius, y, width-2*radius, height);
			gdk_draw_rectangle(
				pDrawable,
				pDrawGC,
				TRUE,
				x, y+radius, width, height-2*radius);
		}
		else
		{
			gdk_draw_line(
				pDrawable,
				pDrawGC,
				x+radius, y, x+width-radius, y);
			gdk_draw_line(
				pDrawable,
				pDrawGC,
				x+radius, y+height, x+width-radius, y+height);
			gdk_draw_line(
				pDrawable,
				pDrawGC,
				x, y+radius, x, y+height-radius);
			gdk_draw_line(
				pDrawable,
				pDrawGC,
				x+width, y+radius, x+width, y+height-radius);
		}
		gdk_draw_arc(
			pDrawable,
			pDrawGC,
			bFill,
			x+width-radius*2, y, radius*2, radius*2, 0*64, 90*64);
		gdk_draw_arc(
			pDrawable,
			pDrawGC,
			bFill,
			x, y, radius*2, radius*2, 90*64, 90*64);
		gdk_draw_arc(
			pDrawable,
			pDrawGC,
			bFill,
			x, y+height-radius*2, radius*2, radius*2, 180*64, 90*64);
		gdk_draw_arc(
			pDrawable,
			pDrawGC,
			bFill,
			x+width-radius*2, y+height-radius*2, radius*2, radius*2, 270*64, 90*64);
	}
*/
};

// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------

CPluginObjectDescEnumAlgorithmSnapshotGenerator::CPluginObjectDescEnumAlgorithmSnapshotGenerator(const IKernelContext& rKernelContext, const CString& sSnapshotDirectory, const CString& sDocTemplateDirectory)
	:CPluginObjectDescEnum(rKernelContext)
	,m_sSnapshotDirectory(sSnapshotDirectory)
	,m_sDocTemplateDirectory(sDocTemplateDirectory)
{
	#define gdk_color_set(c, r, g, b) { c.pixel=0; c.red=r; c.green=g; c.blue=b; }
	gdk_color_set(m_vColors[Color_BackgroundPlayerStarted], 32767, 32767, 32767);
	gdk_color_set(m_vColors[Color_BoxBackgroundSelected],   65535, 65535, 49151);
	gdk_color_set(m_vColors[Color_BoxBackgroundMissing],    49151, 32767, 32767);
	gdk_color_set(m_vColors[Color_BoxBackgroundObsolete],   32767, 49151, 49151);
	gdk_color_set(m_vColors[Color_BoxBackground],           65535, 65535, 65535);
	gdk_color_set(m_vColors[Color_BoxBorderSelected],           0,     0,     0);
	gdk_color_set(m_vColors[Color_BoxBorder],                   0,     0,     0);
	gdk_color_set(m_vColors[Color_BoxInputBackground],      65535, 49151, 32767);
	gdk_color_set(m_vColors[Color_BoxInputBorder],          16383, 16383, 16383);
	gdk_color_set(m_vColors[Color_BoxOutputBackground],     32767, 65535, 49151);
	gdk_color_set(m_vColors[Color_BoxOutputBorder],         16383, 16383, 16383);
	gdk_color_set(m_vColors[Color_BoxSettingBackground],    49151, 32767, 65535);
	gdk_color_set(m_vColors[Color_BoxSettingBorder],        16383, 16383, 16383);
	gdk_color_set(m_vColors[Color_Link],                        0,     0,     0);
	gdk_color_set(m_vColors[Color_LinkSelected],            49151, 16383, 16383);
	gdk_color_set(m_vColors[Color_SelectionArea],          0x3f00,0x3f00,0x3f00);
	gdk_color_set(m_vColors[Color_SelectionAreaBorder],         0,     0,     0);
	#undef gdk_color_set

	m_pWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	m_pWidget=gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(m_pWindow), m_pWidget);
	gtk_widget_set_size_request(m_pWidget, 512, 128);
	gtk_widget_show_all(m_pWindow);
	gdk_flush();
	System::Time::sleep(1000);
}

CPluginObjectDescEnumAlgorithmSnapshotGenerator::~CPluginObjectDescEnumAlgorithmSnapshotGenerator(void)
{
	std::ofstream l_oAlgorithmsFile;
	l_oAlgorithmsFile.open((m_sDocTemplateDirectory+"/Doc_Algorithms.dox").c_str());
	l_oAlgorithmsFile
		<< "/**\n"
		<< " * \\page Doc_Algorithms Algorithms list\n"
		<< " *\n"
		<< " * Available box algorithms are include :\n";

	uint32 l_ui32Level=0;
	string l_sLastCategory;
	vector < string > l_vLastSplittedCategory;
	vector < pair < string, string > >::iterator itCategories;
	std::sort(m_vCategories.begin(), m_vCategories.end());
	for(itCategories=m_vCategories.begin(); itCategories!=m_vCategories.end(); itCategories++)
	{
		string l_sCategory=itCategories->first;
		string l_sName=itCategories->second;

		if(l_sLastCategory!=l_sCategory)
		{
			vector < string >::iterator itLastSplittedCategory;
			vector < string >::iterator itSplittedCategory1;
			vector < string >::iterator itSplittedCategory2;
			vector < string > l_vSplittedCategory;
			size_t i=(size_t)-1;
			size_t j;
			bool l_bFinished=false;
			while(!l_bFinished)
			{
				j=l_sCategory.find('/', i+1);
				if(j==string::npos)
				{
					j=l_sCategory.length();
					l_bFinished=true;
				}
				if(j!=i+1)
				{
					l_vSplittedCategory.push_back(l_sCategory.substr(i+1, j-i-1));
					i=j;
				}
			}
			l_ui32Level=l_vSplittedCategory.size();

			for(itLastSplittedCategory =l_vLastSplittedCategory.begin(), itSplittedCategory1 =l_vSplittedCategory.begin();
			    itLastSplittedCategory!=l_vLastSplittedCategory.end() && itSplittedCategory1!=l_vSplittedCategory.end() && *itLastSplittedCategory==*itSplittedCategory1;
			    itLastSplittedCategory++, itSplittedCategory1++);

			for(; itSplittedCategory1!=l_vSplittedCategory.end(); itSplittedCategory1++)
			{
				l_oAlgorithmsFile << " * ";
				for(itSplittedCategory2=l_vSplittedCategory.begin(); itSplittedCategory2!=itSplittedCategory1; itSplittedCategory2++)
				{
					l_oAlgorithmsFile << "   ";
				}
				l_oAlgorithmsFile << " - " << *itSplittedCategory1 << " : \n";
			}

			l_sLastCategory=l_sCategory;
			l_vLastSplittedCategory=l_vSplittedCategory;
		}

		l_oAlgorithmsFile << " * ";
		for(uint32 k=0; k<l_ui32Level+1; k++)
			l_oAlgorithmsFile << "   ";
		l_oAlgorithmsFile << " - \\subpage Doc_algorithm_" << transform(l_sCategory+"/"+l_sName).c_str() << " \"" << l_sName << "\"\n";
	}

	l_oAlgorithmsFile << " */\n";
	l_oAlgorithmsFile.close();
}

boolean CPluginObjectDescEnumAlgorithmSnapshotGenerator::callback(const IPluginObjectDesc& rPluginObjectDesc)
{
	string l_sFullName=string(rPluginObjectDesc.getCategory().toASCIIString()) + "/" + string(rPluginObjectDesc.getName().toASCIIString());
	string l_sFilename=m_sSnapshotDirectory+"/Algorithm_"+transform(l_sFullName);
	CIdentifier l_oIdentifier;
	if((l_oIdentifier=m_rKernelContext.getAlgorithmManager().createAlgorithm(rPluginObjectDesc.getCreatedClassIdentifier()))==OV_UndefinedIdentifier)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Skipped [" << CString(l_sFilename.c_str()) << "] (could not create corresponding algorithm)\n";
		return true;
	}
	IAlgorithmProxy& l_rAlgorithm=m_rKernelContext.getAlgorithmManager().getAlgorithm(l_oIdentifier);

	m_rKernelContext.getLogManager() << LogLevel_Trace << "Working on [" << CString(l_sFilename.c_str()) << "]\n";

	::PangoContext* l_pPangoContext=gtk_widget_get_pango_context(m_pWidget);
	::PangoLayout* l_pPangoLayout=pango_layout_new(l_pPangoContext);
	::PangoRectangle l_oPangoRectangle;

	pango_layout_set_alignment(l_pPangoLayout, PANGO_ALIGN_CENTER);
	pango_layout_set_text(l_pPangoLayout, rPluginObjectDesc.getName(), -1);
	pango_layout_get_pixel_extents(l_pPangoLayout, NULL, &l_oPangoRectangle);

	uint32 i;
	const int xMargin=5;
	const int yMargin=5;
	const int iCircleMargin=5;
	const int iCircleSize=11;
	const int iCircleSpace=4;

	int xSize=l_oPangoRectangle.width+xMargin*2;
	int ySize=l_oPangoRectangle.height+yMargin*2;
	int xStart=16;
	int yStart=16;

	gdk_window_invalidate_rect(
		m_pWidget->window,
		NULL,
		true);

	while(gtk_events_pending())
	{
		gtk_main_iteration();
	}

	::GdkGC* l_pDrawGC=gdk_gc_new(m_pWidget->window);

	gdk_gc_set_rgb_fg_color(l_pDrawGC, &m_vColors[Color_BoxBackground]);
	gdk_draw_rectangle(
		m_pWidget->window,
		l_pDrawGC,
		TRUE,
		xStart, yStart, xSize, ySize);
	gdk_gc_set_rgb_fg_color(l_pDrawGC, &m_vColors[Color_BoxBorder]);
	gdk_draw_rectangle(
		m_pWidget->window,
		l_pDrawGC,
		FALSE,
		xStart, yStart, xSize, ySize);

	i=0;
	CIdentifier l_oInputIdentifier;
	while((l_oInputIdentifier=l_rAlgorithm.getNextInputParameterIdentifier(l_oInputIdentifier))!=OV_UndefinedIdentifier)
	{
		::GdkColor l_oInputColor=m_vColors[l_rAlgorithm.getInputParameter(l_oInputIdentifier)->getType()];

		::GdkPoint l_vPoint[4];
		l_vPoint[0].x=iCircleSize>>1;
		l_vPoint[0].y=iCircleSize;
		l_vPoint[1].x=0;
		l_vPoint[1].y=0;
		l_vPoint[2].x=iCircleSize-1;
		l_vPoint[2].y=0;
		for(int j=0; j<3; j++)
		{
			l_vPoint[j].x+=xStart+iCircleMargin+i*(iCircleSpace+iCircleSize);
			l_vPoint[j].y+=yStart-(iCircleSize>>1);
		}

		gdk_gc_set_rgb_fg_color(l_pDrawGC, &l_oInputColor);
		gdk_draw_polygon(
			m_pWidget->window,
			l_pDrawGC,
			TRUE,
			l_vPoint,
			3);
		gdk_gc_set_rgb_fg_color(l_pDrawGC, &m_vColors[Color_BoxInputBorder]);
		gdk_draw_polygon(
			m_pWidget->window,
			l_pDrawGC,
			FALSE,
			l_vPoint,
			3);

		i++;
	}

	i=0;
	CIdentifier l_oOutputIdentifier;
	while((l_oOutputIdentifier=l_rAlgorithm.getNextOutputParameterIdentifier(l_oOutputIdentifier))!=OV_UndefinedIdentifier)
	{
		::GdkColor l_oOutputColor=m_vColors[l_rAlgorithm.getOutputParameter(l_oOutputIdentifier)->getType()];

		::GdkPoint l_vPoint[4];
		l_vPoint[0].x=iCircleSize>>1;
		l_vPoint[0].y=iCircleSize;
		l_vPoint[1].x=0;
		l_vPoint[1].y=0;
		l_vPoint[2].x=iCircleSize-1;
		l_vPoint[2].y=0;
		for(int j=0; j<3; j++)
		{
			l_vPoint[j].x+=xStart+iCircleMargin+i*(iCircleSpace+iCircleSize);
			l_vPoint[j].y+=yStart-(iCircleSize>>1)+ySize;
		}

		gdk_gc_set_rgb_fg_color(l_pDrawGC, &l_oOutputColor);
		gdk_draw_polygon(
			m_pWidget->window,
			l_pDrawGC,
			TRUE,
			l_vPoint,
			3);
		gdk_gc_set_rgb_fg_color(l_pDrawGC, &m_vColors[Color_BoxOutputBorder]);
		gdk_draw_polygon(
			m_pWidget->window,
			l_pDrawGC,
			FALSE,
			l_vPoint,
			3);

		i++;
	}

	gdk_draw_layout(
		m_pWidget->window,
		m_pWidget->style->text_gc[GTK_WIDGET_STATE(m_pWidget)],
		xStart+xMargin, yStart+yMargin, l_pPangoLayout);
	g_object_unref(l_pPangoLayout);
	g_object_unref(l_pDrawGC);

	::GdkPixbuf* l_pPixBuf=gdk_pixbuf_get_from_drawable(
		NULL,
		m_pWidget->window,
		NULL,
		0, 0,
		0, 0,
		xSize+32, ySize+32);
	gdk_pixbuf_save(l_pPixBuf, (l_sFilename+".png").c_str(), "png", NULL, NULL);

	g_object_unref(l_pPixBuf);

	// --------------------------------------------------------------------------------------------------------------------

#if 0

	m_vCategories.push_back(pair < string, string >(rPluginObjectDesc.getCategory().toASCIIString(), rPluginObjectDesc.getName().toASCIIString()));

	std::ofstream l_oFile;
	l_oFile.open((l_sFilename+".dox").toASCIIString());

	l_oFile
		<< "/**\n"
		<< " * \\page Doc_" << l_sFilename.c_str() << " " << l_sFullName.toASCIIString() << "\n"
		<< " * \\section Doc_" << l_sFilename.c_str() << "_summary Summary\n"
		<< " * \\image html " << l_sFilename.c_str() << ".png\n"
		<< " *\n"
		<< " * - Plugin name : " << rPluginObjectDesc.getName() << "\n"
		<< " * - Version : " << rPluginObjectDesc.getVersion() << "\n"
		<< " * - Author : " << rPluginObjectDesc.getAuthorName() << "\n"
		<< " * - Company : " << rPluginObjectDesc.getAuthorCompanyName() << "\n"
		<< " * - Short description : " << rPluginObjectDesc.getShortDescription() << "\n"
		<< " * - Documentation template generation date : " << __DATE__ << "\n"
		<< " *\n"
		<< " * \\section Doc_" << l_sFilename.c_str() << "_description Description\n"
		<< " * " << rPluginObjectDesc.getDetailedDescription() << " TODO\n"
		<< " *\n";

	l_oFile
		<< " * \\section Doc_" << l_sFilename.c_str() << "_inputs Inputs\n"
		<< " * Number of inputs : " << l_rBox.getInputCount() << "\n"
		<< " *\n";
	for(i=0; i<l_rBox.getInputCount(); i++)
	{
		CString l_sName;
		CIdentifier l_oTypeIdentifier;
		l_rBox.getInputName(i, l_sName);
		l_rBox.getInputType(i, l_oTypeIdentifier);
		l_oFile
			<< " * - Input " << i+1 << "\n"
			<< " *  - Name : " << l_sName.toASCIIString() << "\n"
			<< " *  - Type identifier : " << m_rKernelContext.getTypeManager().getTypeName(l_oTypeIdentifier).toASCIIString() << " " << l_oTypeIdentifier.toString().toASCIIString() << "\n"
			<< " *  - Description : TODO\n"
			<< " *\n";
	}

	l_oFile
		<< " * \\section Doc_" << l_sFilename.c_str() << "_outputs Outputs\n"
		<< " * Number of outputs : " << l_rBox.getOutputCount() << "\n"
		<< " *\n";
	for(i=0; i<l_rBox.getOutputCount(); i++)
	{
		CString l_sName;
		CIdentifier l_oTypeIdentifier;
		l_rBox.getOutputName(i, l_sName);
		l_rBox.getOutputType(i, l_oTypeIdentifier);
		l_oFile
			<< " * - Output " << i+1 << "\n"
			<< " *  - Name : " << l_sName.toASCIIString() << "\n"
			<< " *  - Type identifier : " << m_rKernelContext.getTypeManager().getTypeName(l_oTypeIdentifier).toASCIIString() << " " << l_oTypeIdentifier.toString().toASCIIString() << "\n"
			<< " *  - Description : TODO\n"
			<< " *\n";
	}

	l_oFile
		<< " * \\section Doc_" << l_sFilename.c_str() << "_settings Settings\n"
		<< " * Number of settings : " << l_rBox.getSettingCount() << "\n"
		<< " *\n";
	for(i=0; i<l_rBox.getSettingCount(); i++)
	{
		CString l_sName;
		CIdentifier l_oTypeIdentifier;
		CString l_sDefaultValue;
		l_rBox.getSettingName(i, l_sName);
		l_rBox.getSettingType(i, l_oTypeIdentifier);
		l_rBox.getSettingDefaultValue(i, l_sDefaultValue);
		l_oFile
			<< " * - Setting " << i+1 << "\n"
			<< " *  - Name : " << l_sName.toASCIIString() << "\n"
			<< " *  - Type identifier : " << m_rKernelContext.getTypeManager().getTypeName(l_oTypeIdentifier).toASCIIString() << " " << l_oTypeIdentifier.toString().toASCIIString() << "\n"
			<< " *  - Default value : " << l_sDefaultValue.toASCIIString() << "\n"
			<< " *  - Description : TODO\n"
			<< " *\n";
	}

	l_oFile
		<< " * \\section Doc_" << l_sFilename.c_str() << "_examples Examples\n"
		<< " * TODO\n"
		<< " *\n"
		<< " *  \\section Doc_" << l_sFilename.c_str() << "_misc Miscellaneous\n"
		<< " * TODO\n"
		<< " */";

	l_oFile.close();

#endif

	m_rKernelContext.getAlgorithmManager().releaseAlgorithm(l_rAlgorithm);

	return true;
}
