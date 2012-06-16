#ifndef __SimpleVisualisationPlugin_CTopographicMap2DView_H__
#define __SimpleVisualisationPlugin_CTopographicMap2DView_H__

#include "../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <gtk/gtk.h>

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
		class CTopographicMap2DView : public CTopographicMapDrawable
		{
		public:
			enum ETopographicMap2DProjection
		{
			TopographicMap2DProjection_Axial,
			TopographicMap2DProjection_Radial,
			TopographicMap2DProjection_NumProjection
		};

			enum ETopographicMap2DView
			{
				TopographicMap2DView_Top,
				TopographicMap2DView_Left,
				TopographicMap2DView_Right,
				TopographicMap2DView_Back
			};

			/**
			 * \brief Constructor
			 * \param rTopographicMapDatabase Datastore
			 * \param ui64DefaultInterpolation Interpolation mode
			 * \param ui64Delay Delay to apply to displayed data
			 */
			CTopographicMap2DView(
				CTopographicMapDatabase& rTopographicMapDatabase,
				OpenViBE::uint64 ui64DefaultInterpolation,
				OpenViBE::float64 f64Delay);

			/**
			 * \brief Destructor
			 */
			virtual ~CTopographicMap2DView();

			/** \name CSignalDisplayDrawable implementation */
			//@{

			/**
			 * \brief Initialize widgets
			 */
			virtual void init();

			/**
			 * \brief Redraw map
			 */
			virtual void redraw();

			//@}

			/** \name CTopographicMapDrawable implementation */
			//@{

			/**
			 * \brief Get matrix of sample points coordinates (places where to interpolate values)
			 * \return Pointer to matrix of sample points coordinates
			 */
			virtual OpenViBE::CMatrix* getSampleCoordinatesMatrix();

			/**
			 * \brief Set matrix of sample points values (values interpolated at places specified in sample coordinates matrix)
			 * \param [in] pSampleValuesMatrix Pointer to matrix of sample points values
			 * \return True if values were successfully set, false otherwise
			 */
			virtual OpenViBE::boolean setSampleValuesMatrix(
				OpenViBE::IMatrix* pSampleValuesMatrix);

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
			 * \brief Get ID of current view
			 * \return ID of current view
			 */
			ETopographicMap2DView getCurrentView(void);

			/** \name Callbacks */
			//@{

			void resizeCB(OpenViBE::uint32 ui32Width, OpenViBE::uint32 ui32Height);
			void toggleElectrodesCB();
			void setProjectionCB(::GtkWidget* pWidget);
			void setViewCB(::GtkWidget* pWidget);
			void setInterpolationCB(::GtkWidget* pWidget);
			void setDelayCB(OpenViBE::float64 f64Delay);

			//@}

		private:
			//draw color palette
			void drawPalette(
				OpenViBE::uint32 ui32X,
				OpenViBE::uint32 ui32Y,
				OpenViBE::uint32 ui32Width,
				OpenViBE::uint32 ui32Height);

			//draw face (ears, nose, neck)
			void drawFace(
				OpenViBE::uint32 ui32X,
				OpenViBE::uint32 ui32Y,
				OpenViBE::uint32 ui32Width,
				OpenViBE::uint32 ui32Height);

			//draw head
			void drawHead();

				//draw RGB buffer
				void drawPotentials();

				//draw electrodes corresponding to visible channels as rings
				void drawElectrodes();

					/**
					 * \brief Get channel position in 2D
					 * \param ui32ChannelIndex[in] Index of channel which position is to be retrieved
					 * \param l_i32ChannelX[out] X coordinate of channel location, if channel is visible
					 * \param l_i32ChannelY[out] Y coordinate of channel location, if channel is visible
					 * \return True if channel is visible in current view, false otherwise
					 */
					OpenViBE::boolean getChannel2DPosition(
						OpenViBE::uint32 ui32ChannelIndex,
						gint& l_i32ChannelX,
						gint& l_i32ChannelY);

			//update RGB buffer with interpolated values
			void refreshPotentials();

			//draw a box in RGB buffer
			void drawBoxToBuffer(
				OpenViBE::uint32 ui32X,
				OpenViBE::uint32 ui32Y,
				OpenViBE::uint32 ui32Width,
				OpenViBE::uint32 ui32Height,
				OpenViBE::uint8 ui8Red,
				OpenViBE::uint8 ui8Green,
				OpenViBE::uint8 ui8Blue);

			void enableElectrodeButtonSignals(
				OpenViBE::boolean bEnable);

			void enableProjectionButtonSignals(
				OpenViBE::boolean bEnable);

			void enableViewButtonSignals(
				OpenViBE::boolean bEnable);

			void enableInterpolationButtonSignals(
				OpenViBE::boolean bEnable);

			/**
			 * \brief Compute normalized coordinates of 2D samples
			 * \remarks This method should first be called with bComputeCoordinates = false, allowing caller
			 * to resize data structures appropriately, and then it may be called with bComputeCoordinates = true
			 * \param bComputeCoordinates If false, this method only computes the number of visible samples
			 * \return Number of visible samples (samples lying within the actual skull area)
			 */
			OpenViBE::uint32 computeSamplesNormalizedCoordinates(
				OpenViBE::boolean bComputeCoordinates);

			void resizeData();

			void redrawClipmask();

			OpenViBE::float64 getThetaFromCartesianCoordinates(
				const OpenViBE::float64* l_pCartesianCoords) const;

			OpenViBE::float64 getPhiFromCartesianCoordinates(
				const OpenViBE::float64* l_pCartesianCoords) const;

			OpenViBE::boolean compute2DCoordinates(
				OpenViBE::float64 f64Theta,
				OpenViBE::float64 f64Phi,
				OpenViBE::uint32 ui32SkullCenterX,
				OpenViBE::uint32 ui32SkullCenterY,
				gint& rX,
				gint& rY) const;

		private:
			//! The database that contains the information to use to draw the signals
			CTopographicMapDatabase& m_rTopographicMapDatabase;

			//Maximum delay that can be applied to displayed data
			OpenViBE::float64 m_f64MaxDelay;

			::GtkBuilder* m_pBuilderInterface;

			::GtkWidget* m_pDrawingArea;
			::GdkBitmap* m_pClipmask; //origin (m_ui32SkullX, m_ui32SkullY)
			OpenViBE::uint32 m_ui32ClipmaskWidth;
			OpenViBE::uint32 m_ui32ClipmaskHeight;
			::GdkGC* m_pClipmaskGC;
			::GdkRegion* m_pVisibleRegion; //reallocated whenever clipmask changes

			::GdkColor m_oBackgroundColor;

			//! Active projection
			ETopographicMap2DProjection m_ui32CurrentProjection;
			//! Projection radio buttons
			GtkRadioToolButton* m_pAxialProjectionButton;
			GtkRadioToolButton* m_pRadialProjectionButton;

			//! Active view
			ETopographicMap2DView m_ui32CurrentView;
			//! View radio buttons
			GtkRadioToolButton* m_pTopViewButton;
			GtkRadioToolButton* m_pLeftViewButton;
			GtkRadioToolButton* m_pRightViewButton;
			GtkRadioToolButton* m_pBackViewButton;

			//! Interpolation type
			OpenViBE::uint64 m_ui64CurrentInterpolation;
			GtkRadioToolButton* m_pMapPotentials;
			GtkRadioToolButton* m_pMapCurrents;

			//! Electrodes toggle button
			GtkToggleToolButton* m_pElectrodesToggleButton;
			//! Electrodes toggle state
			OpenViBE::boolean m_bElectrodesToggledOn;

			OpenViBE::boolean m_bNeedResize;

			OpenViBE::uint32 m_ui32GridSize;
			OpenViBE::uint32 m_ui32CellSize;

			OpenViBE::CMatrix m_oSampleCoordinatesMatrix;

			std::vector<OpenViBE::uint32> m_oSampleValues;
			std::vector<std::pair<OpenViBE::uint32, OpenViBE::uint32> > m_oSample2DCoordinates; //in skull coords

			OpenViBE::uint32 m_ui32MinPaletteBarHeight;
			OpenViBE::uint32 m_ui32MaxPaletteBarHeight;

			OpenViBE::uint32 m_ui32HeadWindowWidth;
			OpenViBE::uint32 m_ui32HeadWindowHeight;

			OpenViBE::uint32 m_ui32PaletteWindowWidth;
			OpenViBE::uint32 m_ui32PaletteWindowHeight;

			OpenViBE::uint32 m_ui32SkullX;
			OpenViBE::uint32 m_ui32SkullY;
			OpenViBE::uint32 m_ui32SkullDiameter;
			//angles relative to 3 o'clock position, CCW, in degrees
			OpenViBE::float32 m_f32SkullOutlineStartAngle;
			OpenViBE::float32 m_f32SkullOutlineEndAngle;
			OpenViBE::float32 m_f32SkullFillStartAngle;
			OpenViBE::float32 m_f32SkullFillEndAngle;

			//determined from m_ui32SkullOutlineEndAngle
			OpenViBE::uint32 m_ui32SkullOutlineLeftPointX;
			OpenViBE::uint32 m_ui32SkullOutlineLeftPointY;
			//determined from m_ui32SkullOutlineStartAngle
			OpenViBE::uint32 m_ui32SkullOutlineRightPointX;
			OpenViBE::uint32 m_ui32SkullOutlineRightPointY;

			//determined from m_ui32SkullFillEndAngle
			OpenViBE::uint32 m_ui32SkullFillLeftPointX;
			OpenViBE::uint32 m_ui32SkullFillLeftPointY;
			//determined from m_ui32SkullFillStartAngle
			OpenViBE::uint32 m_ui32SkullFillRightPointX;
			OpenViBE::uint32 m_ui32SkullFillRightPointY;

			OpenViBE::uint32 m_ui32SkullFillBottomPointX;
			OpenViBE::uint32 m_ui32SkullFillBottomPointY;

			/////////////////////////////
			// TOP VIEW
			/////////////////////////////
			OpenViBE::uint32 m_ui32NoseY;

			/////////////////////////////
			// BOTTOM VIEW
			/////////////////////////////
			OpenViBE::uint32 m_ui32LeftNeckX;
			OpenViBE::uint32 m_ui32LeftNeckY;
			OpenViBE::uint32 m_ui32RightNeckX;
			OpenViBE::uint32 m_ui32RightNeckY;

			//////////////////////////////////
			// LEFT/RIGHT VIEWS
			//////////////////////////////////
			/*
			    + A
			   /
			  /
			 /
			+ B
			| C
			+----+ D
			     |
			     + E
			*/
			OpenViBE::uint32 m_ui32NoseTopX; //A
			OpenViBE::uint32 m_ui32NoseTopY;
			OpenViBE::uint32 m_ui32NoseBumpX; //B
			OpenViBE::uint32 m_ui32NoseBumpY;
			OpenViBE::uint32 m_ui32NoseTipX; //C
			OpenViBE::uint32 m_ui32NoseTipY;
			OpenViBE::uint32 m_ui32NoseBaseX; //D
			OpenViBE::uint32 m_ui32NoseBaseY;
			OpenViBE::uint32 m_ui32NoseBottomX; //E
			OpenViBE::uint32 m_ui32NoseBottomY;

			/**
			 * \brief Main pixmap
			 * \remarks This pixmap is 32-bit aligned. Each row is m_ui32RowStride wide, and the pixmap has the height of the DrawingArea's
			 * window. It is pasted into the DrawingArea's window upon redraw
			 */
			//TODO
			//GdkPixmap* m_pPixmap;

			/**
			 * \brief Skull pixmap
			 * \remarks This pixmap is 32-bit aligned. Each row is m_ui32RowStride wide, and the pixmap has m_ui32SkullDiameter rows.
			 * It is pasted into the main pixmap everytime changes happen (window resizing, display options toggled on/off, etc)
			 */
			guchar* m_pSkullRGBBuffer;
			OpenViBE::uint32 m_ui32RowStride;
		};
	};
};

#endif // __SimpleVisualisationPlugin_CTopographicMap2DView_H__
