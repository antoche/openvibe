#include "ovpCBottomTimeRuler.h"

#include <cmath>

#include <sstream>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

using namespace std;

#define convert_time(i) (float64)(i>>32) + (float64)((float64)(i&0xFFFFFFFF) / (float64)((uint64)1<<32))

gboolean bottomRulerExposeEventCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	//redraw the ruler
	CBottomTimeRuler * l_pBottomRuler = reinterpret_cast<CBottomTimeRuler*>(data);
	l_pBottomRuler->draw();

	//don't propagate this signal to the children if any
	return TRUE;
}

gboolean resizeBottomRulerCallback(GtkWidget *widget, GtkAllocation *allocation, gpointer data)
{
	CBottomTimeRuler * l_pBottomRuler = reinterpret_cast<CBottomTimeRuler*>(data);
	l_pBottomRuler->onResizeEventCB(allocation->width, allocation->height);
	return FALSE;
}

CBottomTimeRuler::CBottomTimeRuler(CBufferDatabase& pDatabase, int32 i32WidthRequest, int32 i32HeightRequest) :
	m_pDatabase(&pDatabase),
	m_i32HeightRequest(i32HeightRequest),
	m_ui64PixelsPerBottomRulerLabel(20),
	m_ui64LeftmostDisplayedTime(0)
{
	//creates the main drawing area
	m_pBottomRuler = gtk_drawing_area_new();

	gtk_widget_set_size_request(m_pBottomRuler, i32WidthRequest, i32HeightRequest);
	g_signal_connect_after(G_OBJECT(m_pBottomRuler), "expose_event", G_CALLBACK(bottomRulerExposeEventCallback), this);
}

CBottomTimeRuler::~CBottomTimeRuler()
{
}

GtkWidget* CBottomTimeRuler::getWidget() const
{
	return m_pBottomRuler;
}

void CBottomTimeRuler::draw()
{
	//if the widget is invisible, no need to redraw it
	if(!GTK_WIDGET_VISIBLE(m_pBottomRuler))
	{
		return;
	}

	//gets the number of buffers to display
	uint64 l_ui64NumberOfBufferToDisplay = m_pDatabase->m_ui64NumberOfBufferToDisplay;

	if(m_pDatabase->m_pDimmensionSizes[1] == 1 && l_ui64NumberOfBufferToDisplay != 1)
	{
		// l_ui64NumberOfBufferToDisplay--;
	}

	//gets the widget's size
	gint l_iBottomRulerWidth;
	gint l_iBottomRulerHeight;
	gdk_drawable_get_size(m_pBottomRuler->window, &l_iBottomRulerWidth, &l_iBottomRulerHeight);

	//in ms
	float64 l_f64IntervalWidth = 0;
	//if(m_pDatabase->areEpochsContiguous() == true)
	{
		l_f64IntervalWidth = convert_time(l_ui64NumberOfBufferToDisplay * m_pDatabase->m_ui64BufferDuration);
	}
	/*else
	{
		//consider start times of first and last epochs
		l_f64IntervalWidth = convert_time(l_ui64NumberOfBufferToDisplay * m_pDatabase->m_ui64BufferDuration);
	}*/

	//available width per buffer
	float64 l_f64WidthPerBuffer = static_cast<float64>(l_iBottomRulerWidth) / static_cast<float64>(l_ui64NumberOfBufferToDisplay);

	//computes the step of the values displayed on the ruler
	float64 l_f64ValueStep = 0;
	float64 l_f64NearestSmallerPowerOf10 = static_cast<float64>(pow(10, floor(log10(l_f64IntervalWidth))));
	uint64 l_ui64MaxNumberOfLabels = (uint64)(l_iBottomRulerWidth / m_ui64PixelsPerBottomRulerLabel);

	if( (uint64)floor(l_f64IntervalWidth / l_f64NearestSmallerPowerOf10) > l_ui64MaxNumberOfLabels )
	{
		l_f64ValueStep = 2 * l_f64NearestSmallerPowerOf10;
	}
	else if( (uint64)floor(l_f64IntervalWidth / l_f64NearestSmallerPowerOf10) < l_ui64MaxNumberOfLabels/2 )
	{
		l_f64ValueStep = l_f64NearestSmallerPowerOf10 / 2;
	}
	else
	{
		l_f64ValueStep = l_f64NearestSmallerPowerOf10;
	}

	if(m_pDatabase->getDisplayMode() == OVP_TypeId_SignalDisplayMode_Scroll)
	{
		//compute start, end time and base value of the step
		float64 l_f64StartTime = 0;
		if(m_pDatabase->m_oStartTime.size() != 0)
		{
			l_f64StartTime = convert_time(m_pDatabase->m_oStartTime[0]);
		}

		float64 l_f64EndTime = l_f64StartTime + l_f64IntervalWidth;

		float64 l_f64BaseValue = l_f64ValueStep * floor(l_f64StartTime/l_f64ValueStep);

		//X position of the first label (if there are less buffers than needed)
		int64 l_i64BaseX = static_cast<int64>(floor(l_iBottomRulerWidth - ( m_pDatabase->m_oSampleBuffers.size() * l_f64WidthPerBuffer)));
		if(l_i64BaseX<0)
		{
			l_i64BaseX = 0;
		}

		//draw ruler base (horizontal line)
		gdk_draw_line(m_pBottomRuler->window, m_pBottomRuler->style->fg_gc[GTK_WIDGET_STATE (m_pBottomRuler)], (gint)l_i64BaseX, 0, (gint)l_iBottomRulerWidth, 0);

		int32 l_i32ClipLeft = 0;
		int32 l_i32ClipRight = l_iBottomRulerWidth - 1;

		drawRuler(l_i64BaseX, l_iBottomRulerWidth, l_f64StartTime, l_f64EndTime, l_f64IntervalWidth, l_f64BaseValue, l_f64ValueStep, l_i32ClipLeft, l_i32ClipRight);
	}
	else //scan mode
	{
		//draw ruler base (horizontal line)
		gdk_draw_line(m_pBottomRuler->window, m_pBottomRuler->style->fg_gc[GTK_WIDGET_STATE (m_pBottomRuler)], 0, 0, (gint)l_iBottomRulerWidth, 0);

		//left part of the ruler (recent data)
		uint32 l_ui32LeftmostBufferToDisplay = 0;
		m_pDatabase->getIndexOfBufferStartingAtTime(m_ui64LeftmostDisplayedTime, l_ui32LeftmostBufferToDisplay);
		float64 l_f64StartTime = 0;
		if(m_pDatabase->m_oStartTime.size() != 0)
		{
			l_f64StartTime = convert_time(m_ui64LeftmostDisplayedTime);
		}
		float64 l_f64EndTime = l_f64StartTime + l_f64IntervalWidth;
		float64 l_f64BaseValue = l_f64ValueStep * floor(l_f64StartTime/l_f64ValueStep);
		int32 l_i32ClipLeft = 0;
		int32 l_i32ClipRight = (int32)((m_pDatabase->m_ui64NumberOfBufferToDisplay - l_ui32LeftmostBufferToDisplay) * l_f64WidthPerBuffer);

		drawRuler(0, l_iBottomRulerWidth, l_f64StartTime, l_f64EndTime, l_f64IntervalWidth, l_f64BaseValue, l_f64ValueStep, l_i32ClipLeft, l_i32ClipRight);

		//right part (older data)
		l_f64StartTime -= l_f64IntervalWidth;
		l_f64EndTime = l_f64StartTime + l_f64IntervalWidth;
		l_f64BaseValue = l_f64ValueStep * floor(l_f64StartTime/l_f64ValueStep);
		l_i32ClipLeft = l_i32ClipRight + 1;
		l_i32ClipRight = l_iBottomRulerWidth - 1;

		drawRuler(0, l_iBottomRulerWidth, l_f64StartTime, l_f64EndTime, l_f64IntervalWidth, l_f64BaseValue, l_f64ValueStep, l_i32ClipLeft, l_i32ClipRight);
	}
}

