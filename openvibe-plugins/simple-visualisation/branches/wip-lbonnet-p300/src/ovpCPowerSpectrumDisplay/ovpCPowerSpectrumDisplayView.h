#ifndef __OpenViBEPlugins_SimpleVisualisation_CPowerSpectrumDisplayView_H__
#define __OpenViBEPlugins_SimpleVisualisation_CPowerSpectrumDisplayView_H__

#include "../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <gtk/gtk.h>

#include "../ovpCBufferDatabase.h"
#include "ovpCPowerSpectrumChannelDisplay.h"

#include <vector>
#include <string>
#include <map>

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		class CPowerSpectrumDatabase;

		/**
		 * This class manages Gtk widgets used to display a power spectrum.
		 * Power spectrum data is retrieved from an external CPowerSpectrumDatabase object.
		 */
		class CPowerSpectrumDisplayView : public CSignalDisplayDrawable
		{
		public:
			/**
			 * \brief Constructor
			 * \param pPowerSpectrumDatabase Object storing power spectrum data
			 * \param f64MinDisplayedFrequency Minimum frequency to display
			 * \param f64MaxDisplayedFrequency Maximum frequency to display
			 */
			CPowerSpectrumDisplayView(
				CPowerSpectrumDatabase& pPowerSpectrumDatabase,
				OpenViBE::float64 f64MinDisplayedFrequency,
				OpenViBE::float64 f64MaxDisplayedFrequency);

			/**
			 * \brief Destructor
			 */
			virtual ~CPowerSpectrumDisplayView();

			/**
			 * \brief Initialize widgets
			 * \remarks Called automatically by spectrum database when first buffer is received
			 */
			virtual void init();

			/**
			 * \brief Get pointers to plugin main widget and (optional) toolbar widget
			 * \param [out] pWidget Pointer to main widget
			 * \param [out] pToolbarWidget Pointer to (optional) toolbar widget
			 */
			void getWidgets(
				::GtkWidget*& pWidget,
				::GtkWidget*& pToolbarWidget);

			/**
			 * \brief Get auto vertical scale status
			 * \return True if auto scale is on, false otherwise
			 */
			OpenViBE::boolean isAutoVerticalScaleEnabled();

			/**
			 * \brief Get custom vertical scale value
			 * \return Custom vertical scale value
			 */
			OpenViBE::float64 getCustomVerticalScaleValue();

			/**
			 * \brief Toggle left rulers (amplitude values) visibility
			 * \param bActive Visibility flag
			 */
			void toggleLeftRulers(
				OpenViBE::boolean bActive);

			/**
			 * \brief Toggle bottom ruler (frequency scale) visibility
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
			 * \brief Draw bottom ruler
			 */
			void redrawBottomRuler();

			/** \name Gtk widget callbacks */
			//@{
			void minDisplayedFrequencyChangedCB(::GtkWidget* pWidget);
			void maxDisplayedFrequencyChangedCB(::GtkWidget* pWidget);
			void showChannelSelectionDialogCB();
			void applyChannelSelectionCB();
			void applyMinMaxAttenuationCB();

			/**
			 * Callback called when vertical scale mode changes
			 * \param pToggleButton Radio button toggled
			 * \return True
			 */
			OpenViBE::boolean onVerticalScaleModeToggledCB(
				GtkToggleButton* pToggleButton);

			/**
			 * Callback called when custom vertical scale is changed
			 * \param pEditable Custom vertical scale widget
			 * \return True if custom vertical scale value could be retrieved, false otherwise
			 */
			OpenViBE::boolean onCustomVerticalScaleChangedCB(
				GtkEditable *pEditable);

			//@}

			OpenViBE::boolean isSelected(OpenViBE::uint32 ui32Channel) { return m_vSelectedChannels[ui32Channel]; }

		private:
			/**
			 * \brief Invalidate window contents and have it redraw itself
			 */
			virtual void redraw();

			/**
			 * \brief Update channel widgets visibility
			 */
			void updateMainTableStatus();

			/**
			 * \brief Set toolbar buttons activation state
			 * \param bActive Activation flag
			 */
			void activateToolbarButtons(
				OpenViBE::boolean bActive);

		private:
			//! The Builder handler used to create the interface
			::GtkBuilder* m_pBuilderInterface;
			//! Table (3*N) containing (channel label, vertical separator, channel display) triplets
			GtkWidget* m_pDisplayTable;
			//! Array of channels labels
			std::vector<GtkWidget*> m_oChannelLabels;
			//! Array of channel displays
			std::vector<CPowerSpectrumChannelDisplay*> m_oChannelDisplays;
			std::map < OpenViBE::uint32, ::GtkWidget* > m_vSeparator;
			//! Bottom box containing bottom ruler
			GtkBox* m_pBottomBox;
			//! Bottom ruler widget
			GtkWidget* m_pBottomRuler;

			/** \name Vertical scale */
			//@{
			//! Auto vertical scale radio button
			GtkRadioButton* m_pAutoVerticalScaleRadioButton;
			//! Flag set to true when auto vertical scale is toggled on
			OpenViBE::boolean m_bAutoVerticalScale;
			//! Value of custom vertical scale
			OpenViBE::float64 m_f64CustomVerticalScaleValue;
			//@}

			//! The database that contains the information to use to draw the signals
			CPowerSpectrumDatabase* m_pPowerSpectrumDatabase;
			//! Vector of indexes of the channels to display
			std::map<OpenViBE::uint32, OpenViBE::boolean> m_vSelectedChannels;
			//! Minimum frequency to display
			OpenViBE::float64 m_f64MinDisplayedFrequency;
			//! Maximum frequency to display
			OpenViBE::float64 m_f64MaxDisplayedFrequency;
		};
	}
}

#endif
