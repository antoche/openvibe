#ifndef __OpenViBEPlugins_SimpleVisualisation_CPowerSpectrumChannelDisplay_H__
#define __OpenViBEPlugins_SimpleVisualisation_CPowerSpectrumChannelDisplay_H__

#include "ovpCPowerSpectrumDatabase.h"

#include <openvibe/ov_all.h>
#include <gtk/gtk.h>
#include <iostream>
using namespace std;

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		class CPowerSpectrumDisplayView;

		/**
		 * Displays power spectrum of a given channel.
		 */
		class CPowerSpectrumChannelDisplay
		{
		public:
			/**
			 * \brief Constructor
			 * \param pParentDisplay Pointer to object managing power spectrum widgets
			 * \param ui32Channel Index of channel to display
			 * \param pDatabase Object holding signal data
			 * \param i32ChannelDisplayWidthRequest Width to be requested by widget
			 * \param i32ChannelDisplayHeightRequest Height to be requested by widget
			 * \param i32LeftRulerWidthRequest Width to be requested by left ruler
			 * \param i32LeftRulerHeightRequest Height to be requested by left ruler
			 */
			CPowerSpectrumChannelDisplay(
				CPowerSpectrumDisplayView* pParentDisplay,
				OpenViBE::uint32 ui32Channel,
				CPowerSpectrumDatabase& pDatabase,
				OpenViBE::int32 i32ChannelDisplayWidthRequest,
				OpenViBE::int32 i32ChannelDisplayHeightRequest,
				OpenViBE::int32 i32LeftRulerWidthRequest,
				OpenViBE::int32 i32LeftRulerHeightRequest);

			/**
			 * \brief Destructor
			 */
			~CPowerSpectrumChannelDisplay();

			/**
			 * \brief Get channel top (main) widget
			 * \return Pointer to top GtkWidget
			 */
			::GtkWidget* getTopWidget() const;

			/**
			 * \brief Get spectrum display widget
			 * \return Pointer to spectrum display widget
			 */
			GtkWidget* getSpectrumDisplayWidget() const;

			/**
			 * \brief Toggle channel visibility
			 * \param bActive Visibility flag
			 */
			void toggle(
				OpenViBE::boolean bActive);

			/**
			 * \brief Set left ruler visibility
			 * \param bActive Visibility flag
			 */
			void toggleLeftRuler(
				OpenViBE::boolean bActive);

			/**
			 * \brief Draw channel spectrum
			 */
			void drawSpectrum();

			/**
			 * \brief Draws left ruler (amplitude scale)
			 */
			void drawLeftRuler();

			/**
			 * \brief Draws bottom ruler (frequency scale)
			 */
			void drawBottomRuler();

			/**
			 * \brief Compute spectrum
			 */
			void update();

			/**
			 * \brief Resize RGB buffer of this channel
			 * \param ui32Width Buffer width
			 * \param ui32Height Buffer height
			 */
			void resizeRGBBuffer(
				OpenViBE::uint32 ui32Width,
				OpenViBE::uint32 ui32Height);

			/**
			 * \brief Draw a box in a RGB buffer
			 * \param ui32X Box left coordinate
			 * \param ui32Y	Box top coordinate
			 * \param ui32Width Box width
			 * \param ui32Height Box height
			 * \param ui8Red Red component of box color
			 * \param ui8Green Green component of box color
			 * \param ui8Blue Blue component of box color
			 */
			void drawBoxToBuffer(
				OpenViBE::uint32 ui32X,
				OpenViBE::uint32 ui32Y,
				OpenViBE::uint32 ui32Width,
				OpenViBE::uint32 ui32Height,
				OpenViBE::uint8 ui8Red,
				OpenViBE::uint8 ui8Green,
				OpenViBE::uint8 ui8Blue);

		private:
			//Parent plugin
			CPowerSpectrumDisplayView* m_pParentDisplay;
			//Table containing left ruler and spectrum drawing area
			GtkTable* m_pWidgetTable;
			//Drawing area where the spectrum is to be drawn
			GtkWidget* m_pDisplay;
			//Drawing area for amplitudes scale
			GtkWidget* m_pLeftRuler;
			//Gdk graphics context
			GdkGC* m_pGraphicsContext;
			//Power spectrum database
			CPowerSpectrumDatabase* m_pDatabase;
			//Channel index
			OpenViBE::uint32 m_ui32Channel;
			//RGB buffer storing current spectrum
			guchar * m_pRGBBuffer;
			//RGB buffer width
			OpenViBE::uint32 m_ui32RGBBufferWidth;
			//RGB buffer height
			OpenViBE::uint32 m_ui32RGBBufferHeight;
			//RGB buffer row stride
			OpenViBE::uint32 m_ui32Rowstride;
		};
	};
};

#endif
