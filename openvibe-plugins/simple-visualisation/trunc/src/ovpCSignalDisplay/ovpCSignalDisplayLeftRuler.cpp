#include "ovpCSignalDisplayLeftRuler.h"

#include <iostream>
#include <sstream>
#include <cmath>
#include <glib.h>
#include <glib/gprintf.h>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

using namespace std;

#define convert_time(i) (float64)(i>>32) + (float64)((float64)(i&0xFFFFFFFF) / (float64)((uint64)1<<32))

//! Callback to redraw the bottom ruler
gboolean leftRulerExposeEventCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	//redraw the ruler
	CSignalDisplayLeftRuler * l_pLeftRuler = reinterpret_cast<CSignalDisplayLeftRuler*>(data);
	l_pLeftRuler->draw();

	//don't propagate this signal to the children if any
	return TRUE;
}

//! Called when the widget whose height is associated with the ruler is resized.
gboolean resizeLeftRulerCallback(GtkWidget *widget, GtkAllocation *allocation, gpointer data)
{
	CSignalDisplayLeftRuler * l_pLeftRuler = reinterpret_cast<CSignalDisplayLeftRuler*>(data);

	//resizes the left ruler
	gtk_widget_set_size_request(l_pLeftRuler->getWidget(), l_pLeftRuler->m_i32WidthRequest, allocation->height);

	return FALSE;
}

CSignalDisplayLeftRuler::CSignalDisplayLeftRuler(int32 i32WidthRequest, int32 i32HeightRequest) :
	m_i32WidthRequest(i32WidthRequest)
	,m_f64MaximumDisplayedValue(-DBL_MAX)
	,m_f64MinimumDisplayedValue(DBL_MAX)
	,m_ui64PixelsPerLeftRulerLabel(10)
{
	//creates the main drawing area
	m_pLeftRuler = gtk_drawing_area_new();

	gtk_widget_set_size_request(m_pLeftRuler, i32WidthRequest, i32HeightRequest);

	g_signal_connect_after(G_OBJECT(m_pLeftRuler), "expose_event", G_CALLBACK(leftRulerExposeEventCallback), this);

	//get left ruler widget's font description
	PangoContext * l_pPangoContext = gtk_widget_get_pango_context(m_pLeftRuler);
	PangoFontDescription * l_pFontDescription = pango_context_get_font_description(l_pPangoContext);

	//adapt the allocated height per label to the font's height (plus 4 pixel to add some spacing)
	if(pango_font_description_get_size_is_absolute(l_pFontDescription))
	{
		m_ui64PixelsPerLeftRulerLabel = pango_font_description_get_size(l_pFontDescription) + 4;
	}
	else
	{
		m_ui64PixelsPerLeftRulerLabel = pango_font_description_get_size(l_pFontDescription)/PANGO_SCALE + 4;
	}
}

CSignalDisplayLeftRuler::~CSignalDisplayLeftRuler()
{
	g_object_unref(m_pLeftRuler);
}

void CSignalDisplayLeftRuler::update(float64 f64Min, float64 f64Max)
{
	m_f64MaximumDisplayedValue = f64Max;
	m_f64MinimumDisplayedValue = f64Min;

	//redraw the ruler
	if(m_pLeftRuler->window) gdk_window_invalidate_rect(m_pLeftRuler->window, NULL, true);
}

GtkWidget* CSignalDisplayLeftRuler::getWidget() const
{
	return m_pLeftRuler;
}

