#include "ovpCSignalChannelDisplay.h"

#include "ovpCSignalDisplayView.h"
#include <cfloat>

#include <sstream>
#include <iostream>

using namespace std;

using namespace OpenViBE;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

#define convert_time(i) (float64)(i>>32) + (float64)((float64)(i&0xFFFFFFFF) / (float64)((uint64)1<<32))

gboolean drawingAreaExposeEventCallback(GtkWidget* pWidget, GdkEventExpose* pEvent, gpointer data);
gboolean drawingAreaResizeEventCallback(GtkWidget* pWidget, GtkAllocation* pAllocation, gpointer data);
void drawingAreaClickedEventCallback(GtkWidget* pWidget, GdkEventButton* pEvent, gpointer data);
void drawingAreaEnterEventCallback(GtkWidget* pWidget, GdkEventCrossing* pEvent, gpointer data);
void drawingAreaLeaveEventCallback(GtkWidget* pWidget, GdkEventCrossing* pEvent, gpointer data);

CSignalChannelDisplay::CSignalChannelDisplay(
	CSignalDisplayView* pDisplayView,
	int32 i32ChannelDisplayWidthRequest,
	int32 i32ChannelDisplayHeightRequest,
	int32 i32LeftRulerWidthRequest,
	int32 i32LeftRulerHeightRequest)
	:m_pLeftRuler(NULL)
	,m_pDrawingArea(NULL)
	,m_ui32Width(0)
	,m_ui32Height(0)
	,m_f64WidthPerBuffer(0)
	,m_f64PointStep(0)
	,m_pParentDisplayView(pDisplayView)
	,m_pDatabase(pDisplayView->m_pBufferDatabase)
	,m_f64LocalMaximum(0)
	,m_f64LocalMinimum(0)
	,m_f64ScaleX(1)
	,m_f64ScaleY(1)
	,m_f64TranslateX(0)
	,m_f64TranslateY(0)
	,m_f64ZoomTranslateX(0)
	,m_f64ZoomTranslateY(0)
	,m_f64ZoomScaleX(1)
	,m_f64ZoomScaleY(1)
	,m_f64ZoomFactor(1.5)
	,m_f64MaximumTopMargin(0)
	,m_f64MaximumBottomMargin(0)
	,m_f64MinimumTopMargin(0)
	,m_f64MinimumBottomMargin(0)
	,m_eCurrentSignalMode(DisplayMode_GlobalBestFit)
	,m_ui64LatestDisplayedTime(0)
	,m_bRedrawAll(false)
{
	//creates the drawing area
	m_pDrawingArea = gtk_drawing_area_new();
	gtk_widget_set_size_request(m_pDrawingArea, i32ChannelDisplayWidthRequest, i32ChannelDisplayHeightRequest);

	//creates the left ruler
	m_pLeftRuler = new CSignalDisplayLeftRuler(i32LeftRulerWidthRequest, i32LeftRulerHeightRequest);

	//connects the signals
	gtk_widget_add_events(GTK_WIDGET(m_pDrawingArea), GDK_BUTTON_PRESS_MASK);
	gtk_widget_add_events(GTK_WIDGET(m_pDrawingArea), GDK_ENTER_NOTIFY_MASK);
	gtk_widget_add_events(GTK_WIDGET(m_pDrawingArea), GDK_LEAVE_NOTIFY_MASK);

	g_signal_connect_after(G_OBJECT(m_pDrawingArea), "expose_event",       G_CALLBACK(drawingAreaExposeEventCallback), this);
	g_signal_connect_after(G_OBJECT(m_pDrawingArea), "size-allocate",      G_CALLBACK(drawingAreaResizeEventCallback), this);
	g_signal_connect_after(G_OBJECT(m_pDrawingArea), "button-press-event", G_CALLBACK(drawingAreaClickedEventCallback), this);
	g_signal_connect_after(G_OBJECT(m_pDrawingArea), "enter-notify-event", G_CALLBACK(drawingAreaEnterEventCallback), this);
	g_signal_connect_after(G_OBJECT(m_pDrawingArea), "leave-notify-event", G_CALLBACK(drawingAreaLeaveEventCallback), this);
}

CSignalChannelDisplay::~CSignalChannelDisplay()
{
	delete m_pLeftRuler;
}

GtkWidget* CSignalChannelDisplay::getRulerWidget() const
{
	return m_pLeftRuler->getWidget();
}

GtkWidget* CSignalChannelDisplay::getSignalDisplayWidget() const
{
	return m_pDrawingArea;
}

void CSignalChannelDisplay::onResizeEventCB(gint i32Width, gint i32Height)
{
	m_ui32Width = i32Width;
	m_ui32Height = i32Height;

	updateScale();
}

