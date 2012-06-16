#include "ovpCTimeFrequencyMapChannelDisplay.h"

#include <cmath>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

using namespace OpenViBEToolkit;

using namespace std;

namespace
{
	inline uint32* getLookUpTable(void)
	{
		static uint32 l_pTable[1<<8];
		static boolean l_bInitialized=false;

		if(!l_bInitialized)
		{
			int i=0;

for(i=0; i<256; i++) l_pTable[(256*0+i)/2]=((i/2    )<<16)+((0    )<<8)+((255-i));
for(i=0; i<256; i++) l_pTable[(256*2+i)/4]=((127-i/2)<<16)+((i    )<<8)+((0    ));
for(i=0; i<256; i++) l_pTable[(256*3+i)/4]=((0      )<<16)+((255  )<<8)+((i    ));

			l_bInitialized=true;
		}

		return l_pTable;
	}

	inline void drawBoxToBuffer(guchar* pRGBBuffer, uint32 ui32RowStride, uint32 ui32X, uint32 ui32Y, uint32 ui32Width, uint32 ui32Height, uint8 ui8Red, uint8 ui8Green, uint8 ui8Blue)
	{
		if(pRGBBuffer)
		{
			guchar * l_pLineBase = pRGBBuffer + (ui32RowStride*ui32Y) + (ui32X*3);
			for(uint32 j=ui32Height ; j>0 ; j--)
			{
				guchar * l_pLine=l_pLineBase;
				for(uint32 i=ui32Width ; i>0 ; i--)
				{
					*(l_pLine++) =  ui8Red;
					*(l_pLine++) =  ui8Green;
					*(l_pLine++) =  ui8Blue;
				}
				l_pLineBase+=ui32RowStride;
			}
		}
	}
};

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		gboolean timeFrequencyMapChannelDisplaySizeAllocateCallback(::GtkWidget *widget, ::GtkAllocation *allocation, gpointer data);
		gboolean timeFrequencyMapChannelDisplayExposeEventCallback(::GtkWidget *widget, GdkEventExpose *event, gpointer data);
		gboolean timeFrequencyMapLeftRulerExposeEventCallback(::GtkWidget *widget, GdkEventExpose *event, gpointer data);
	}
};

CTimeFrequencyMapChannelDisplay::CTimeFrequencyMapChannelDisplay(CSpectrumDatabase& rSpectrumDatabase, uint32 ui32Channel,
	float64 f64Attenuation, float64 f64MinimumDisplayedFrequency, float64 f64MaximumDisplayedFrequency,
	int32 i32ChannelDisplayWidthRequest, int32 i32ChannelDisplayHeightRequest,
	int32 i32LeftRulerWidthRequest, int32 i32LeftRulerHeightRequest) :
	m_rSpectrumDatabase(rSpectrumDatabase),
	m_pWidgetTable(NULL),
	m_pDisplay(NULL),
	m_pLeftRuler(NULL),
	m_pLeftRulerLeftPadding(5),
	m_pGraphicsContext(NULL),
	m_ui32Channel(ui32Channel),
	m_f64MinimumDisplayedFrequency(f64MinimumDisplayedFrequency),
	m_f64MaximumDisplayedFrequency(f64MaximumDisplayedFrequency),
	m_f64MinimumValue(0),
	m_f64MaximumValue(0),
	m_f64Attenuation(f64Attenuation),
	m_pRGBBuffer(NULL),
	m_ui32RGBBufferWidth(0),
	m_ui32RGBBufferHeight(0),
	m_ui32Rowstride(0)
{
	//widgets table
	m_pWidgetTable = GTK_TABLE(gtk_table_new(1, 2, FALSE));

	//TF map display
	m_pDisplay = gtk_drawing_area_new();
	gtk_widget_set_size_request(m_pDisplay, i32ChannelDisplayWidthRequest, i32ChannelDisplayHeightRequest);

	//left ruler
	m_pLeftRuler = gtk_drawing_area_new();
	gtk_widget_set_size_request(m_pLeftRuler, m_pLeftRulerLeftPadding + i32LeftRulerWidthRequest, i32LeftRulerHeightRequest);

	//add displays to table
	gtk_table_attach(m_pWidgetTable, m_pDisplay,
		1, 2, 0, 1,
		static_cast<GtkAttachOptions>(GTK_EXPAND | GTK_FILL), static_cast<GtkAttachOptions>(GTK_EXPAND | GTK_FILL),
		0, 0);

	gtk_table_attach(m_pWidgetTable, m_pLeftRuler,
		0, 1, 0, 1,
		GTK_FILL, GTK_FILL,
		0, 0);

	//TF map display events
	g_signal_connect_after(G_OBJECT(m_pDisplay), "expose_event", G_CALLBACK(timeFrequencyMapChannelDisplayExposeEventCallback), this);
	g_signal_connect(G_OBJECT(m_pDisplay), "size-allocate", G_CALLBACK(timeFrequencyMapChannelDisplaySizeAllocateCallback), this);

	//left ruler events
	g_signal_connect_after(G_OBJECT(m_pLeftRuler), "expose_event", G_CALLBACK(timeFrequencyMapLeftRulerExposeEventCallback), this);
}

