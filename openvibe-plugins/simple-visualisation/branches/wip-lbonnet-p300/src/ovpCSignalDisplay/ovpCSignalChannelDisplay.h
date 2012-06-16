#ifndef __OpenViBEPlugins_SimpleVisualisation_CSignalChannelDisplay_H__
#define __OpenViBEPlugins_SimpleVisualisation_CSignalChannelDisplay_H__

#include "../ovp_defines.h"
#include "ovpCSignalDisplayLeftRuler.h"

#include <glib.h>
#include <glib/gprintf.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <openvibe/ov_all.h>

#include <openvibe-toolkit/ovtk_all.h>

#include <memory.h>
#include <cmath>

#include <vector>

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{

	class CSignalDisplayView;
	class CBufferDatabase;

	class CSignalChannelDisplay
	{
	public:
		/**
		 * \brief Constructor
		 * \param pDisplayView Parent view
		 * \param i32ChannelDisplayWidthRequest Width to be requested by widget
		 * \param i32ChannelDisplayHeightRequest Height to be requested by widget
		 * \param i32LeftRulerWidthRequest Width to be requested by left ruler
		 * \param i32LeftRulerHeightRequest Height to be requested by left ruler
		 */
		CSignalChannelDisplay(
			CSignalDisplayView* pDisplayView,
			OpenViBE::int32 i32ChannelDisplayWidthRequest,
			OpenViBE::int32 i32ChannelDisplayHeightRequest,
			OpenViBE::int32 i32LeftRulerWidthRequest,
			OpenViBE::int32 i32LeftRulerHeightRequest);

		/**
		 * \brief Destructor
		 */
		~CSignalChannelDisplay();

		/**
		 * \brief Get ruler widget
		 * \return Pointer to ruler widget
		 */
		::GtkWidget* getRulerWidget() const;

		/**
		 * \brief Get signal display widget
		 * \return Pointer to signal display widget
		 */
		::GtkWidget* getSignalDisplayWidget() const;

		/**
		 * \brief Callback notified upon resize events
		 * \param i32Width New window width
		 * \param i32Height New window height
		 */
		void onResizeEventCB(
			gint i32Width,
			gint i32Height);

		/**
		 * \brief Updates scale following a resize event or a time scale change
		 */
		void updateScale();

		/**
		 * \brief Reset list of channels displayed by this object
		 */
		void resetChannelList();

		/**
		 * \brief Add a channel to the list of channels to be displayed
		 * \param ui32Channel Index of channel to be displayed
		 */
		void addChannel(
			OpenViBE::uint32 ui32Channel);

		/**
		 * \brief Get rectangle to clear and redraw based on latest signal data received
		 * \param[out] rRect Rectangle holding part of drawing area to clear and update
		 */
		void getUpdateRectangle(
			GdkRectangle& rRect);

		/**
		 * \brief Flag widget so that its whole window is redrawn at next refresh
	   */
		void redrawAllAtNextRefresh();

		/**
		 * \brief Check whether the whole window must be redrawn
		 * \return True if the whole window must be redrawn, false otherwise
		 */
		OpenViBE::boolean mustRedrawAll();

		/**
		 * \brief Draws the signal on the signal's drawing area.
		 * \param rExposedArea Exposed area that needs to be redrawn
		 */
		void draw(
			const GdkRectangle& rExposedArea);

		/**
		 * \brief Clip signals to drawing area
		 * Computes the list of points used to draw the lines (m_pParentDisplayView->m_pPoints) using the raw points list
		 * (m_pParentDisplayView->m_pRawPoints) and by cropping the lines when they go out of the window.
		 * \param ui64PointCount Number of points to clip
		 * \return The number of points to display.
		 */
		OpenViBE::uint64 cropCurve(
			OpenViBE::uint64 ui64PointCount);

		/**
		 * \brief Computes the parameters necessary for the signal to be zoomed at the selected coordinates.
		 * \param bZoomIn If true, the operation is a zoom In, if false it's a zoom Out.
		 * \param f64XClick The X-coordinate of the center of the area we want to zoom in.
		 * \param f64YClick The Y-coordinate of the center of the area we want to zoom in.
		 */
		void computeZoom(
			OpenViBE::boolean bZoomIn,
			OpenViBE::float64 f64XClick,
			OpenViBE::float64 f64YClick);

		/**
		 * \brief Recomputes value range and changes signal translation if needed
		 * In global best fit mode, recomputes translation so that signals fit in the drawing area.
		 * If the value range has changed too much, the scale will be recomputed at next redraw.
		 * \param[out] rDisplayedValueRange Returns updated value range displayed by this channel
		 */
		void checkTranslation(
			OpenViBE::float64& rDisplayedValueRange);

		/**
		 * \brief Sets latest global best fit parameters
		 * Channel display parameters need to be updated based on this information.
		 * The visible range is recomputed so that :
		 * maxVisibleValue = channelMiddleValue + rRange/2 + rMargin
		 * minVisibleValue = channelMiddleValue - rRange/2 - rMargin
		 * The margin ensures signals are not redrawn every time, allowing
		 * for some extra top and bottom space in case signals increase/decrease.
		 * \param rRange New global value range
		 * \param rMargin New global value margin
		 */
		void setGlobalBestFitParameters(
			const OpenViBE::float64& rRange,
			const OpenViBE::float64& rMargin);

		/**
		 * \brief Updates signal scale and translation based on latest global range and margin
		 * Called from setGlobalBestFitParameters when in global best fit mode, and when
		 * leaving zooming mode.
		 */
		void updateDisplayParameters();

	private:
		/**
		 * \brief Get first buffer to display index and position and first sample to display index
		 * \param[out] rFirstBufferToDisplay Index of first buffer to display
		 * \param[out] rFirstSampleToDisplay Index of first sample to display
		 * \param[out] rFirstBufferToDisplayPosition Position of first buffer to display (0-based, from left edge)
		 */
		void getFirstBufferToDisplayInformation(
			OpenViBE::uint32& rFirstBufferToDisplay,
			OpenViBE::uint32& rFirstSampleToDisplay,
			OpenViBE::uint32& rFirstBufferToDisplayPosition);

		/**
		 * \brief Get start X coord of a buffer
		 * \param ui32Position Position of buffer on screen (0-based, from left edge)
		 * \return Floored X coordinate of buffer
		 */
		OpenViBE::int32 getBufferStartX(
			OpenViBE::uint32 ui32Position);

		/**
		 * \brief Get X coordinate of a sample
		 * \param ui32BufferPosition Position of buffer on screen (0-based, from left edge)
		 * \param ui32SampleIndex Index of sample in buffer
		 * \param f64XOffset X offset from which to start drawing. Used in scroll mode only.
		 * \return X coordinate of sample
		 */
		OpenViBE::float64 getSampleXCoordinate(
			OpenViBE::uint32 ui32BufferPosition,
			OpenViBE::uint32 ui32SampleIndex,
			OpenViBE::float64 f64XOffset);

		/**
		 * \brief Get Y coordinate of a sample
		 * \param f64Value Sample value
		 * \return Y coordinate of sample
		 */
		OpenViBE::float64 getSampleYCoordinate(
			OpenViBE::float64 f64Value);

		/**
		 * \brief Draw signals (and stimulations, if any) displayed by this channel
		 * \param ui32FirstBufferToDisplay Index of first buffer to display
		 * \param ui32LastBufferToDisplay Index of last buffer to display
		 * \param ui32FirstSampleToDisplay Index of first sample to display in first buffer (subsequent buffers will start at sample 0)
		 * \param f64BaseX X offset to apply to signals (can be non null in scroll mode)
		 */
		void drawSignals(
			OpenViBE::uint32 ui32FirstBufferToDisplay,
			OpenViBE::uint32 ui32LastBufferToDisplay,
			OpenViBE::uint32 ui32FirstSampleToDisplay,
			OpenViBE::float64 f64FirstBufferStartX);

		/**
		 * \brief Draw vertical line highlighting where data was last drawn
		 * \param ui32X X coordinate of line
		 */
		void drawProgressLine(
			OpenViBE::uint32 ui32FirstBufferToDisplay,
			OpenViBE::uint32 ui32FirstBufferToDisplayPosition);

		/**
		 * \brief Draw Y=0 line
		 */
		void drawZeroLine();

	public:
		//! Left ruler displaying signal scale
		CSignalDisplayLeftRuler* m_pLeftRuler;
		//! The drawing area where the signal is to be drawn
		GtkWidget * m_pDrawingArea;
		//! Drawing area dimensions, in pixels
		OpenViBE::uint32 m_ui32Width, m_ui32Height;
		//! Available width per buffer, in pixels
		OpenViBE::float64 m_f64WidthPerBuffer;
		//! Available width per sample point, in pixels
		OpenViBE::float64 m_f64PointStep;
		//! The index list of the channels to display
		std::vector<OpenViBE::uint32> m_oChannelList;
		//! The "parent" view (which uses this widget)
		CSignalDisplayView * m_pParentDisplayView;
		//! The database from which the information are to be read
		CBufferDatabase * m_pDatabase;

		/** \ name Extrema of displayed values for this channel */
		//@{
		OpenViBE::float64 m_f64LocalMaximum;
		OpenViBE::float64 m_f64LocalMinimum;
		//@}

		/** \name Auto scaling parameters */
		//@{
		OpenViBE::float64 m_f64ScaleX;
		OpenViBE::float64 m_f64ScaleY;
		OpenViBE::float64 m_f64TranslateX;
		OpenViBE::float64 m_f64TranslateY;
		//@}

		/** \name Zooming parameters (user controlled) */
		//@{
		OpenViBE::float64 m_f64ZoomTranslateX;
		OpenViBE::float64 m_f64ZoomTranslateY;
		OpenViBE::float64 m_f64ZoomScaleX;
		OpenViBE::float64 m_f64ZoomScaleY;
		//! The zoom factor step
		const OpenViBE::float64 m_f64ZoomFactor;
		//@}

		/** \name Scale margin parameters */
		//@{
		OpenViBE::float64 m_f64MaximumTopMargin;
		OpenViBE::float64 m_f64MaximumBottomMargin;
		OpenViBE::float64 m_f64MinimumTopMargin;
		OpenViBE::float64 m_f64MinimumBottomMargin;
		//@}

		//! Current signal display mode
		OpenViBEPlugins::SimpleVisualisation::EDisplayMode m_eCurrentSignalMode;
		//! Time of latest displayed data
		OpenViBE::uint64 m_ui64LatestDisplayedTime;
		//! Should the whole window be redrawn at next redraw?
		OpenViBE::boolean m_bRedrawAll;
	};

	}
}

#endif