void CSignalChannelDisplay::updateScale()
{
	uint32 l_ui32SamplesPerBuffer = (uint32)m_pDatabase->m_pDimmensionSizes[1];
	uint64 l_ui64NumberOfBufferToDisplay = m_pDatabase->m_ui64NumberOfBufferToDisplay;
	if(l_ui32SamplesPerBuffer == 1 && l_ui64NumberOfBufferToDisplay != 1)
	{
		l_ui64NumberOfBufferToDisplay--;
	}

	m_f64WidthPerBuffer = static_cast<float64>(m_ui32Width) / static_cast<float64>(l_ui64NumberOfBufferToDisplay);

	m_f64PointStep = 0;
	if( (l_ui32SamplesPerBuffer*l_ui64NumberOfBufferToDisplay) - 1 != 0 )
	{
		m_f64PointStep = static_cast<float64>(m_ui32Width) / ( (l_ui32SamplesPerBuffer*l_ui64NumberOfBufferToDisplay) - 1);
	}

	redrawAllAtNextRefresh();
}

void CSignalChannelDisplay::resetChannelList()
{
	m_oChannelList.clear();
}

void CSignalChannelDisplay::addChannel(uint32 ui32Channel)
{
	m_oChannelList.push_back(ui32Channel);
}

uint64 CSignalChannelDisplay::cropCurve(uint64 ui64PointCount)
{
	if(ui64PointCount == 0)
	{
		return 0;
	}

	//clears the vector of the points to draw
	m_pParentDisplayView->m_pPoints.clear();

	GdkPoint l_oPoint;
	vector< pair< float64, float64> >& l_vCurvePoints = m_pParentDisplayView->m_pRawPoints;

	//for each couple of successive points
	for(size_t i=0 ; i<ui64PointCount-1; i++)
	{
		//get the two points coordinates
		float64 l_f64X0 = l_vCurvePoints[i].first;
		float64 l_f64Y0 = l_vCurvePoints[i].second;
		float64 l_f64X1 = l_vCurvePoints[i+1].first;
		float64 l_f64Y1 = l_vCurvePoints[i+1].second;

		//if one of the point is out of the drawing area
		if(l_f64Y0<0 || l_f64Y0>=m_ui32Height || l_f64Y1<0 || l_f64Y1>=m_ui32Height)
		{
			//computes the line's coefficients
			float64 l_f64A = (l_f64Y1-l_f64Y0)/(l_f64X1-l_f64X0);
			float64 l_f64B = l_f64Y0 - (l_f64A*l_f64X0);

			//if the first point is out of the window
			if(l_f64Y0<0)
			{
				//computes its X-coordinate with the minimum Y
				l_oPoint.x = static_cast<gint>(-l_f64B/l_f64A);
				//we take -1 and not 0, this way, the line between the two successive intersect points won't be drawn
				l_oPoint.y = static_cast<gint>(-1);
				//adds it to the vector
				m_pParentDisplayView->m_pPoints.push_back(l_oPoint);
			}
			else if(l_f64Y0>=m_ui32Height)
			{
				//same with the maximum Y
				l_oPoint.x = static_cast<gint>((m_ui32Height-l_f64B)/l_f64A);
				l_oPoint.y = m_ui32Height;
				m_pParentDisplayView->m_pPoints.push_back(l_oPoint);
			}
			//if it is inside, keep its current coordinates
			else
			{
				l_oPoint.x = static_cast<gint>(l_f64X0);
				l_oPoint.y = static_cast<gint>(l_f64Y0);
				m_pParentDisplayView->m_pPoints.push_back(l_oPoint);
			}

			//if the second point is out of the window, computes its intersect point and adds it
			if(l_f64Y1<0)
			{
				l_oPoint.x = static_cast<gint>(-l_f64B/l_f64A);
				l_oPoint.y = static_cast<gint>(-1);
				m_pParentDisplayView->m_pPoints.push_back(l_oPoint);
			}
			else if(l_f64Y1>=m_ui32Height)
			{
				l_oPoint.x = static_cast<gint>((m_ui32Height-l_f64B)/l_f64A);
				l_oPoint.y = static_cast<gint>(m_ui32Height) ;
				m_pParentDisplayView->m_pPoints.push_back(l_oPoint);
			}
		}
		else //both points lie within the drawing area
		{
			//keep the first point
			l_oPoint.x = static_cast<gint>(l_f64X0);
			l_oPoint.y = static_cast<gint>(l_f64Y0);
			m_pParentDisplayView->m_pPoints.push_back(l_oPoint);

			//add the last point
			if(i == ui64PointCount-2)
			{
				l_oPoint.x = static_cast<gint>(l_f64X1);
				l_oPoint.y = static_cast<gint>(l_f64Y1);
				m_pParentDisplayView->m_pPoints.push_back(l_oPoint);
			}
		}
	}

	//return the number of points to draw
	return m_pParentDisplayView->m_pPoints.size();
}

