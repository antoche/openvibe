#ifndef __OpenViBEKernel_Kernel_Player_CSimulatedBox_H__
#define __OpenViBEKernel_Kernel_Player_CSimulatedBox_H__

#include "../ovkTKernelObject.h"
#include "ovkCBuffer.h"
#include "../ovkIGtkOVCustomHandler.h"

#include <gtk/gtk.h>
#include <system/CChrono.h>
#include <vector>
#include <string>
#include <deque>

class COgreVisualisation;

namespace OpenViBE
{
	namespace Kernel
	{
		class CScheduler;

		class CChunk
		{
		public:

			CChunk(void)
				:m_ui64StartTime(0)
				,m_ui64EndTime(0)
				,m_bIsDeprecated(false)
			{
			}

			CChunk(const OpenViBE::Kernel::CChunk& rChunk)
				:m_oBuffer(rChunk.m_oBuffer)
				,m_ui64StartTime(rChunk.m_ui64StartTime)
				,m_ui64EndTime(rChunk.m_ui64EndTime)
				,m_bIsDeprecated(false)
			{
			}

			const OpenViBE::Kernel::CBuffer& getBuffer(void) const
			{
				return m_oBuffer;
			}

			OpenViBE::uint64 getStartTime(void) const
			{
				return m_ui64StartTime;
			}

			OpenViBE::uint64 getEndTime(void) const
			{
				return m_ui64EndTime;
			}

			OpenViBE::boolean isDeprecated(void) const
			{
				return m_bIsDeprecated;
			}

			OpenViBE::Kernel::CBuffer& getBuffer(void)
			{
				return m_oBuffer;
			}

			OpenViBE::boolean setStartTime(OpenViBE::uint64 ui64StartTime)
			{
				m_ui64StartTime=ui64StartTime;
				return true;
			}

			OpenViBE::boolean setEndTime(OpenViBE::uint64 ui64EndTime)
			{
				m_ui64EndTime=ui64EndTime;
				return true;
			}

			OpenViBE::boolean markAsDeprecated(OpenViBE::boolean bIsDeprecated)
			{
				m_bIsDeprecated=bIsDeprecated;
				return true;
			}

		protected:

			OpenViBE::Kernel::CBuffer m_oBuffer;
			OpenViBE::uint64 m_ui64StartTime;
			OpenViBE::uint64 m_ui64EndTime;
			OpenViBE::boolean m_bIsDeprecated;
		};

		class CSimulatedBox : public OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IBoxIO >, public IGtkOVCustomHandler
		{
		public:

			CSimulatedBox(
				const OpenViBE::Kernel::IKernelContext& rKernelContext,
				OpenViBE::Kernel::CScheduler& rScheduler);

			virtual ~CSimulatedBox() ;

			/** \name IGtkOVCustomHandler implementation */
			//@{
			OpenViBE::boolean handleDestroyEvent(::GtkWidget* pOVCustomWidget);
			OpenViBE::boolean handleRealizeEvent(::GtkWidget* pOVCustomWidget);
			OpenViBE::boolean handleUnrealizeEvent(::GtkWidget* pOVCustomWidget);
			OpenViBE::boolean handleSizeAllocateEvent(::GtkWidget* pOVCustomWidget, unsigned int uiWidth, unsigned int uiHeight);
			OpenViBE::boolean handleExposeEvent(::GtkWidget* pOVCustomWidget);
			OpenViBE::boolean handleMotionEvent(::GtkWidget* pOVCustomWidget,	int i32X, int i32Y);
			OpenViBE::boolean handleButtonPressEvent(::GtkWidget* pOVCustomWidget, unsigned int uiButton,	int i32X, int i32Y);
			OpenViBE::boolean handleButtonReleaseEvent(::GtkWidget* pOVCustomWidget, unsigned int uiButton,	int i32X, int i32Y);
			//@}

			virtual OpenViBE::boolean setScenarioIdentifier(
				const OpenViBE::CIdentifier& rScenarioIdentifier);

			virtual OpenViBE::boolean getBoxIdentifier(
				OpenViBE::CIdentifier& rBoxIdentifier);

