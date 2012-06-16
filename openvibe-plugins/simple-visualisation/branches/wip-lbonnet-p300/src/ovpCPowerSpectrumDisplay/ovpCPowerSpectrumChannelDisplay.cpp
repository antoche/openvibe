#include "ovpCPowerSpectrumChannelDisplay.h"
#include "ovpCPowerSpectrumDisplayView.h"
#include "../ovpCBufferDatabase.h"
#include <cmath>
#include <sstream>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;
using namespace OpenViBEToolkit;
using namespace std;

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		gboolean powerSpectrumExposeEventCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data);
		gboolean powerSpectrumSizeAllocateCallback(GtkWidget *widget, GtkAllocation *allocation, gpointer data);
		gboolean powerSpectrumLeftRulerExposeEventCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data);

		CPowerSpectrumChannelDisplay::CPowerSpectrumChannelDisplay(
			CPowerSpectrumDisplayView* pParentDisplay, uint32 ui32Channel, CPowerSpectrumDatabase& pDatabase,
			int32 i32ChannelDisplayWidthRequest, int32 i32ChannelDisplayHeightRequest,
			int32 i32LeftRulerWidthRequest, int32 i32LeftRulerHeightRequest) :
			m_pParentDisplay(pParentDisplay),
			m_pWidgetTable(NULL),
			m_pDisplay(NULL),
			m_pLeftRuler(NULL),
			m_pGraphicsContext(NULL),
			m_pDatabase(&pDatabase),
			m_ui32Channel(ui32Channel),
			m_pRGBBuffer(NULL),
			m_ui32RGBBufferWidth(0),
			m_ui32RGBBufferHeight(0),
			m_ui32Rowstride(0)
		{
			//widgets table
			m_pWidgetTable = GTK_TABLE(gtk_table_new(1, 2, FALSE));

			//spectrum display
			m_pDisplay = gtk_drawing_area_new();
			gtk_widget_set_size_request(m_pDisplay, i32ChannelDisplayWidthRequest, i32ChannelDisplayHeightRequest);

			//left ruler
			m_pLeftRuler = gtk_drawing_area_new();
			gtk_widget_set_size_request(m_pLeftRuler, i32LeftRulerWidthRequest, i32LeftRulerHeightRequest);

			//add displays to table
			gtk_table_attach(m_pWidgetTable, m_pDisplay,
				1, 2, 0, 1,
				static_cast<GtkAttachOptions>(GTK_EXPAND | GTK_FILL), static_cast<GtkAttachOptions>(GTK_EXPAND | GTK_FILL),
				0, 0);

			gtk_table_attach(m_pWidgetTable, m_pLeftRuler,
				0, 1, 0, 1,
				GTK_FILL, GTK_FILL,
				0, 0);

			//spectrum display events
			g_signal_connect_after(G_OBJECT(m_pDisplay), "expose_event", G_CALLBACK(powerSpectrumExposeEventCallback), this);
			g_signal_connect(G_OBJECT(m_pDisplay), "size-allocate", G_CALLBACK(powerSpectrumSizeAllocateCallback), this);

			//left ruler events
			g_signal_connect_after(G_OBJECT(m_pLeftRuler), "expose_event", G_CALLBACK(powerSpectrumLeftRulerExposeEventCallback), this);
		}

		CPowerSpectrumChannelDisplay::~CPowerSpectrumChannelDisplay()
		{
			g_object_unref(G_OBJECT(m_pGraphicsContext));

			if(m_pRGBBuffer != NULL)
			{
				delete[] m_pRGBBuffer;
			}
		}

		GtkWidget* CPowerSpectrumChannelDisplay::getTopWidget() const
		{
			return GTK_WIDGET(m_pWidgetTable);
		}

		GtkWidget* CPowerSpectrumChannelDisplay::getSpectrumDisplayWidget() const
		{
			return GTK_WIDGET(m_pDisplay);
		}

		void CPowerSpectrumChannelDisplay::toggle(OpenViBE::boolean bActive)
		{
			bActive ? gtk_widget_show(GTK_WIDGET(m_pWidgetTable)) : gtk_widget_hide(GTK_WIDGET(m_pWidgetTable));
		}

		void CPowerSpectrumChannelDisplay::toggleLeftRuler(boolean bActive)
		{
			bActive ? gtk_widget_show(m_pLeftRuler) : gtk_widget_hide(m_pLeftRuler);
		}

		void CPowerSpectrumChannelDisplay::drawSpectrum()
		{
			gdk_draw_rgb_image(
				m_pDisplay->window,
				m_pDisplay->style->fg_gc[GTK_WIDGET_STATE(m_pDisplay)],
				0, 0, //origin
				m_ui32RGBBufferWidth, m_ui32RGBBufferHeight, //dimensions
				GDK_RGB_DITHER_NONE,
				m_pRGBBuffer,
				m_ui32Rowstride);
		}

		void CPowerSpectrumChannelDisplay::update()
		{
			//do we have something to display?
			if(m_pDatabase == NULL || m_pDatabase->getDisplayedFrequencyBandCount() == 0)
			{
				return;
			}

			//has a size-allocate event been received yet?
			if(m_pRGBBuffer == NULL)
			{
				return;
			}

			//create graphics context if needed
			if(m_pGraphicsContext == NULL)
			{
				m_pGraphicsContext = gdk_gc_new(m_pDisplay->window);
			}

			//clear buffer
			drawBoxToBuffer(0, 0, m_ui32RGBBufferWidth, m_ui32RGBBufferHeight, 0xFF, 0xFF, 0xFF);

			//get number of frequency bands to display
			uint32 l_ui32FrequencyCount = m_pDatabase->getDisplayedFrequencyBandCount();

			//get size of drawing area
			gint l_iWidth = 0;
			gint l_iHeight = 0;
			gdk_drawable_get_size(m_pDisplay->window, &l_iWidth, &l_iHeight);

			//width of each frequency band
			float32 l_f32FreqBandWidth = static_cast<float32>(l_iWidth) / l_ui32FrequencyCount;

			GdkColor l_oCurrentColor;
			l_oCurrentColor.pixel = 0;
			// uint8 l_ui8IndexedColor = 0;

			//get current buffer min/max values
			float64 l_f64CurrentBufferMax=-DBL_MAX;
			float64 l_f64CurrentBufferMin=DBL_MAX;
			if(m_pParentDisplay->isAutoVerticalScaleEnabled() == true)
			{
				for(uint32 i=0; i<m_pDatabase->getChannelCount(); i++)
				{
					if(m_pParentDisplay->isSelected(i))
					{
						float64 l_f64CurrentBufferMax2;
						float64 l_f64CurrentBufferMin2;
						m_pDatabase->getLastBufferChannelMinMaxValue(i, l_f64CurrentBufferMin2, l_f64CurrentBufferMax2);
						if(l_f64CurrentBufferMax2 > l_f64CurrentBufferMax) l_f64CurrentBufferMax=l_f64CurrentBufferMax2;
						if(l_f64CurrentBufferMin2 < l_f64CurrentBufferMin) l_f64CurrentBufferMin=l_f64CurrentBufferMin2;
					}
				}
			}
			else
			{
				l_f64CurrentBufferMin = 0;
				l_f64CurrentBufferMax = m_pParentDisplay->getCustomVerticalScaleValue();
			}

			//gets a pointer to this channels' samples data in the current buffer
			float64* l_pChannelBuf = m_pDatabase->getLastBufferChannelPointer(m_ui32Channel);

			//draw frequency bands
			uint32 l_ui32MinDisplayedFrequencyBandIndex = m_pDatabase->getMinDisplayedFrequencyBandIndex();
			uint32 l_ui32MaxDisplayedFrequencyBandIndex = m_pDatabase->getMaxDisplayedFrequencyBandIndex();

			for(uint32 i=l_ui32MinDisplayedFrequencyBandIndex; i<=l_ui32MaxDisplayedFrequencyBandIndex; i++)
			{
				//height ratio
				float ratio = 0;
				if(l_f64CurrentBufferMax > l_f64CurrentBufferMin)
				{
					ratio = (float)((l_pChannelBuf[i] - l_f64CurrentBufferMin) / (l_f64CurrentBufferMax - l_f64CurrentBufferMin));
					if(ratio < 0)
					{
						ratio = 0;
					}
					else if(ratio > 1)
					{
						ratio = 1;
					}
				}

				gint l_iX = (gint) ((i-l_ui32MinDisplayedFrequencyBandIndex) *  l_f32FreqBandWidth);
				gint l_iW = (gint)((i+1-l_ui32MinDisplayedFrequencyBandIndex) * l_f32FreqBandWidth - l_iX);

				gint l_iH = (gint)(l_iHeight * ratio);
				gint l_iY = l_iHeight-l_iH;

				drawBoxToBuffer(
					l_iX, l_iY, //origin
					l_iW, l_iH, //size
					0xFF, 0, 0); //RGB color
			}
		}

		void CPowerSpectrumChannelDisplay::drawLeftRuler()
		{
			//if the widget is invisible, no need to redraw it
			if(!GTK_WIDGET_VISIBLE(m_pLeftRuler))
			{
				return;
			}

			gint l_iLeftRulerWidth;
			gint l_iLeftRulerHeight;
			gdk_drawable_get_size(m_pLeftRuler->window, &l_iLeftRulerWidth, &l_iLeftRulerHeight);

			//draw ruler base (vertical line)
			gdk_draw_line(m_pLeftRuler->window, m_pLeftRuler->style->fg_gc[GTK_WIDGET_STATE (m_pLeftRuler)],  l_iLeftRulerWidth-1, 0, l_iLeftRulerWidth-1, l_iLeftRulerHeight-1);

			//maximum and minimum signal values that will be displayed
			//get current buffer min/max values
			float64 l_f64CurrentBufferMax;
			float64 l_f64CurrentBufferMin;
			if(m_pParentDisplay->isAutoVerticalScaleEnabled() == true)
			{
				m_pDatabase->getLastBufferMinMaxValue(l_f64CurrentBufferMin, l_f64CurrentBufferMax);
			}
			else
			{
				l_f64CurrentBufferMin = 0;
				l_f64CurrentBufferMax = m_pParentDisplay->getCustomVerticalScaleValue();
			}

			float64 l_f64IntervalWidth = l_f64CurrentBufferMax-l_f64CurrentBufferMin;
			float64 l_f64ValueStep;
			float64 l_f64BaseValue;

			//if powers aren't all equal
			if(l_f64IntervalWidth != 0)
			{
				//computes the step
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
				l_f64BaseValue = l_f64ValueStep * ceil(l_f64CurrentBufferMin/l_f64ValueStep);
			}
			else
			{
				l_f64ValueStep = 1;
				l_f64BaseValue = floor(l_f64CurrentBufferMin + 0.5);
			}

			//if the step is too small, it causes problems, so don't display anything and return
			if(l_f64ValueStep < 0.5e-5)
			{
				return;
			}

			for(float64 i=l_f64BaseValue; i<=l_f64CurrentBufferMax; i+=l_f64ValueStep)
			{
				gchar l_pValueLabel[40];

				//computes the coordinate of the current label
				gint l_iTextY = static_cast<gint>(l_iLeftRulerHeight * (1 - (i - l_f64CurrentBufferMin)/l_f64IntervalWidth));

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
				pango_layout_set_width(l_pText, 28);
				pango_layout_set_justify(l_pText, PANGO_ALIGN_RIGHT);

				int l_iTextW;
				int l_iTextH;
				pango_layout_get_pixel_size(l_pText, &l_iTextW, &l_iTextH);

				if(l_iTextH  > l_iLeftRulerHeight)
				{
					break;
				}

				if(l_iTextY - (l_iTextH/2) < 0)
				{
					l_iTextY = l_iTextH/2;
				}
				else if((l_iTextY + ceil(l_iTextH/2.f)) > (l_iLeftRulerHeight-1))
				{
					l_iTextY = (gint)(l_iLeftRulerHeight - ceil(l_iTextH/2.f));
				}

				gdk_draw_layout(m_pLeftRuler->window, m_pLeftRuler->style->fg_gc[GTK_WIDGET_STATE (m_pLeftRuler)], 0, l_iTextY-(l_iTextH/2), l_pText);
			}
		}

		void CPowerSpectrumChannelDisplay::resizeRGBBuffer(OpenViBE::uint32 ui32Width, OpenViBE::uint32 ui32Height)
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

		void CPowerSpectrumChannelDisplay::drawBoxToBuffer(uint32 ui32X, uint32 ui32Y, uint32 ui32Width, uint32 ui32Height, uint8 ui8Red, uint8 ui8Green, uint8 ui8Blue)
		{
			guchar * l_pLineBase = m_pRGBBuffer + (m_ui32Rowstride*ui32Y) + (ui32X*3);

			for(uint32 j=0 ; j<ui32Height ; j++)
			{
				for(uint32 i=0 ; i<(ui32Width*3) ; i+=3)
				{
					*(l_pLineBase + i) =  ui8Red;
					*(l_pLineBase + i + 1) =  ui8Green;
					*(l_pLineBase + i + 2) =  ui8Blue;
				}

				l_pLineBase+=(m_ui32Rowstride);
			}
		}

		//CALLBACKS

		gboolean powerSpectrumExposeEventCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
		{
			//redraw spectrum
			CPowerSpectrumChannelDisplay * l_pDisplay = reinterpret_cast<CPowerSpectrumChannelDisplay*>(data);
			l_pDisplay->drawSpectrum();
			return TRUE;
		}

		gboolean powerSpectrumSizeAllocateCallback(GtkWidget *widget, GtkAllocation *allocation, gpointer data)
		{
			CPowerSpectrumChannelDisplay * l_pDisplay = reinterpret_cast<CPowerSpectrumChannelDisplay*>(data);
			l_pDisplay->resizeRGBBuffer(allocation->width,allocation->height);
			return FALSE;
		}

		gboolean powerSpectrumLeftRulerExposeEventCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
		{
			//redraw left ruler (powers)
			CPowerSpectrumChannelDisplay* l_pDisplay = reinterpret_cast<CPowerSpectrumChannelDisplay*>(data);
			l_pDisplay->drawLeftRuler();
			return TRUE;
		}
	}
}