void CSignalChannelDisplay::getUpdateRectangle(GdkRectangle& rRect)
{
	rRect.y = 0;
	rRect.height = m_ui32Height;

	//if in scroll mode, or if redrawing everything, update the whole drawing area
	if(m_pDatabase->getDisplayMode() == OVP_TypeId_SignalDisplayMode_Scroll || mustRedrawAll() == true)
	{
		rRect.x = 0;
		rRect.width = m_ui32Width;
	}
	else //partial redraw only
	{
		//determine index and position of first buffer to display, and index of first sample to display
		uint32 l_ui32FirstBufferToDisplay = 0;
		uint32 l_ui32FirstSampleToDisplay = 0;
		uint32 l_ui32FirstBufferToDisplayPosition = 0;
		getFirstBufferToDisplayInformation(l_ui32FirstBufferToDisplay, l_ui32FirstSampleToDisplay, l_ui32FirstBufferToDisplayPosition);

		//X position of first sample that will be drawn when channel is refreshed
		float64 l_f64StartX = getSampleXCoordinate(l_ui32FirstBufferToDisplayPosition, l_ui32FirstSampleToDisplay, 0);

		//position on screen of latest buffer
		uint32 l_ui32LatestBufferPosition = l_ui32FirstBufferToDisplayPosition + m_pDatabase->m_oSampleBuffers.size()-1-l_ui32FirstBufferToDisplay;

		//X position of last sample that will be drawn when channel is refreshed
		uint32 l_ui32SamplesPerBuffer = (uint32)m_pDatabase->m_pDimmensionSizes[1];
		float64 l_f64EndX = getSampleXCoordinate(l_ui32LatestBufferPosition, l_ui32SamplesPerBuffer-1, 0);

		rRect.x = (gint)l_f64StartX;
		rRect.width = (gint)l_f64EndX - (gint)l_f64StartX + 1 /* this extra pixel accounts for vertical update line*/+ 1;
	}
}

void CSignalChannelDisplay::redrawAllAtNextRefresh()
{
	m_bRedrawAll = true;
}

boolean CSignalChannelDisplay::mustRedrawAll()
{
	return m_bRedrawAll;
}