CTimeFrequencyMapChannelDisplay::~CTimeFrequencyMapChannelDisplay()
{
	g_object_unref(G_OBJECT(m_pGraphicsContext));

	if(m_pRGBBuffer != NULL)
	{
		delete[] m_pRGBBuffer;
	}
}

::GtkWidget* CTimeFrequencyMapChannelDisplay::getTopWidget()
{
	return GTK_WIDGET(m_pWidgetTable);
}

::GtkWidget* CTimeFrequencyMapChannelDisplay::getMapWidget()
{
	return GTK_WIDGET(m_pDisplay);
}

void CTimeFrequencyMapChannelDisplay::toggle(boolean bActive)
{
	if(bActive)
	{
		gtk_widget_show(GTK_WIDGET(m_pWidgetTable));
	}
	else
	{
		gtk_widget_hide(GTK_WIDGET(m_pWidgetTable));
	}
}

void CTimeFrequencyMapChannelDisplay::toggleLeftRuler(boolean bActive)
{
	if(bActive)
	{
		gtk_widget_show(m_pLeftRuler);
	}
	else
	{
		gtk_widget_hide(m_pLeftRuler);
	}
}

void CTimeFrequencyMapChannelDisplay::drawMap()
{
	gdk_draw_rgb_image(
		m_pDisplay->window,
		m_pDisplay->style->fg_gc[GTK_WIDGET_STATE (m_pDisplay)],
		0,
		0,
		m_ui32RGBBufferWidth,
		m_ui32RGBBufferHeight,
		GDK_RGB_DITHER_NONE,
		m_pRGBBuffer,
		m_ui32Rowstride);
}

