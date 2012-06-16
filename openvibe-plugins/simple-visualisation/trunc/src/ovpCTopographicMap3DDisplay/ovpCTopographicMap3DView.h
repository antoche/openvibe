#ifndef __SimpleVisualisationPlugin_CTopographicMap3DView_H__
#define __SimpleVisualisationPlugin_CTopographicMap3DView_H__

#include "../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <gtk/gtk.h>

#include "../ovpCTopographicMap3DDisplay.h"
#include "../ovpCTopographicMapDatabase.h"

#include <vector>
#include <string>

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		/**
		 * This class contains everything necessary to setup a GTK window and display
		 * a 2D topographic map
		 */
		class CTopographicMap3DView
		{
		public:
			/**
			 * \brief Constructor
			 * \param rTopographicMap3DDisplay Parent plugin
			 * \param rTopographicMapDatabase Datastore
			 * \param ui64DefaultInterpolation Interpolation mode
			 * \param ui64Delay Delay to apply to displayed data
			 */
			CTopographicMap3DView(
				CTopographicMap3DDisplay& rTopographicMap3DDisplay,
				CTopographicMapDatabase& rTopographicMapDatabase,
				OpenViBE::uint64 ui64DefaultInterpolation,
				OpenViBE::float64 f64Delay);

			/**
			 * \brief Destructor
			 */
			virtual ~CTopographicMap3DView();

			/**
			 * \brief Initialize widgets
			 */
			virtual void init();

			/**
			 * \brief Get toolbar pointer (if any)
			 * \param [out] pToolbarWidget Pointer to toolbar widget
			 */
			void getToolbar(
				::GtkWidget*& pToolbarWidget);

			/** \name Callbacks */
			//@{

			void setInterpolationCB(::GtkWidget* pWidget);
			void toggleElectrodesCB();
			//void toggleSamplingPointsCB();
			void setDelayCB(OpenViBE::float64 f64Delay);

			//@}

		private:
			void enableInterpolationButtonSignals(
				OpenViBE::boolean bEnable);

		private:
			CTopographicMap3DDisplay& m_rTopographicMap3DDisplay;

			//! The database that contains the information to use to draw the signals
			CTopographicMapDatabase& m_rTopographicMapDatabase;

			//Maximum delay that can be applied to displayed data
			OpenViBE::float64 m_f64MaxDelay;

			::GtkBuilder* m_pBuilderInterface;

			//! Interpolation type
			OpenViBE::uint64 m_ui64CurrentInterpolation;
			GtkRadioToolButton* m_pMapPotentials;
			GtkRadioToolButton* m_pMapCurrents;

			//! Electrodes toggle button
			GtkToggleToolButton* m_pElectrodesToggleButton;
			//! Electrodes toggle state
			OpenViBE::boolean m_bElectrodesToggledOn;

			//! Pointer to sampling points toggle button
			//GtkToggleToolButton* m_pSamplingPointsToggleButton;
			//! Sampling points toggle state
			//OpenViBE::boolean m_bSamplingPointsToggledOn;
		};
	};
};

#endif // __SimpleVisualisationPlugin_CTopographicMap3DView_H__