void CSignalChannelDisplay::draw(const GdkRectangle& rExposedArea)
{
	//ensure there is data to display
	if(!m_pDatabase || m_pDatabase->m_oSampleBuffers.size()==0)
	{
		return;
	}

	//updates the left ruler
	float64 l_f64MaximumDisplayedValue = m_f64TranslateY - ( (0 - ((m_ui32Height*m_f64ZoomScaleY)/2) + (m_f64ZoomTranslateY* m_f64ZoomScaleY)) / (m_f64ScaleY * m_f64ZoomScaleY * m_ui32Height) );
	float64 l_f64MinimumDisplayedValue = m_f64TranslateY - ( (m_ui32Height - ((m_ui32Height*m_f64ZoomScaleY)/2) + (m_f64ZoomTranslateY* m_f64ZoomScaleY)) / (m_f64ScaleY * m_f64ZoomScaleY * m_ui32Height) );
	m_pLeftRuler->update(l_f64MinimumDisplayedValue,l_f64MaximumDisplayedValue);

	//determine index and position of first (in the sense of leftmost) buffer to display, and index of first sample to display
	uint32 l_ui32SamplesPerBuffer = (uint32)m_pDatabase->m_pDimmensionSizes[1];
	uint32 l_ui32FirstBufferToDisplay = 0;
	uint32 l_ui32FirstSampleToDisplay = 0;
	uint32 l_ui32FirstBufferToDisplayPosition = 0;
	getFirstBufferToDisplayInformation(l_ui32FirstBufferToDisplay, l_ui32FirstSampleToDisplay, l_ui32FirstBufferToDisplayPosition);

	if(m_pDatabase->getDisplayMode() == OVP_TypeId_SignalDisplayMode_Scan && mustRedrawAll() == false)
	{
		//X position of last drawn sample (0 if restarting from left edge)
		float64 l_f64StartX = getSampleXCoordinate(l_ui32FirstBufferToDisplayPosition, l_ui32FirstSampleToDisplay, 0);

		//position on screen of latest buffer
		uint32 l_ui32LatestBufferPosition = l_ui32FirstBufferToDisplayPosition + m_pDatabase->m_oSampleBuffers.size()-1-l_ui32FirstBufferToDisplay;

		//X position of last sample that will be drawn when channel is refreshed
		float64 l_f64EndX = getSampleXCoordinate(l_ui32LatestBufferPosition, l_ui32SamplesPerBuffer-1, 0);

		//make sure exposed area lies within area about to be redrawn
		if(rExposedArea.x < (gint)l_f64StartX ||
			rExposedArea.width-1/*exposed width is 1 pixel larger than asked for*/ > (gint)l_f64EndX - (gint)l_f64StartX + 1 + 1)
		{
			//this means the window was invalidated by an external widget : redraw it all
			m_bRedrawAll = true;
			m_pDatabase->getIndexOfBufferStartingAtTime(m_pParentDisplayView->m_ui64LeftmostDisplayedTime, l_ui32FirstBufferToDisplay);
			l_ui32FirstBufferToDisplayPosition = 0;
		}
		else
		{
			//start drawing from at least one pixel to the left of first sample so that partial redraws connect well together
			uint32 l_ui32StartX = (uint32)l_f64StartX;
			uint32 l_ui32CurX = l_ui32StartX;
			do
			{
				if(l_ui32FirstSampleToDisplay == 0)
				{
					if(l_ui32FirstBufferToDisplayPosition == 0)
					{
						break;
					}

					l_ui32FirstBufferToDisplay--;
					l_ui32FirstBufferToDisplayPosition--;
					l_ui32FirstSampleToDisplay = l_ui32SamplesPerBuffer-1;
				}
				else
				{
					l_ui32FirstSampleToDisplay--;
				}

				l_ui32CurX = (uint32)getSampleXCoordinate(l_ui32FirstBufferToDisplayPosition, l_ui32FirstSampleToDisplay, 0);
			}
			while(l_ui32CurX >= l_ui32StartX);
		}
	}

	//determine start x coord of first buffer to display
	float64 l_f64FirstBufferToDisplayStartX = 0;
	if(m_pDatabase->getDisplayMode() == OVP_TypeId_SignalDisplayMode_Scroll)
	{
		l_f64FirstBufferToDisplayStartX = m_ui32Width - m_pDatabase->m_oSampleBuffers.size() * m_f64WidthPerBuffer;

		if(l_f64FirstBufferToDisplayStartX < 0)
		{
			l_f64FirstBufferToDisplayStartX = 0;
		}
	}
	else
	{
		l_f64FirstBufferToDisplayStartX = getSampleXCoordinate(l_ui32FirstBufferToDisplayPosition, 0, 0);
	}

	uint32 l_ui32LastBufferToDisplay = m_pDatabase->m_oSampleBuffers.size()-1;

	//draw latest signals
	drawSignals(l_ui32FirstBufferToDisplay, l_ui32LastBufferToDisplay, l_ui32FirstSampleToDisplay, l_f64FirstBufferToDisplayStartX);

	//in scan mode, there is more to be drawn
	if(m_pDatabase->getDisplayMode() == OVP_TypeId_SignalDisplayMode_Scan)
	{
		//draw progress line
		drawProgressLine(l_ui32FirstBufferToDisplay, l_ui32FirstBufferToDisplayPosition);

		//if redrawing the whole window
		if(m_bRedrawAll == true && l_ui32FirstBufferToDisplay > 0)
		{
			//get start x coord of first buffer after the most recent one
			l_f64FirstBufferToDisplayStartX = getSampleXCoordinate(
				l_ui32LastBufferToDisplay - l_ui32FirstBufferToDisplay + 1,
				0,
				l_f64FirstBufferToDisplayStartX);

			//draw older signals (to the right of progress line)
			drawSignals(0, l_ui32FirstBufferToDisplay-1, 0, l_f64FirstBufferToDisplayStartX);
		}
	}

	//draw Y=0 line
	drawZeroLine();

	//update time of latest displayed data
	m_ui64LatestDisplayedTime = m_pDatabase->m_oEndTime.back();

	//reset redraw all flag
	m_bRedrawAll = false;
}

void CSignalChannelDisplay::computeZoom(OpenViBE::boolean bZoomIn, OpenViBE::float64 f64XClick, OpenViBE::float64 f64YClick)
{
	if(bZoomIn)
	{
		m_f64ZoomTranslateX += (f64XClick - (m_ui32Width/ (m_f64ZoomFactor * 2))) / m_f64ZoomScaleX;
		m_f64ZoomTranslateY += (f64YClick - (m_ui32Height/ (m_f64ZoomFactor * 2))) / m_f64ZoomScaleY;

		m_f64ZoomScaleX *= m_f64ZoomFactor;
		m_f64ZoomScaleY *= m_f64ZoomFactor;
	}
	else
	{
		m_f64ZoomScaleX /= m_f64ZoomFactor;
		m_f64ZoomScaleY /= m_f64ZoomFactor;

		if(fabs(m_f64ZoomScaleY - 1) < 0.001)
		{
			m_f64ZoomScaleX = m_f64ZoomScaleY = 1.;
		}

		m_f64ZoomTranslateX -= (f64XClick - (m_ui32Width/ (m_f64ZoomFactor * 2))) / m_f64ZoomScaleX;
		m_f64ZoomTranslateY -= (f64YClick - (m_ui32Height/ (m_f64ZoomFactor * 2))) / m_f64ZoomScaleY;

		if(fabs(m_f64ZoomTranslateY) < 0.001)
		{
			m_f64ZoomTranslateX = m_f64ZoomTranslateY = 1.;
		}
	}

	//check if we are out of the window
	if(m_f64ZoomTranslateX < 0)
	{
		m_f64ZoomTranslateX=0;
	}
	if(m_f64ZoomTranslateY < 0)
	{
		m_f64ZoomTranslateY=0;
	}
	if(m_f64ZoomTranslateX > m_ui32Width - (m_ui32Width/ m_f64ZoomScaleX) )
	{
		m_f64ZoomTranslateX = m_ui32Width - (m_ui32Width/ m_f64ZoomScaleX);
	}
	if(m_f64ZoomTranslateY > m_ui32Height - (m_ui32Height/ m_f64ZoomScaleY) )
	{
		m_f64ZoomTranslateY = m_ui32Height - (m_ui32Height/ m_f64ZoomScaleY);
	}
}

