#ifndef __OpenViBEPlugins_SimpleVisualisation_CTimeFrequencyMapDisplayView_H__
#define __OpenViBEPlugins_SimpleVisualisation_CTimeFrequencyMapDisplayView_H__

#include "../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <gtk/gtk.h>

#include "../ovpCSpectrumDatabase.h"
#include "../ovpCTimeRuler.h"
#include "ovpCTimeFrequencyMapChannelDisplay.h"

#include <vector>
#include <string>
#include <map>

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		/**
		 * This class manages Gtk widgets used to display a TimeFrequency map.
		 * Spectrum data is retrieved from spectrum database object
		 */
		class CTimeFrequencyMapDisplayView : public OpenViBEPlugins::SimpleVisualisation::IStreamDisplayDrawable
		{
		public:
			/**
			 * \brief Constructor
			 * \param rSpectrumDatabase Object storing spectrum data
			 * \param f64MinDisplayedFrequency Minimum frequency to display
			 * \param f64MaxDisplayedFrequency Maximum frequency to display
			 * \param f64TimeScale Time scale to display (in s)
			 */
			CTimeFrequencyMapDisplayView(
				CSpectrumDatabase& rSpectrumDatabase,
				OpenViBE::float64 f64MinDisplayedFrequency,
				OpenViBE::float64 f64MaxDisplayedFrequency,
				OpenViBE::float64 f64TimeScale);

			/**
			 * \brief Destructor
			 */
			virtual ~CTimeFrequencyMapDisplayView();

			/** \name IStreamDisplayDrawable implementation */
			//@{

			/**
			 * \brief Initialize widgets.
			 * \remarks Called automatically by spectrum database when first buffer is received
			 * \return True if initialization was successful, false otherwise
			 */
			virtual OpenViBE::boolean init(void);

			/**
			 * \brief Invalidate window contents and have it redraw itself.
			 * \return True if redraw was successful, false otherwise
			 */
			virtual OpenViBE::boolean redraw(void);

			//@}

			/**
			 * \brief Get pointers to plugin main widget and (optional) toolbar widget
			 * \param [out] pWidget Pointer to main widget
			 * \param [out] pToolbarWidget Pointer to (optional) toolbar widget
			 */
			void getWidgets(
				::GtkWidget*& pWidget,
				::GtkWidget*& pToolbarWidget);

			/**
			 * \brief Toggle left rulers (frequency scales) visibility
			 * \param bActive Visibility flag
			 */
			void toggleLeftRulers(
				OpenViBE::boolean bActive);

			/**
			 * \brief Toggle bottom ruler (time scale) visibility
			 * \param bActive Visibility flag
			 */
			void toggleBottomRuler(
				OpenViBE::boolean bActive);

			/**
			 * Toggle a channel on/off
			 * \param ui64ChannelIndex The index of the channel to toggle.
			 * \param bActive Show the channel if true.
			 */
			void toggleChannel(
				OpenViBE::uint32 ui32ChannelIndex,
				OpenViBE::boolean bActive);

			/**
			 * \brief Update channel widgets visibility
			 */
			void updateMainTableStatus(void);

			/**
			 * \brief Set toolbar buttons activation state
			 * \param bActive Activation flag
			 */
			void activateToolbarButtons(
				OpenViBE::boolean bActive);

			/**
			 * \brief Set minimum/maximum values attenuation
			 * The attenuation is performed between last min/max values and current values, according to this formula :
			 * minimum = f64Attenuation * minimum + (1-f64Attenuation) * previous minimum. Therefore, 1 corresponds to no attenuation.
			 * \param f64Attenuation Attenuation of min/max values between 2 consecutive buffers
			 */
			void setMinMaxAttenuation(
				OpenViBE::float64 f64Attenuation);

			/** \name Gtk widget callbacks */
			//@{
			void minDisplayedFrequencyChangedCB(::GtkWidget* pWidget);
			void maxDisplayedFrequencyChangedCB(::GtkWidget* pWidget);
			void timeScaleChangedCB(::GtkSpinButton* pSpinButton);
			void channelSelectionDialogOpenedCB(GtkButton* pButton);
			void channelSelectionChangedCB(::GtkButton* pButton);
			//@}

			OpenViBE::boolean isChannelDisplayVisible(
				OpenViBE::uint32 ui32ChannelIndex);

		private:
			//! The spectrum database that contains the information to use to draw TF maps
			CSpectrumDatabase& m_rSpectrumDatabase;
			//! The Builder handler used to create the interface
			::GtkBuilder* m_pBuilderInterface;
			//! The table containing the displays
			GtkWidget * m_pDisplayTable;
			//! Array of channel label widgets
			std::vector<GtkWidget*> m_vChannelLabels;
			//! Array of Displays (one per channel, displays the corresponding channel)
			std::vector<CTimeFrequencyMapChannelDisplay*> m_vChannelDisplays;
			std::map < OpenViBE::uint32, ::GtkWidget* > m_vSeparator;
			//! Vector of pointers to the select channels dialog's check buttons
			std::vector<GtkWidget *> m_vChannelsCheckButtons;
			//! Vector of indexes of the channels to display
			std::map<OpenViBE::uint32, OpenViBE::boolean> m_vSelectedChannels;

			//! Show left rulers when true
			OpenViBE::boolean m_bShowLeftRulers;
			//!Show bottom time ruler when true
			OpenViBE::boolean m_bShowBottomRuler;
			//! Attenuation value
			OpenViBE::float64 m_f64Attenuation;
			//! Minimum frequency to display
			OpenViBE::float64 m_f64MinDisplayedFrequency;
			//! Maximum frequency to display
			OpenViBE::float64 m_f64MaxDisplayedFrequency;
			//! Bottom box containing bottom ruler
			GtkBox* m_pBottomBox;
			//! Pointer to time ruler
			CTimeRuler* m_pBottomRuler;
		};
	};
};

#endif