			virtual OpenViBE::boolean setBoxIdentifier(
				const OpenViBE::CIdentifier& rBoxIdentifier);

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean processClock(void);
			virtual OpenViBE::boolean processInput(const OpenViBE::uint32 ui32InputIndex, const OpenViBE::Kernel::CChunk& rChunk);
			virtual OpenViBE::boolean process(void);
			virtual OpenViBE::boolean isReadyToProcess(void) const;

			virtual OpenViBE::uint64 getCurrentTime() const;
			virtual OpenViBE::CString getName(void) const;
			virtual const OpenViBE::Kernel::IScenario& getScenario(void) const;

			/** \name IBoxIO inputs handling */
			//@{
			virtual OpenViBE::uint32 getInputChunkCount(
				const OpenViBE::uint32 ui32InputIndex) const;
			virtual OpenViBE::boolean getInputChunk(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::uint32 ui32ChunkIndex,
				OpenViBE::uint64& rStartTime,
				OpenViBE::uint64& rEndTime,
				OpenViBE::uint64& rChunkSize,
				const OpenViBE::uint8*& rpChunkBuffer) const;
			virtual const OpenViBE::IMemoryBuffer* getInputChunk(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::uint32 ui32ChunkIndex) const;
			virtual OpenViBE::uint64 getInputChunkStartTime(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::uint32 ui32ChunkIndex) const;
			virtual OpenViBE::uint64 getInputChunkEndTime(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::uint32 ui32ChunkIndex) const;
			virtual OpenViBE::boolean markInputAsDeprecated(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::uint32 ui32ChunkIndex);
			//@}

			/** \name IBoxIO outputs handling */
			//@{
			virtual OpenViBE::uint64 getOutputChunkSize(
				const OpenViBE::uint32 ui32OutputIndex) const;
			virtual OpenViBE::boolean setOutputChunkSize(
				const OpenViBE::uint32 ui32OutputIndex,
				const OpenViBE::uint64 ui64Size,
				const OpenViBE::boolean bDiscard=true);
			virtual OpenViBE::uint8* getOutputChunkBuffer(
				const OpenViBE::uint32 ui32OutputIndex);
			virtual OpenViBE::boolean appendOutputChunkData(
				const OpenViBE::uint32 ui32OutputIndex,
				const OpenViBE::uint8* pBuffer,
				const OpenViBE::uint64 ui64BufferSize);
			virtual OpenViBE::IMemoryBuffer* getOutputChunk(
				const OpenViBE::uint32 ui32OutputIndex);
			virtual OpenViBE::boolean markOutputAsReadyToSend(
				const OpenViBE::uint32 ui32OutputIndex,
				const OpenViBE::uint64 ui64StartTime,
				const OpenViBE::uint64 ui64EndTime);
			//@}

			/** \name 3D API - window management */
			//@{
			OpenViBE::CIdentifier create3DWidget(
				::GtkWidget*& p3DWidget);
			OpenViBE::boolean is3DWidgetRealized(
				const OpenViBE::CIdentifier& rWidgetIdentifier) const;
			OpenViBE::boolean update3DWidget(
				const OpenViBE::CIdentifier& rWidgetIdentifier);
			OpenViBE::boolean setBackgroundColor(
				const OpenViBE::CIdentifier& rWindowIdentifier,
				OpenViBE::float32 f32ColorRed,
				OpenViBE::float32 f32ColorGreen,
				OpenViBE::float32 f32ColorBlue);
			OpenViBE::boolean setCameraToEncompassObjects(
				const OpenViBE::CIdentifier& rWindowIdentifier);
			OpenViBE::boolean setCameraSphericalCoordinates(
				const CIdentifier& rWindowIdentifier,
				OpenViBE::float32 f32Theta,
				OpenViBE::float32 f32Phi,
				OpenViBE::float32 f32Radius);
			OpenViBE::boolean getCameraSphericalCoordinates(
				const CIdentifier& rWindowIdentifier,
				OpenViBE::float32& rTheta,
				OpenViBE::float32& rPhi,
				OpenViBE::float32& rRadius);
			//@}