void CSignalChannelDisplay::checkTranslation(float64& rDisplayedValueRange)
{
	//update maximum and minimum values displayed by this channel
	m_f64LocalMaximum = -DBL_MAX;
	m_f64LocalMinimum = +DBL_MAX;
	float64 l_f64CurrentMaximum;
	float64 l_f64CurrentMinimum;

	for(size_t k=0 ; k<m_oChannelList.size() ; k++)
	{
		//get local min/max
		m_pDatabase->getDisplayedChannelLocalMinMaxValue(m_oChannelList[k], l_f64CurrentMinimum, l_f64CurrentMaximum);

		if(l_f64CurrentMinimum < m_f64LocalMinimum)
		{
			m_f64LocalMinimum = l_f64CurrentMinimum;
		}
		if(l_f64CurrentMaximum > m_f64LocalMaximum)
		{
			m_f64LocalMaximum = l_f64CurrentMaximum;
		}
	}

	//set parameter to recomputed range
	rDisplayedValueRange = m_f64LocalMaximum - m_f64LocalMinimum;

	//in global best fit mode, translate signals if necessary
	if(m_eCurrentSignalMode == DisplayMode_GlobalBestFit)
	{
		//compute Y coord of local max and min
		gint l_iMaxY = (gint)getSampleYCoordinate(m_f64LocalMaximum);
		gint l_iMinY = (gint)getSampleYCoordinate(m_f64LocalMinimum);

		//translate signal if some data is plotted out of the window
		if(l_iMaxY < 0 || l_iMinY > (int32)m_ui32Height-1)
		{
			m_f64TranslateY = (m_f64LocalMaximum + m_f64LocalMinimum) / 2;

			//reflect changes
			redrawAllAtNextRefresh();
		}
	}
}

void CSignalChannelDisplay::setGlobalBestFitParameters(const float64& rRange, const float64& rMargin)
{
	m_f64ScaleX = 1;

	float64 l_f64LocalMediumValue = (m_f64LocalMaximum + m_f64LocalMinimum) / 2;

	m_f64MaximumTopMargin = l_f64LocalMediumValue + rRange/2 + rMargin;
	m_f64MaximumBottomMargin = l_f64LocalMediumValue + rRange/2 - rMargin;

	m_f64MinimumTopMargin = l_f64LocalMediumValue - rRange/2 + rMargin;
	m_f64MinimumBottomMargin = l_f64LocalMediumValue - rRange/2 - rMargin;

	if(m_eCurrentSignalMode == DisplayMode_GlobalBestFit)
	{
		updateDisplayParameters();
	}
}

void CSignalChannelDisplay::updateDisplayParameters()
{
	if(m_f64MaximumTopMargin == m_f64MinimumBottomMargin)
	{
		m_f64ScaleY = 1;
	}
	else
	{
		m_f64ScaleY =  1 / (m_f64MaximumTopMargin - m_f64MinimumBottomMargin);
	}

	//compute the translation needed to center the signal correctly in the window
	m_f64TranslateX = 0;
	m_f64TranslateY =  (m_f64MaximumTopMargin + m_f64MinimumBottomMargin) / 2;

	//reflect changes
	redrawAllAtNextRefresh();
}

