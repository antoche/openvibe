#ifndef __SimpleVisualisationPlugin_CVoxelView_H__
#define __SimpleVisualisationPlugin_CVoxelView_H__

#include "../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <gtk/gtk.h>

#include "../ovpCVoxelDisplay.h"

#include <vector>
#include <string>

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		/**
		 * This class contains everything necessary to setup a GTK window and display
		 * a voxel visualiser
		 */
		class CVoxelView : public OpenViBEPlugins::SimpleVisualisation::IStreamDisplayDrawable
		{
		public:
			/**
			 * \brief Constructor
			 * \param rVoxelDisplay Parent plugin
			 */
			CVoxelView(
				CVoxelDisplay& rVoxelDisplay);

			/**
			 * \brief Destructor
			 */
			virtual ~CVoxelView();

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
			 * \brief Get toolbar pointer (if any)
			 * \param [out] pToolbarWidget Pointer to toolbar widget
			 */
			void getToolbar(
				GtkWidget*& pToolbarWidget);
			/**
			 * Returns name of viewport currently used
			 */
			//EVoxelViewport getCurrentViewport(void);
			/**
			 * Sets current viewport.
			 * \param eViewport Display mode to set.
			 */
			//void setCurrentViewport(
				//EVoxelViewport eViewport);

			void setVoxelObjectCB(GtkWidget* pWidget);
			void toggleColorModificationCB(OpenViBE::boolean bModifyColor);
			void toggleTransparencyModificationCB(OpenViBE::boolean bModifyTransparency);
			void toggleSizeModificationCB(OpenViBE::boolean bModifySize);
			void setMinVoxelScaleFactorCB(::GtkSpinButton* pWidget);
			void setMaxVoxelScaleFactorCB(::GtkSpinButton* pWidget);
			void setMinDisplayThresholdCB(OpenViBE::float64 f64MinDisplayThreshold);
			void setMaxDisplayThresholdCB(OpenViBE::float64 f64MaxDisplayThreshold);
			void setDisplayThresholdBoundaryTypeCB(OpenViBE::boolean bInclusiveBoundary);
			void setSkullOpacityCB(OpenViBE::float64 f64Delay);
			void enableAutoCameraMovementCB(OpenViBE::boolean bEnable);
			void repositionCameraCB();

		private:
			CVoxelDisplay& m_rVoxelDisplay;

			::GtkBuilder* m_pBuilderInterface;

			//! View radio buttons
			::GtkRadioToolButton* m_pCubeButton;
			::GtkRadioToolButton* m_pSphereButton;

			GtkSpinButton* m_pMinScaleFactorSpinButton;
			GtkSpinButton* m_pMaxScaleFactorSpinButton;

			OpenViBE::float64 m_f64MinScaleFactor;
			OpenViBE::float64 m_f64MaxScaleFactor;

			GtkLabel* m_pThresholdRangeAndOrLabel;
			GtkButton* m_pMinDisplayThresholdBoundaryButton;
			GtkButton* m_pMaxDisplayThresholdBoundaryButton;

			GtkScale* m_pMinDisplayThresholdScale;
			GtkScale* m_pMaxDisplayThresholdScale;

			OpenViBE::float64 m_f64MinDisplayThreshold;
			OpenViBE::float64 m_f64MaxDisplayThreshold;
		};
	}
}

#endif // __SimpleVisualisationPlugin_CVoxelView_H__