void CTimeFrequencyMapChannelDisplay::update()
{
	if(m_rSpectrumDatabase.isFirstBufferReceived() == false || //ensure there is data to display
		m_pRGBBuffer == NULL) //has a size-allocate event been received yet?
	{
		return;
	}

	//create graphics context if needed
	if(m_pGraphicsContext == NULL)
	{
		m_pGraphicsContext = gdk_gc_new(m_pDisplay->window);
	}

	//compute displayed frequency band indices
	uint32 l_ui32MinDisplayedFrequencyIndex=0;
	uint32 l_ui32MaxDisplayedFrequencyIndex=0;
	if(getDisplayedFrequencyBandIndices(l_ui32MinDisplayedFrequencyIndex, l_ui32MaxDisplayedFrequencyIndex) == false)
	{
		return;
	}

	//get width and height of each frequency band
	gint l_iWidth = 0;
	gint l_iHeight = 0;
	gdk_drawable_get_size(m_pDisplay->window, &l_iWidth, &l_iHeight);
	float64 l_f64WidthPerPoint = 0;
	if(m_rSpectrumDatabase.getMaxBufferCount() > 0)
	{
		l_f64WidthPerPoint = static_cast<float64>(l_iWidth) / static_cast<float64>(m_rSpectrumDatabase.getMaxBufferCount());
	}
	float64 l_f64HeightPerPoint = static_cast<float64>(l_iHeight) /
		static_cast<float64>(l_ui32MaxDisplayedFrequencyIndex - l_ui32MinDisplayedFrequencyIndex + 1);

	//get x coordinate of oldest displayed frequency band
	int64 l_i64BaseX = static_cast<int64>(l_iWidth - (m_rSpectrumDatabase.getCurrentBufferCount() * l_f64WidthPerPoint));
	if(l_i64BaseX<0)
	{
		l_i64BaseX = 0;
	}
	if(l_i64BaseX > 0)
	{
		//clear undrawn part of buffer
		drawBoxToBuffer(m_pRGBBuffer, m_ui32Rowstride, 0, 0, (uint32)l_i64BaseX, (uint32)l_iHeight, 0xFF, 0xFF, 0xFF);
	}

	//values to compute current minimum and maximum power values
	float64 l_f64CurrentBufferMax=-DBL_MAX;
	float64 l_f64CurrentBufferMin=DBL_MAX;

	//compute color scale factor
	float64 l_f64ScaleCoef = 0;
	if(m_f64MinimumValue < m_f64MaximumValue)
	{
		l_f64ScaleCoef = 256.0/(m_f64MaximumValue - m_f64MinimumValue);
	}
	const uint32* l_pLookUpTable=getLookUpTable();

	//draw TF map, one buffer at a time
	float64 l_f64XPosition=(float64)l_i64BaseX;
	float64 l_f64YPosition=(float64)l_iHeight;

	for(size_t j=0 ; j<m_rSpectrumDatabase.getCurrentBufferCount() ; j++)
	{
		//gets a pointer to this channels' samples data in the current buffer
		const float64 * l_pCurrentChannelSampleBuffer = m_rSpectrumDatabase.getBuffer(j);

		l_pCurrentChannelSampleBuffer += m_ui32Channel * m_rSpectrumDatabase.getFrequencyBandCount() + l_ui32MinDisplayedFrequencyIndex;

		l_f64YPosition=l_iHeight;

		//calculates the coordinates of the point in the new base
		for(uint32 f = l_ui32MinDisplayedFrequencyIndex ; f<=l_ui32MaxDisplayedFrequencyIndex; f++, l_pCurrentChannelSampleBuffer++)
		{
			l_f64YPosition-=l_f64HeightPerPoint;

			if(*l_pCurrentChannelSampleBuffer>l_f64CurrentBufferMax) l_f64CurrentBufferMax=*l_pCurrentChannelSampleBuffer;
			if(*l_pCurrentChannelSampleBuffer<l_f64CurrentBufferMin) l_f64CurrentBufferMin=*l_pCurrentChannelSampleBuffer;

			int32 l_i32ColorIndex=static_cast<int32>((*l_pCurrentChannelSampleBuffer - m_f64MinimumValue)*l_f64ScaleCoef);
			if(l_i32ColorIndex>0xff) l_i32ColorIndex=0xff;
			if(l_i32ColorIndex<0x00) l_i32ColorIndex=0x00;
			uint8 l_ui8ColorR=(uint8)((l_pLookUpTable[l_i32ColorIndex]    )&0xff);
			uint8 l_ui8ColorG=(uint8)((l_pLookUpTable[l_i32ColorIndex]>>8 )&0xff);
			uint8 l_ui8ColorB=(uint8)((l_pLookUpTable[l_i32ColorIndex]>>16)&0xff);

			drawBoxToBuffer(
				m_pRGBBuffer,
				m_ui32Rowstride,
				(gint)l_f64XPosition,
				(gint)l_f64YPosition,
				((gint)(l_f64XPosition+l_f64WidthPerPoint))-((gint)(l_f64XPosition)),
				((gint)(l_f64YPosition+l_f64HeightPerPoint))-((gint)(l_f64YPosition)),
				l_ui8ColorR,
				l_ui8ColorG,
				l_ui8ColorB);
		}
		l_f64XPosition+=l_f64WidthPerPoint;
	}

	m_f64MaximumValue = (1-m_f64Attenuation)*m_f64MaximumValue + (m_f64Attenuation)*l_f64CurrentBufferMax;
	m_f64MinimumValue = (1-m_f64Attenuation)*m_f64MinimumValue + (m_f64Attenuation)*l_f64CurrentBufferMin;
}