void CSignalChannelDisplay::getFirstBufferToDisplayInformation(uint32& rFirstBufferToDisplay, uint32& rFirstSampleToDisplay, uint32& rFirstBufferToDisplayPosition)
{
	rFirstBufferToDisplay = 0;
	rFirstSampleToDisplay = 0;
	rFirstBufferToDisplayPosition = 0;

	if(m_pDatabase->getDisplayMode() == OVP_TypeId_SignalDisplayMode_Scan)
	{
		if(m_bRedrawAll == true)
		{
			m_pDatabase->getIndexOfBufferStartingAtTime(m_pParentDisplayView->m_ui64LeftmostDisplayedTime, rFirstBufferToDisplay);
		}
		else if(m_pDatabase->getIndexOfBufferStartingAtTime(m_ui64LatestDisplayedTime, rFirstBufferToDisplay) == false)
		{
			//chances are drawing is up to date and this call was triggered following an "external" expose event
			//(e.g. the window was covered by a widget which was just moved, resulting in an expose event)
			//=>let's redraw the whole window
			m_pDatabase->getIndexOfBufferStartingAtTime(m_pParentDisplayView->m_ui64LeftmostDisplayedTime, rFirstBufferToDisplay);
			m_bRedrawAll = true;
		}
		else //partial redraw
		{
			uint32 l_ui32LeftmostBufferIndex = 0;
			m_pDatabase->getIndexOfBufferStartingAtTime(m_pParentDisplayView->m_ui64LeftmostDisplayedTime, l_ui32LeftmostBufferIndex);

			//get position of first new buffer
			rFirstBufferToDisplayPosition = rFirstBufferToDisplay - l_ui32LeftmostBufferIndex;

			//redraw from last sample of last drawn buffer, if we're not restarting from left edge
			if(rFirstBufferToDisplayPosition > 0)
			{
				rFirstBufferToDisplay--;
				rFirstBufferToDisplayPosition--;
				rFirstSampleToDisplay = (uint32)m_pDatabase->m_pDimmensionSizes[1] - 1;
			}
		}
	}
}

int32 CSignalChannelDisplay::getBufferStartX(uint32 ui32Position)
{
	return static_cast<int32>((ui32Position*m_f64WidthPerBuffer - m_f64TranslateX) * m_f64ScaleX);
}

float64 CSignalChannelDisplay::getSampleXCoordinate(uint32 ui32BufferPosition, uint32 ui32SampleIndex, float64 f64XOffset)
{
	return (f64XOffset + ui32BufferPosition*m_f64WidthPerBuffer + ui32SampleIndex*m_f64PointStep - m_f64TranslateX) * m_f64ScaleX;
}

float64 CSignalChannelDisplay::getSampleYCoordinate(float64 f64Value)
{
	//TODO : precompute some factors!
	return m_f64ScaleY*m_f64ZoomScaleY*m_ui32Height* (m_f64TranslateY-f64Value) + (m_ui32Height*m_f64ZoomScaleY)/2 - m_f64ZoomTranslateY*m_f64ZoomScaleY;
}

void CSignalChannelDisplay::drawSignals(uint32 ui32FirstBufferToDisplay, uint32 ui32LastBufferToDisplay, uint32 ui32FirstSampleToDisplay, float64 f64FirstBufferStartX)
{
	//compute and draw sample points
	uint32 l_ui32SamplesPerBuffer = (uint32)m_pDatabase->m_pDimmensionSizes[1];

	GdkColor l_oLineColor;

	for(size_t k=0; k<m_oChannelList.size(); k++)
	{
		if(m_oChannelList.size()!=1)
		{
			m_pParentDisplayView->getMultiViewColor(m_oChannelList[k], l_oLineColor);
		}
		else
		{
			if(m_eCurrentSignalMode != DisplayMode_GlobalBestFit)
			{
				l_oLineColor.red = 65535;
				l_oLineColor.green = 0;
				l_oLineColor.blue = 0;
			}
			else
			{
				l_oLineColor.red = 0;
				l_oLineColor.green = 0;
				l_oLineColor.blue = 0;
			}
		}
		gdk_gc_set_rgb_fg_color(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)], &l_oLineColor);

		size_t l_ui64PointIndex = 0;

		for(size_t j=ui32FirstBufferToDisplay; j<=ui32LastBufferToDisplay; j++)
		{
			const float64 * l_pCurrentChannelSampleBuffer = (m_pDatabase->m_oSampleBuffers)[j] + (m_oChannelList[k] * l_ui32SamplesPerBuffer);

			//for all samples in current buffer
			for(uint32 i = (j==ui32FirstBufferToDisplay)?ui32FirstSampleToDisplay:0; i<l_ui32SamplesPerBuffer; i++, l_ui64PointIndex++)
			{
				(m_pParentDisplayView->m_pRawPoints)[l_ui64PointIndex].first = getSampleXCoordinate(j - ui32FirstBufferToDisplay, i, f64FirstBufferStartX);
				(m_pParentDisplayView->m_pRawPoints)[l_ui64PointIndex].second = getSampleYCoordinate(l_pCurrentChannelSampleBuffer[i]);
			}
		}

		//crop points
		uint64 l_ui64NumberOfPointsToDisplay = cropCurve(l_ui64PointIndex);

		if(l_ui64NumberOfPointsToDisplay != 0)
		{
			//draw all the points and link them
			gdk_draw_lines(m_pDrawingArea->window, m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE (m_pDrawingArea)], &m_pParentDisplayView->m_pPoints[0], (gint)l_ui64NumberOfPointsToDisplay);
		}
	}

	if(m_pDatabase->m_oStimulations.size() != 0)
	{
		//switch to dashed line
		gdk_gc_set_line_attributes(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE (m_pDrawingArea)], 1, GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_BEVEL);

		//compute current time window start and end time
