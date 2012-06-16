#ifndef __OpenViBEPlugins_SimpleVisualisation_CTimeFrequencyMapChannelDisplay_H__
#define __OpenViBEPlugins_SimpleVisualisation_CTimeFrequencyMapChannelDisplay_H__

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <gtk/gtk.h>
#include <iostream>

#include "../ovpCSpectrumDatabase.h"

using namespace std;

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		/**
		 * Displays TF map for a given channel.
		 * Spectrum data is retrieved from the CSpectrumDatabase object.
		 */
		class CTimeFrequencyMapChannelDisplay
		{
		public:
			/**
			 * \brief Constructor
			 * \param pDatabase Object holding signal data
			 * \param ui32Channel Index of channel to display
			 * \param i32ChannelDisplayWidthRequest Width to be requested by widget
			 * \param i32ChannelDisplayHeightRequest Height to be requested by widget
			 * \param i32LeftRulerWidthRequest Width to be requested by left ruler
			 * \param i32LeftRulerHeightRequest Height to be requested by left ruler
			 */
			CTimeFrequencyMapChannelDisplay(
				CSpectrumDatabase& rSpectrumDatabase,
				OpenViBE::uint32 ui32Channel,
				OpenViBE::float64 f64Attenuation,
				OpenViBE::float64 f64MinimumDisplayedFrequency,
				OpenViBE::float64 f64MaximumDisplayedFrequency,
				OpenViBE::int32 i32ChannelDisplayWidthRequest,
				OpenViBE::int32 i32ChannelDisplayHeightRequest,
				OpenViBE::int32 i32LeftRulerWidthRequest,
				OpenViBE::int32 i32LeftRulerHeightRequest);

			/**
			 * \brief Destructor
			 */
			~CTimeFrequencyMapChannelDisplay();

			/**
			 * \brief Get channel widget (table containing both ruler and TF map display areas)
			 * \return Pointer to top table widget
			 */
			::GtkWidget* getTopWidget();

			/**
			 * \brief Get TF map widget
			 * \return Pointer to TF map widget
			 */
			 ::GtkWidget* getMapWidget();

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
			 * \brief Draw TF map
			 */
			void drawMap();

			/**
			 * \brief Draw left ruler
			 */
			void drawLeftRuler();

			/**
			 * \brief Compute TF map
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
			 * \brief Set minimum/maximum values attenuation
			 * The attenuation is performed between last min/max values and current values, according to this formula :
			 * minimum = f64Attenuation * minimum + (1-f64Attenuation) * previous minimum. Therefore, 1 corresponds to no attenuation.
			 * \param f64Attenuation Attenuation of min/max values between 2 consecutive buffers
			 */
			void setMinMaxAttenuation(
				OpenViBE::float64 f64Attenuation);

			/**
			 * \brief Set displayed frequency range
			 * \param f64MinimumDisplayedFrequency Minimum frequency to display
			 * \param f64MaximumDisplayedFrequency Maximum frequency to display
			 */
			void setDisplayedFrequencyRange(
				OpenViBE::float64 f64MinimumDisplayedFrequency,
				OpenViBE::float64 f64MaximumDisplayedFrequency);

		private:
			/**
			 * \brief Compute minimum and maximum displayed frequency band indices (in CSpectrumDatabase)
			 * \param rMinDisplayedFrequencyIndex Minimum displayed frequency band index
			 * \param rMaxDisplayedFrequencyIndex Maximum displayed frequency band index
			 * \return True if indices could be retrieved
			 */
			OpenViBE::boolean getDisplayedFrequencyBandIndices(
				OpenViBE::uint32& rMinDisplayedFrequencyIndex,
				OpenViBE::uint32& rMaxDisplayedFrequencyIndex);

		private:
			//Spectrum database
			CSpectrumDatabase& m_rSpectrumDatabase;
			//Table containing TF map and left ruler
			::GtkTable* m_pWidgetTable;
			//Channel drawing area
			::GtkWidget* m_pDisplay;
			//Amplitudes scale drawing area
			::GtkWidget* m_pLeftRuler;
			//Left padding for left ruler labels
			OpenViBE::uint32 m_pLeftRulerLeftPadding;
			//Gdk graphics context
			::GdkGC* m_pGraphicsContext;
			//Channel index
			OpenViBE::uint32 m_ui32Channel;
			//Minimum frequency to display
			OpenViBE::float64 m_f64MinimumDisplayedFrequency;
			//Maximum frequency to display
			OpenViBE::float64 m_f64MaximumDisplayedFrequency;
			//Minimum amplitude value for this channel in current TF map
			OpenViBE::float64 m_f64MinimumValue;
			//Maximum amplitude value for this channel in current TF map
			OpenViBE::float64 m_f64MaximumValue;
			//Attenuation coefficient between previous min/max values and current ones
			OpenViBE::float64 m_f64Attenuation;
			//RGB buffer storing current TF map
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