void CTimeFrequencyMapChannelDisplay::drawLeftRuler()
{
	if(m_rSpectrumDatabase.isFirstBufferReceived() == false || //ensure there is data to display
		!GTK_WIDGET_VISIBLE(m_pLeftRuler)) //if the widget is invisible, no need to redraw it
	{
		return;
	}

	//retrieve widget size
	gint l_iLeftRulerWidth;
	gint l_iLeftRulerHeight;
	gdk_drawable_get_size(m_pLeftRuler->window, &l_iLeftRulerWidth, &l_iLeftRulerHeight);

	//draw ruler base (vertical line)
	gdk_draw_line(
		m_pLeftRuler->window,
		m_pLeftRuler->style->fg_gc[GTK_WIDGET_STATE (m_pLeftRuler)],
		l_iLeftRulerWidth-1, 0, //origin : top right corner
		l_iLeftRulerWidth-1, l_iLeftRulerHeight-1 //end : bottom right corner
		);

	//compute displayed frequency band indices
	uint32 l_ui32MinDisplayedFrequencyIndex=0;
	uint32 l_ui32MaxDisplayedFrequencyIndex=0;
	if(getDisplayedFrequencyBandIndices(l_ui32MinDisplayedFrequencyIndex, l_ui32MaxDisplayedFrequencyIndex) == false)
	{
		return;
	}

	float64 l_f64MinDisplayedFrequency = m_rSpectrumDatabase.getFrequencyBandStart(l_ui32MinDisplayedFrequencyIndex);
	float64 l_f64MaxDisplayedFrequency = m_rSpectrumDatabase.getFrequencyBandStop(l_ui32MaxDisplayedFrequencyIndex);
	float64 l_f64IntervalWidth = l_f64MaxDisplayedFrequency - l_f64MinDisplayedFrequency;
	/*
	//get height of each frequency band
	float64 l_f64HeightPerPoint = static_cast<float64>(l_iLeftRulerHeight) /
		static_cast<float64>(l_ui32MaxDisplayedFrequencyIndex - l_ui32MinDisplayedFrequencyIndex + 1);*/
	float64 l_f64ValueStep, l_f64BaseValue;

	//compute step between labels
	if(l_ui32MinDisplayedFrequencyIndex < l_ui32MaxDisplayedFrequencyIndex)
	{
		float64 l_f64NearestSmallerPowerOf10 = static_cast<float64>(pow(10, floor(log10(l_f64IntervalWidth))));

		if( (l_f64IntervalWidth / l_f64NearestSmallerPowerOf10) > 8 )
		{
			l_f64ValueStep = 4 * l_f64NearestSmallerPowerOf10;
		}
		else if( (l_f64IntervalWidth / l_f64NearestSmallerPowerOf10) > 4 )
		{
			l_f64ValueStep = 2 * l_f64NearestSmallerPowerOf10;
		}
		else if ((l_f64IntervalWidth / l_f64NearestSmallerPowerOf10) > 2 )
		{
			l_f64ValueStep = l_f64NearestSmallerPowerOf10;
		}
		else
		{
			l_f64ValueStep = l_f64NearestSmallerPowerOf10/2;
		}

		//recalculate base value of the step
		l_f64BaseValue = l_f64ValueStep * ceil(l_f64MinDisplayedFrequency/l_f64ValueStep);
	}
	else
	{
		l_f64ValueStep = 1;
		l_f64BaseValue = floor(l_f64MinDisplayedFrequency + 0.5);
	}

	//if the step is too small, it causes problems, so don't display anything and return
	if(l_f64ValueStep < 0.5e-5)
	{
		return;
	}

	PangoLayout* l_pText = gtk_widget_create_pango_layout(m_pLeftRuler, "0");
	int l_iTextH;
	pango_layout_get_pixel_size(l_pText, NULL, &l_iTextH);

	//return if there is no room even for a single label
	if(l_iTextH  > l_iLeftRulerHeight)
	{
		return;
	}

	//increase step so that labels don't step over each other
	int l_iMinGapBetweenLabels = l_iTextH / 3;
	int l_iLabelCount = (int)((l_f64MaxDisplayedFrequency - l_f64MinDisplayedFrequency) / l_f64ValueStep + 1);
	while( l_iLabelCount > 1 &&
		(l_iTextH * l_iLabelCount + l_iMinGapBetweenLabels * (l_iLabelCount-1)) > l_iLeftRulerHeight)
	{
		l_f64ValueStep  *= 2;
		l_iLabelCount = (int)((l_f64MaxDisplayedFrequency - l_f64MinDisplayedFrequency) / l_f64ValueStep + 1);
	}

	//if room for a single label, pick one in the middle of displayed frequency range
	if(l_iLabelCount == 1)
	{
		return;
	}

	//update base value
	l_f64BaseValue = l_f64ValueStep * ceil(l_f64MinDisplayedFrequency/l_f64ValueStep);

	//draw frequency band labels
	for(float64 i=l_f64BaseValue; i<=l_f64MaxDisplayedFrequency; i+=l_f64ValueStep)
	{
		gchar l_pValueLabel[40];

		//compute current label coordinates
		gint l_iTextY = static_cast<gint>(l_iLeftRulerHeight * (1 - (i - l_f64MinDisplayedFrequency)/l_f64IntervalWidth));

		gdk_draw_line(m_pLeftRuler->window, m_pLeftRuler->style->fg_gc[GTK_WIDGET_STATE (m_pLeftRuler)], l_iLeftRulerWidth-3, l_iTextY, l_iLeftRulerWidth-1, l_iTextY);

		//if the current value is (almost) 0, display 0
		if(i < 0.5e-10 && i> -0.5e-10)
		{
			sprintf(l_pValueLabel,"0");
		}
		else
		{
			sprintf(l_pValueLabel, "%g", i);
		}

		PangoLayout* l_pText = gtk_widget_create_pango_layout(m_pLeftRuler, l_pValueLabel);
		//FIXME : arbitrary value- depends on font used
		pango_layout_set_width(l_pText, 28);
		pango_layout_set_justify(l_pText, PANGO_ALIGN_RIGHT);

		//ensure text is not truncated by top/bottom edges
		if(l_iTextY - (l_iTextH/2) < 0)
		{
			l_iTextY = l_iTextH/2;
		}
		else if((l_iTextY + ceil(l_iTextH/2.f)) > (l_iLeftRulerHeight-1))
		{
			l_iTextY = (gint)(l_iLeftRulerHeight - ceil(l_iTextH/2.f));
		}

		gdk_draw_layout(
			m_pLeftRuler->window,
			m_pLeftRuler->style->fg_gc[GTK_WIDGET_STATE (m_pLeftRuler)],
			m_pLeftRulerLeftPadding,
			l_iTextY-(l_iTextH/2),
			l_pText);
	}
}