#if 0
		uint64 l_ui64StartTime = m_pDatabase->m_oStartTime[ui32FirstBufferToDisplay] +	m_pDatabase->m_ui64BufferDuration * ui32FirstSampleToDisplay / l_ui32SamplesPerBuffer;
		uint64 l_ui64EndTime = m_pDatabase->m_oStartTime[ui32LastBufferToDisplay] + m_pDatabase->m_ui64BufferDuration;
#else
		uint64 l_ui64FirstBufferDuration = m_pDatabase->m_oEndTime[ui32FirstBufferToDisplay] - m_pDatabase->m_oStartTime[ui32FirstBufferToDisplay];
		uint64 l_ui64LastBufferDuration = m_pDatabase->m_oEndTime[ui32LastBufferToDisplay] - m_pDatabase->m_oStartTime[ui32LastBufferToDisplay];
		uint64 l_ui64StartTime = m_pDatabase->m_oStartTime[ui32FirstBufferToDisplay] + l_ui64FirstBufferDuration * ui32FirstSampleToDisplay / l_ui32SamplesPerBuffer;
		uint64 l_ui64EndTime = m_pDatabase->m_oStartTime[ui32LastBufferToDisplay] + l_ui64LastBufferDuration;
#endif

		//draw stimulations
		std::deque<std::pair<uint64, uint64> >::iterator it;
		for(it=m_pDatabase->m_oStimulations.begin(); it!=m_pDatabase->m_oStimulations.end(); it++)
		{
			//look for stimulations lying in current time window
			if(it->first >= l_ui64StartTime && it->first <= l_ui64EndTime)
			{
				uint32 j = ui32FirstBufferToDisplay;
				while(it->first > m_pDatabase->m_oEndTime[j])
				{
					j++;
				}

				m_pParentDisplayView->getStimulationColor(it->second, l_oLineColor);
				gdk_gc_set_rgb_fg_color(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)], &l_oLineColor);

#if 0
				uint32 i = (uint32)((it->first - m_pDatabase->m_oStartTime[j]) * l_ui32SamplesPerBuffer / m_pDatabase->m_ui64BufferDuration);
#else
				uint64 l_ui64StimBufferDuration = m_pDatabase->m_oEndTime[j] - m_pDatabase->m_oStartTime[j];
				uint32 i = (uint32)((it->first - m_pDatabase->m_oStartTime[j]) * l_ui32SamplesPerBuffer / l_ui64StimBufferDuration);
#endif
				uint32 l_ui32StimulationX = (uint32)getSampleXCoordinate(j - ui32FirstBufferToDisplay, i, f64FirstBufferStartX);
				gdk_draw_line(m_pDrawingArea->window, m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE (m_pDrawingArea)], l_ui32StimulationX, 0, l_ui32StimulationX, m_ui32Height);
			}
		}
	}

	l_oLineColor.red = 0;
	l_oLineColor.green = 0;
	l_oLineColor.blue = 0;
	gdk_gc_set_rgb_fg_color(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)], &l_oLineColor);
	gdk_gc_set_line_attributes(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE (m_pDrawingArea)], 1, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_BEVEL);
}

void CSignalChannelDisplay::drawProgressLine(uint32 ui32FirstBufferToDisplay, uint32 ui32FirstBufferToDisplayPosition)
{
	//draw line only if there's more data to be drawn after it
	if(m_pDatabase->m_oSampleBuffers.size() < m_pDatabase->m_ui64NumberOfBufferToDisplay ||
		m_pParentDisplayView->m_ui64LeftmostDisplayedTime > m_pDatabase->m_oStartTime[0])
	{
		uint32 l_ui32SamplesPerBuffer = (uint32)m_pDatabase->m_pDimmensionSizes[1];

		//position on screen of latest buffer
		uint32 l_ui32LatestBufferPosition = ui32FirstBufferToDisplayPosition + m_pDatabase->m_oSampleBuffers.size()-1-ui32FirstBufferToDisplay;

		//X position of last sample that will be drawn when channel is refreshed
		float64 l_f64EndX = getSampleXCoordinate(l_ui32LatestBufferPosition, l_ui32SamplesPerBuffer-1, 0);

		GdkColor l_oLineColor;
		l_oLineColor.red = 0*65535/255; l_oLineColor.green = 255*65535/255; l_oLineColor.blue = 0*65535/255;
		gdk_gc_set_rgb_fg_color(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)], &l_oLineColor);

		//draw line one pixel after last sample
		gdk_draw_line(m_pDrawingArea->window, m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE (m_pDrawingArea)],
			(uint32)l_f64EndX + 1, 0,
			(uint32)l_f64EndX + 1, m_ui32Height-1);

		GdkColor l_oBlack;
		l_oBlack.red = 0; l_oBlack.green = 0; l_oBlack.blue = 0;
		gdk_gc_set_rgb_fg_color(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)], &l_oBlack);
	}
}