void CSignalDisplayLeftRuler::draw()
{
	if(!GTK_WIDGET_VISIBLE(m_pLeftRuler))
	{
		return;
	}

	gint l_iLeftRulerWidth;
	gint l_iLeftRulerHeight;
	gdk_drawable_get_size(m_pLeftRuler->window, &l_iLeftRulerWidth, &l_iLeftRulerHeight);

	//draw ruler base (vertical line)
	gdk_draw_line(m_pLeftRuler->window, m_pLeftRuler->style->fg_gc[GTK_WIDGET_STATE (m_pLeftRuler)],  l_iLeftRulerWidth-1, 0, l_iLeftRulerWidth-1, l_iLeftRulerHeight);

	//computes the step in values for the ruler
	float64 l_f64IntervalWidth = m_f64MaximumDisplayedValue-m_f64MinimumDisplayedValue;
	float64 l_f64ValueStep = 0;
	float64 l_f64BaseValue = 0;
	uint64 l_ui64MaxNumberOfLabels = 0;

	//if the signal is not constant
	if(l_f64IntervalWidth > 0)
	{
		//computes the step
		float64 l_f64NearestSmallerPowerOf10 = static_cast<float64>(pow(10, floor(log10(l_f64IntervalWidth))));

		//get max number of labels that fit in widget
		l_ui64MaxNumberOfLabels = (uint64)(l_iLeftRulerHeight / m_ui64PixelsPerLeftRulerLabel);

		//ensure there is room for at least one label
		if(l_ui64MaxNumberOfLabels > 0)
		{
			//get the current number of labels to display based on the nearest inferior power of ten value
			uint64 l_ui64TempNumberOfLabels = (uint64)floor(l_f64IntervalWidth / l_f64NearestSmallerPowerOf10);

			if(l_ui64TempNumberOfLabels > 2*l_ui64MaxNumberOfLabels)
			{
				l_f64ValueStep = 4 * l_f64NearestSmallerPowerOf10;
			}
			else if(l_ui64TempNumberOfLabels > l_ui64MaxNumberOfLabels)
			{
				l_f64ValueStep = 2 * l_f64NearestSmallerPowerOf10;
			}
			else if(l_ui64TempNumberOfLabels < (l_ui64MaxNumberOfLabels/4))
			{
				l_f64ValueStep = l_f64NearestSmallerPowerOf10 / 4;
			}
			else if(l_ui64TempNumberOfLabels < (l_ui64MaxNumberOfLabels/2))
			{
				l_f64ValueStep = l_f64NearestSmallerPowerOf10 / 2;
			}
			else
			{
				l_f64ValueStep = l_f64NearestSmallerPowerOf10;
			}

			//recompute base value of the step
			l_f64BaseValue = l_f64ValueStep *floor(m_f64MinimumDisplayedValue/l_f64ValueStep);
		}
	}
	else
	{
		l_f64ValueStep = 1;
		l_f64BaseValue = floor(m_f64MinimumDisplayedValue - 0.5);
	}

	int l_iTextW;
	int l_iTextH;

	//if the step is too small, it causes problems, so don't display anything and return
	if(l_f64ValueStep < 0.5e-5)
	{
		return;
	}

	for(float64 i=l_f64BaseValue ; i<static_cast<float64>(0.5+m_f64MaximumDisplayedValue) ; i+=l_f64ValueStep)
	{
		gchar l_pValueLabel[40];

		//computes the coordinate of the current label
		gint l_iTextY = static_cast<gint>((m_f64MaximumDisplayedValue - i) * (l_iLeftRulerHeight/l_f64IntervalWidth));

		if(l_iTextY>=0 && l_iTextY<=l_iLeftRulerHeight)
		{

			//if the current value is (almost) 0, displays 0
			if(i < 0.5e-10 && i> -0.5e-10)
			{
				g_sprintf(l_pValueLabel,"0");
			}
			else
			{
				g_sprintf(l_pValueLabel, "%g", i);
			}

			PangoLayout * l_pText = gtk_widget_create_pango_layout(m_pLeftRuler, l_pValueLabel);
			pango_layout_set_width(l_pText, 28);
			pango_layout_set_justify(l_pText, PANGO_ALIGN_RIGHT);

			pango_layout_get_pixel_size(l_pText, &l_iTextW, &l_iTextH);

			gdk_draw_layout(m_pLeftRuler->window, m_pLeftRuler->style->fg_gc[GTK_WIDGET_STATE (m_pLeftRuler)],
					0, l_iTextY-(l_iTextH/2), l_pText);

			gdk_draw_line(m_pLeftRuler->window, m_pLeftRuler->style->fg_gc[GTK_WIDGET_STATE (m_pLeftRuler)], l_iLeftRulerWidth-4, l_iTextY, l_iLeftRulerWidth, l_iTextY);

		}
	}

}