void CTimeFrequencyMapChannelDisplay::resizeRGBBuffer(uint32 ui32Width, uint32 ui32Height)
{
	//align lines on 32bit boundaries
	m_ui32Rowstride = ((ui32Width*3)%4 == 0) ? (ui32Width*3) : ((((ui32Width*3)>>2)+1)<<2);

	//delete previously allocated buffer
	if(m_pRGBBuffer != NULL)
	{
		delete[] m_pRGBBuffer;
	}
	m_pRGBBuffer = new guchar[m_ui32Rowstride*ui32Height];

	m_ui32RGBBufferWidth=ui32Width;
	m_ui32RGBBufferHeight=ui32Height;
}

void CTimeFrequencyMapChannelDisplay::setMinMaxAttenuation(float64 f64Attenuation)
{
	m_f64Attenuation = f64Attenuation;
}

void CTimeFrequencyMapChannelDisplay::setDisplayedFrequencyRange(float64 f64MinimumDisplayedFrequency, float64 f64MaximumDisplayedFrequency)
{
	m_f64MinimumDisplayedFrequency = f64MinimumDisplayedFrequency;
	m_f64MaximumDisplayedFrequency = f64MaximumDisplayedFrequency;
}

boolean CTimeFrequencyMapChannelDisplay::getDisplayedFrequencyBandIndices(uint32& rMinDisplayedFrequencyIndex, uint32& rMaxDisplayedFrequencyIndex)
{
	uint32 l_ui32FrequencyCount = m_rSpectrumDatabase.getFrequencyBandCount();
	if(l_ui32FrequencyCount == 0)
	{
		return false;
	}

	float64 l_f64FrequencyStep = m_rSpectrumDatabase.getFrequencyBandWidth();
	if(l_f64FrequencyStep < 1e-1)
	{
		return false;
	}

	float64 l_f64MinFrequency = m_rSpectrumDatabase.getFrequencyBandStart(0);

	//min frequency index
	rMinDisplayedFrequencyIndex=(uint32)((m_f64MinimumDisplayedFrequency - l_f64MinFrequency) / l_f64FrequencyStep);

	if(rMinDisplayedFrequencyIndex >= l_ui32FrequencyCount)
	{
		rMinDisplayedFrequencyIndex = l_ui32FrequencyCount-1;
	}

	//max frequency index
	rMaxDisplayedFrequencyIndex=(uint32)((m_f64MaximumDisplayedFrequency - l_f64MinFrequency) / l_f64FrequencyStep);

	if(rMaxDisplayedFrequencyIndex >= l_ui32FrequencyCount)
	{
		rMaxDisplayedFrequencyIndex = l_ui32FrequencyCount-1;
	}

	return rMaxDisplayedFrequencyIndex >= rMinDisplayedFrequencyIndex;
}