void CSignalChannelDisplay::drawZeroLine()
{
	//switch to dashed line
	gdk_gc_set_line_attributes(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE (m_pDrawingArea)], 1, GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_BEVEL);

	//draw Y=0 line
	gint l_iZeroY = (gint)getSampleYCoordinate(0);
	gdk_draw_line(m_pDrawingArea->window, m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE (m_pDrawingArea)], 0, l_iZeroY, m_ui32Width ,l_iZeroY);

	//switch back to normal line
	gdk_gc_set_line_attributes(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE (m_pDrawingArea)], 1, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_BEVEL);
}

//
//CALLBACKS
//

gboolean drawingAreaExposeEventCallback(GtkWidget *widget, GdkEventExpose *pEvent, gpointer data)
{
	CSignalChannelDisplay * m_pChannelDisplay = reinterpret_cast<CSignalChannelDisplay*>(data);

	//check if a full redrawn was asked for
	if(pEvent->area.width == (gint)m_pChannelDisplay->m_ui32Width && pEvent->area.height == (gint)m_pChannelDisplay->m_ui32Height)
	{
		m_pChannelDisplay->redrawAllAtNextRefresh();
	}

	//redraw signals
	m_pChannelDisplay->draw(pEvent->area);

	//don't propagate this signal to the children if any
	return TRUE;
}

gboolean drawingAreaResizeEventCallback(GtkWidget* pWidget, GtkAllocation* pAllocation, gpointer data)
{
	CSignalChannelDisplay * m_pChannelDisplay = reinterpret_cast<CSignalChannelDisplay*>(data);
	m_pChannelDisplay->onResizeEventCB(pAllocation->width, pAllocation->height);
	return FALSE;
}

void drawingAreaClickedEventCallback(GtkWidget *widget, GdkEventButton *pEvent, gpointer data)
{
	if(pEvent->type != GDK_BUTTON_PRESS)
	{
		return;
	}

	CSignalChannelDisplay * m_pChannelDisplay = reinterpret_cast<CSignalChannelDisplay*>(data);
	OpenViBE::boolean l_bZoomChanged = false;
	m_pChannelDisplay->m_eCurrentSignalMode = DisplayMode_GlobalBestFit;

	if(pEvent->button == 1)
	{
		m_pChannelDisplay->m_eCurrentSignalMode = DisplayMode_ZoomIn;
		m_pChannelDisplay->computeZoom(true, pEvent->x, pEvent->y);
		l_bZoomChanged = true;
	}
	else if(pEvent->button == 3)
	{
		if(m_pChannelDisplay->m_f64ZoomScaleY != 1.0)
		{
			m_pChannelDisplay->m_eCurrentSignalMode = DisplayMode_ZoomOut;
			m_pChannelDisplay->computeZoom(false, pEvent->x, pEvent->y);
			l_bZoomChanged = true;

			if(m_pChannelDisplay->m_f64ZoomScaleY == 1.0)
			{
				m_pChannelDisplay->m_eCurrentSignalMode = DisplayMode_GlobalBestFit;
				m_pChannelDisplay->updateDisplayParameters();
			}
			else
			{
				m_pChannelDisplay->m_eCurrentSignalMode = DisplayMode_ZoomOut;
			}
		}
	}

	//if the zoom level has changed, redraw the signal and left ruler
	if(l_bZoomChanged)
	{
		m_pChannelDisplay->redrawAllAtNextRefresh();
		if(GTK_WIDGET(m_pChannelDisplay->m_pDrawingArea)->window) gdk_window_invalidate_rect(GTK_WIDGET(m_pChannelDisplay->m_pDrawingArea)->window, NULL, true);
		if(GTK_WIDGET(m_pChannelDisplay->m_pLeftRuler->getWidget())->window) gdk_window_invalidate_rect(GTK_WIDGET(m_pChannelDisplay->m_pLeftRuler->getWidget())->window, NULL, true);
	}
}

void drawingAreaEnterEventCallback(GtkWidget *widget, GdkEventCrossing *event, gpointer data)
{
	CSignalChannelDisplay * m_pChannelDisplay = reinterpret_cast<CSignalChannelDisplay*>(data);

	//change the cursor to the zooming one
	gdk_window_set_cursor(widget->window, m_pChannelDisplay->m_pParentDisplayView->m_pCursor[1]);
}

void drawingAreaLeaveEventCallback(GtkWidget *widget, GdkEventCrossing *event, gpointer data)
{
	CSignalChannelDisplay * m_pChannelDisplay = reinterpret_cast<CSignalChannelDisplay*>(data);

	//change the cursor back to the normal one
	gdk_window_set_cursor(widget->window, m_pChannelDisplay->m_pParentDisplayView->m_pCursor[0]);
}