void CBottomTimeRuler::onResizeEventCB(gint i32Width, gint i32Height)
{
	gtk_widget_set_size_request(m_pBottomRuler, i32Width, m_i32HeightRequest);
}

void CBottomTimeRuler::drawRuler(int64 i64BaseX, int32 i32RulerWidth, float64 f64StartTime, float64 f64EndTime, float64 f64Length, float64 f64BaseValue, float64 f64ValueStep, int32 i32ClipLeft, int32 i32ClipRight)
{
	stringstream l_oTimeLabel;

	for(float64 i=f64BaseValue ; i<static_cast<float64>(0.5+f64EndTime) ; i+=f64ValueStep)
	{
		//clears the stringstream
		l_oTimeLabel.str("");

		//compute the position of the label
		gint l_iTextX = static_cast<gint>(i64BaseX + ((i - f64StartTime)*(((float64)i32RulerWidth)/f64Length))) ;

		//is text clipped?
		if(l_iTextX < i32ClipLeft)
		{
			continue;
		}

		l_oTimeLabel<<i;

		PangoLayout * l_pText = gtk_widget_create_pango_layout(m_pBottomRuler, l_oTimeLabel.str().c_str());

		int l_iTextWidth;
		pango_layout_get_pixel_size(l_pText, &l_iTextWidth, NULL);

		//is text beyond visible range?
		if(l_iTextX + l_iTextWidth > i32ClipRight)
		{
			break;
		}

		//if the width allocated per label becomes too small compared to the effective width of the label
		if( static_cast<uint64>(l_iTextWidth) >= m_ui64PixelsPerBottomRulerLabel-20)
		{
			//increases the allocated width per label
			m_ui64PixelsPerBottomRulerLabel = l_iTextWidth + 30;
		}

		//display it
		gdk_draw_layout(m_pBottomRuler->window, m_pBottomRuler->style->fg_gc[GTK_WIDGET_STATE (m_pBottomRuler)], l_iTextX, 4, l_pText);

		//draw a small line above it
		gdk_draw_line(m_pBottomRuler->window, m_pBottomRuler->style->fg_gc[GTK_WIDGET_STATE (m_pBottomRuler)], l_iTextX, 0, l_iTextX, 3);
	}
}

void CBottomTimeRuler::setLeftmostDisplayedTime(uint64 ui64LeftmostDisplayedTime)
{
	m_ui64LeftmostDisplayedTime = ui64LeftmostDisplayedTime;
}

void CBottomTimeRuler::linkWidthToWidget(GtkWidget * pWidget)
{
	//adds a callback to the widget for the size-allocate signal
	g_signal_connect(G_OBJECT(pWidget), "size-allocate", G_CALLBACK(resizeBottomRulerCallback), this);
}
