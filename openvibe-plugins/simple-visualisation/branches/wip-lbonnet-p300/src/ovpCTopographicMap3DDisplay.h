#ifndef __SamplePlugin_CTopographicMap3DDisplay_H__
#define __SamplePlugin_CTopographicMap3DDisplay_H__

#include "ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/IReader.h>

#include <gtk/gtk.h>

#include "ovpCTopographicMapDatabase.h"

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		class CTopographicMap3DView;

		class CTopographicMap3DDisplay : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >,
		virtual public OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback::ICallback,
		virtual public CTopographicMapDrawable
		{
		public:
			CTopographicMap3DDisplay(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(
				OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean processClock(
				OpenViBE::Kernel::IMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_TopographicMap3DDisplay)

			/** \name IBoxAlgorithmStreamedMatrixInputReaderCallback::ICallback implementation */
			//@{
			virtual void setMatrixDimmensionCount(const OpenViBE::uint32 ui32DimmensionCount);
			virtual void setMatrixDimmensionSize(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize);
			virtual void setMatrixDimmensionLabel(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel);
			virtual void setMatrixBuffer(const OpenViBE::float64* pBuffer);
			//@}

			/** \name CSignalDisplayDrawable implementation */
			//@{

			/**
			 * \brief Initialize window.
			 */
			virtual void init();
			/**
			 * \brief Invalidate window contents and tell it to redraw itself.
			 */
			virtual void redraw();

			//@}

			/** \name CTopographicMapDrawable implementation */
			//@{

			/**
			 * \brief Return pointer to sample matrix (locations where to interpolate values)
			 */
			virtual OpenViBE::CMatrix* getSampleCoordinatesMatrix();
			/**
			 * \brief Set pointer to values matrix (interpolation results)
			 */
			virtual OpenViBE::boolean setSampleValuesMatrix(
				OpenViBE::IMatrix* pSampleValuesMatrix);

			//@}

			/**
			 * \brief Toggle electrodes on/off
			 */
			void toggleElectrodes(
				OpenViBE::boolean);

			//void toggleSamplingPoints(OpenViBE::boolean);

		protected:
			/**
			 * \brief Perform initialization tasks related to scalp mesh
			 * Retrieves scalp vertex data, stores it in internal array,
			 * computes electrode localization in model frame based on
			 * the coordinates in normalized frame as stored in database
			 * \return True if initialization completed successfully
			 */
			OpenViBE::boolean initializeScalpData();

			/**
			 * \brief Compute electrode coordinates in head model frame
			 * Coordinates are computed by casting rays from sphere projection
			 * center to normalized electrode coordinates, looking for intersection
			 * between such rays and head model.
			 * \return True if coordinates could be computed for all electrodes, false otherwise
			 */
			OpenViBE::boolean computeModelFrameChannelCoordinates();

			/**
			 * \brief Looks for intersection between ray and triangle
			 * If an intersection exists, rT holds its location from origin
			 * \param[in] pOrigin Ray origin
			 * \param[in] pDirection Ray normalized direction
			 * \param[in] pV0 Triangle vertex 0
			 * \param[in] pV1 Triangle vertex 1
			 * \param[in] pV2 Triangle vertex 2
			 * \param[out] rT Intersection location from origin, if any
			 * \return True if intersection found, false otherwise
			 */
			OpenViBE::boolean findRayTriangleIntersection(
				OpenViBE::float32* pOrigin,
				OpenViBE::float32* pDirection,
				OpenViBE::float32* pV0,
				OpenViBE::float32* pV1,
				OpenViBE::float32* pV2,
				OpenViBE::float32& rT);

			/**
			 * \brief Process 3D requests
			 * \return True if plugin execution may go on, false if plugin must be stopped
			 */
			OpenViBE::boolean process3D();

			/**
			 * \brief Create 3D skull
			 * \return True if skull created, false otherwise
			 */
			OpenViBE::boolean createSkull();

			/**
			 * \brief Create 3D objects at sampling points locations
			 * \return True if sampling points created, false otherwise
			 */
			OpenViBE::boolean createSamplingPoints();

		private:
			//error flag (plugin should be disabled when true)
			OpenViBE::boolean m_bError;

			//channel localisation decoder
			OpenViBE::Kernel::IAlgorithmProxy* m_pChannelLocalisationStreamDecoder;

			//ebml
			EBML::IReader* m_pStreamedMatrixReader;
			OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback* m_pStreamedMatrixReaderCallBack;

			//Name of file containing face mesh
			OpenViBE::CString m_oFaceMeshFilename;
			//Name of file containing scalp mesh
			OpenViBE::CString m_oScalpMeshFilename;
			//Name of file containing sphere on which scalp vertices are mapped
			OpenViBE::CString m_oProjectionSphereMeshFilename;

			//Start time of last buffer
			OpenViBE::uint64 m_ui64StartTime;
			//end time of last buffer
			OpenViBE::uint64 m_ui64EndTime;

			//interpolation algorithm
			OpenViBE::Kernel::IAlgorithmProxy* m_pSphericalSplineInterpolation;
			//signal database object
			CTopographicMapDatabase* m_pTopographicMapDatabase;
			//main object used for the display (contains all the GUI code)
			CTopographicMap3DView* m_pTopographicMap3DView;

			//ID of 3D widget associated to this plugin
			OpenViBE::CIdentifier m_o3DWidgetIdentifier;
			//ID of resource group associated to this plugin
			OpenViBE::CIdentifier m_oResourceGroupIdentifier;

			//flag set to true once skull meshes are loaded
			OpenViBE::boolean m_bSkullCreated;
			//flag set to true once camera is centered in front of 3D scene
			OpenViBE::boolean m_bCameraPositioned;
			//flag set to true once scalp data is initialized
			OpenViBE::boolean m_bScalpDataInitialized;
			//flag set to true once 3D electrode objects are created
			OpenViBE::boolean m_bElectrodesCreated;

			//flag set to true once channel localisation buffer is received
			OpenViBE::boolean m_bModelElectrodeCoordinatesInitialized;

			//number of predefined colors
			OpenViBE::uint32 m_ui32NbColors;
			//scale of predefined colors potentials are converted to
			OpenViBE::float32* m_pColorScale;

			//IDs of electrode objects
			std::vector<OpenViBE::CIdentifier> m_oElectrodeIds;
			//flag set to true when electrode objects toggle status has changed
			OpenViBE::boolean m_bNeedToggleElectrodes;
			//flag specifying whether electrode objects must be toggled on or off
			OpenViBE::boolean m_bElectrodesToggleState;

			//IDs of sampling points (scalp vertices) objects
			std::vector<OpenViBE::CIdentifier> m_oSamplingPointIds;
			//flag set to true when sampling points objects toggle status has changed
			OpenViBE::boolean m_bNeedToggleSamplingPoints;
			//flag specifying whether sampling point objects must be toggled on or off
			OpenViBE::boolean m_bSamplingPointsToggleState;

			//ID of face object
			OpenViBE::CIdentifier m_oFaceId;
			//ID of scalp object
			OpenViBE::CIdentifier m_oScalpId;
			//number of scalp vertices
			OpenViBE::uint32 m_ui32NbScalpVertices;
			//working copy of scalp vertices coordinates, in model space
			OpenViBE::float32* m_pScalpVertices;
			//center of unit sphere on which scalp vertices are projected, in normalized space (X right Y front Z up)
			OpenViBE::float32 m_f32ProjectionCenter[3];
			//normalized vertices where to interpolate potentials
			OpenViBE::CMatrix m_oSampleCoordinatesMatrix;
			//scalp vertex colors
			OpenViBE::float32* m_pScalpColors;
		};

		class CTopographicMap3DDisplayDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("3D topographic map"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Vincent Delannoy"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("3D potentials mapping using spherical spline interpolation"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Visualisation/Topography"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString(GTK_STOCK_EXECUTE); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_TopographicMap3DDisplay; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SimpleVisualisation::CTopographicMap3DDisplay(); }

			virtual OpenViBE::boolean hasFunctionality(OpenViBE::Kernel::EPluginFunctionality ePF) const
			{
				return ePF == OpenViBE::Kernel::PluginFunctionality_Visualization;
			}

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addSetting("Interpolation type", OVP_TypeId_SphericalLinearInterpolationType, "1");
				rPrototype.addSetting("Delay (in s)", OV_TypeId_Float, "0");
				rPrototype.addSetting("Face mesh filename", OV_TypeId_String, "face");
				rPrototype.addSetting("Scalp mesh filename", OV_TypeId_String, "scalp");
				rPrototype.addSetting("Sphere projection mesh filename", OV_TypeId_String, "projection_center");
				rPrototype.addInput("Signal", OV_TypeId_StreamedMatrix);
				rPrototype.addInput("Channel localisation", OV_TypeId_ChannelLocalisation);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_TopographicMap3DDisplayDesc);
		};
	};
};

#endif // __SamplePlugin_CTopographicMap3DDisplay_H__