			/** \name 3D API - scene management */
			//@{
			OpenViBE::CIdentifier createObject(
				const OpenViBE::CString& rObjectFileName,
				const OpenViBE::CNameValuePairList* pObjectParams);
			OpenViBE::CIdentifier createObject(
				const OpenViBE::Kernel::EStandard3DObject eStandard3DObject,
				const OpenViBE::CNameValuePairList* pObjectParams);
			OpenViBE::boolean removeObject(
				const OpenViBE::CIdentifier& rIdentifier);
			OpenViBE::boolean setObjectVisible(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::boolean bVisible);
			OpenViBE::boolean setObjectPosition(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32PositionX,
				OpenViBE::float32 f32PositionY,
				OpenViBE::float32 f32PositionZ);
			OpenViBE::boolean setObjectOrientation(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32OrientationX,
				OpenViBE::float32 f32OrientationY,
				OpenViBE::float32 f32OrientationZ,
				OpenViBE::float32 f32OrientationW);
			OpenViBE::boolean setObjectScale(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32ScaleX,
				OpenViBE::float32 f32ScaleY,
				OpenViBE::float32 f32ScaleZ);
			OpenViBE::boolean setObjectScale(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32Scale);
			OpenViBE::boolean setObjectColor(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32ColorRed,
				OpenViBE::float32 f32ColorGreen,
				OpenViBE::float32 f32ColorBlue);
			OpenViBE::boolean setObjectTransparency(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32Transparency);
			OpenViBE::boolean setObjectVertexColorArray(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::uint32 ui32VertexColorCount,
				const OpenViBE::float32* pVertexColorArray);
			OpenViBE::boolean getObjectAxisAlignedBoundingBox(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32* pMinimum,
				OpenViBE::float32* pMaximum);
			OpenViBE::boolean getObjectVertexCount(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32VertexCount) const;
			OpenViBE::boolean getObjectVertexPositionArray(
				const OpenViBE::CIdentifier& rIdentifier,
				const uint32 ui32VertexCount,
				OpenViBE::float32* pVertexPositionArray) const;
			OpenViBE::boolean getObjectTriangleCount(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32TriangleCount) const;
			OpenViBE::boolean getObjectTriangleIndexArray(
				const OpenViBE::CIdentifier& rIdentifier,
				const uint32 ui32TriangleCount,
				OpenViBE::uint32* pTriangleIndexArray) const;
			//@}

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IBoxIO >, OVK_ClassId_Kernel_Player_SimulatedBox);

			OpenViBE::Kernel::CScheduler& getScheduler(void)
			{
				return m_rScheduler;
			}

		private:

			CIdentifier createOgreWindow();

			CIdentifier getUnusedIdentifier(void) const;

			virtual void handleCrash(const char* sHintName="");

		protected:

			OpenViBE::uint32 m_ui32CrashCount;
			OpenViBE::boolean m_bReadyToProcess;
			OpenViBE::boolean m_bSuspended;
			OpenViBE::boolean m_bCrashed;

			OpenViBE::Plugins::IBoxAlgorithm* m_pBoxAlgorithm;
			const OpenViBE::Kernel::IScenario* m_pScenario;
			const OpenViBE::Kernel::IBox* m_pBox;
			OpenViBE::Kernel::CScheduler& m_rScheduler;

			OpenViBE::uint64 m_ui64LastClockActivationDate;
			OpenViBE::uint64 m_ui64ClockFrequency;
			OpenViBE::uint64 m_ui64ClockActivationStep;

		private:

			System::CChrono m_oBenchmarkChronoProcessClock;
			System::CChrono m_oBenchmarkChronoProcessInput;
			System::CChrono m_oBenchmarkChronoProcess;

			//OpenViBE::CString m_sName;
			COgreVisualisation* m_pOgreVis;

			OpenViBE::CIdentifier m_oSceneIdentifier;
			OpenViBE::CString m_oSceneName;

			/// maps GtkWidgets to corresponding COgreWindow identifiers
			std::map<GtkWidget*, OpenViBE::CIdentifier> m_mOgreWindows;

			/// maps object identifiers to object names
			std::map<OpenViBE::CIdentifier, OpenViBE::CString> m_mSimulatedObjects;

		public:

			std::vector < std::deque < OpenViBE::Kernel::CChunk > > m_vInput;
			std::vector < std::deque < OpenViBE::Kernel::CChunk > > m_vOutput;
			std::vector < OpenViBE::Kernel::CChunk > m_vCurrentOutput;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CSimulatedBox_H__