//CALLBACKS

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		gboolean timeFrequencyMapChannelDisplayExposeEventCallback(::GtkWidget *widget, GdkEventExpose *event, gpointer data)
		{
			//redraw map
			CTimeFrequencyMapChannelDisplay * l_pDisplay = reinterpret_cast<CTimeFrequencyMapChannelDisplay*>(data);
			l_pDisplay->drawMap();

			//don't propagate this signal to the children if any
			return TRUE;
		}

		gboolean timeFrequencyMapChannelDisplaySizeAllocateCallback(::GtkWidget *widget, ::GtkAllocation *allocation, gpointer data)
		{
			CTimeFrequencyMapChannelDisplay * l_pDisplay = reinterpret_cast<CTimeFrequencyMapChannelDisplay*>(data);
			l_pDisplay->resizeRGBBuffer(allocation->width,allocation->height);
			return FALSE;
		}

		gboolean timeFrequencyMapLeftRulerExposeEventCallback(::GtkWidget *widget, GdkEventExpose *event, gpointer data)
		{
			//redraw left ruler
			CTimeFrequencyMapChannelDisplay * l_pDisplay = reinterpret_cast<CTimeFrequencyMapChannelDisplay*>(data);
			l_pDisplay->drawLeftRuler();

			//don't propagate this signal to the children if any
			return TRUE;
		}
	}
}
