#if !defined _SIMULATED_OBJECTS_OVKPSSIMULATEDBOX_H_
#define _SIMULATED_OBJECTS_OVKPSSIMULATEDBOX_H_

#include "ovkPsSimulatedBoxBase.h"

#include "../ovkTKernelObject.h"

#include <system/CChrono.h>

#include <vector>
#include <string>
#include <list>

class GtkOgre;

/** \brief Implementation class of \ref PsSimulatedBoxBase.
 * \date 2006-09-12 at 18:00:28
 * \author Yann Renard (INRIA/IRISA)
 * @description
 * This class is the implementation class of \ref PsSimulatedBoxBase.\n
 * All the members of this class are protected except the constructor and
 * the destructor according to the OpenMASK precepts.\n\n
 * @configurationParameters
 * \n See the base class \ref PsSimulatedBoxBase to see used configuration parameters.\n
 * \todo \ref computeParameters must be written.
 */
namespace OpenViBE {
namespace Kernel {
class CPlayer;
class CSimulatedBoxCreator;
class CSimulatedBox : public CSimulatedBoxBase, public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IBoxIO>
{
public:

  /**\brief The creator is a friend. */
  friend class OpenViBE::Kernel::CSimulatedBoxCreator;
  /**\brief The id for the class in a configuration.*/
  static OMK::Name CLASS_ID ;
  /**\brief The registration in the simulated object factory.*/
  static const bool REGISTERED ;

	/// @name Constructors and destructor.
	//@{
	/// \brief Destructor of \ref CSimulatedBox.
	virtual ~CSimulatedBox() ;
	/// \brief Default constructor of \ref CSimulatedBox.
	CSimulatedBox(
		OMK::Controller& ctrl,
		const OMK::ObjectDescriptor& objectDescriptor,
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		const OpenViBE::Kernel::IScenario& rScenario);
	//@}

protected:

	virtual void init(void);

	/// @name OMKSimulatedObject interfaces. Compute method inherited from \ref CSimulatedBoxBase.
	//@{
	/// \brief Compute methods inherited from \ref CSimulatedBoxBase.
	///
	/// The implementation of the behaviour.
	/// This method updates nothing.
	virtual void computeParameters() ;
	//@}

	virtual void handleCrash(const char* sHintName="");
	virtual void doProcess(void);

public:

	virtual OpenViBE::uint64 getCurrentTime() const;
	virtual OpenViBE::CString getOVName(void) const;
	virtual const OpenViBE::Kernel::IScenario& getScenario(void) const;

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

	// Visualisation stuff

	virtual ::GtkWidget* create3DWidget(
		const OpenViBE::CString& rName);

	virtual OpenViBE::boolean setBackgroundColor(
				OpenViBE::float32 f32ColorRed,
				OpenViBE::float32 f32ColorGreen,
				OpenViBE::float32 f32ColorBlue);

	virtual OpenViBE::boolean setCameraToEncompassObjects();

	virtual OpenViBE::CIdentifier createObject(
		const OpenViBE::CString& rObjectFileName);
	virtual OpenViBE::CIdentifier createObject(
		const OpenViBE::Kernel::EStandard3DObject eStandard3DObject);
	virtual OpenViBE::boolean removeObject(
		const OpenViBE::CIdentifier& rIdentifier);
	virtual OpenViBE::boolean setObjectPosition(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::float32 f32PositionX,
		OpenViBE::float32 f32PositionY,
		OpenViBE::float32 f32PositionZ);
	virtual OpenViBE::boolean setObjectOrientation(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::float32 f32OrientationX,
		OpenViBE::float32 f32OrientationY,
		OpenViBE::float32 f32OrientationZ,
		OpenViBE::float32 f32OrientationW);
	virtual OpenViBE::boolean setObjectScale(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::float32 f32ScaleX,
		OpenViBE::float32 f32ScaleY,
		OpenViBE::float32 f32ScaleZ);
	virtual OpenViBE::boolean setObjectScale(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::float32 f32Scale);
	virtual OpenViBE::boolean setObjectColor(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::float32 f32ColorRed,
		OpenViBE::float32 f32ColorGreen,
		OpenViBE::float32 f32ColorBlue);
	virtual OpenViBE::boolean setObjectTransparency(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::float32 f32Transparency);

	virtual OpenViBE::boolean setObjectVertexColorArray(
		const OpenViBE::CIdentifier& rIdentifier,
		const OpenViBE::uint32 ui32VertexColorCount,
		const OpenViBE::float32* pVertexColorArray);

	virtual OpenViBE::boolean getObjectVertexCount(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::uint32& ui32VertexCount) const;
	virtual OpenViBE::boolean getObjectVertexPositionArray(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::float32* pVertexPositionArray) const;

	_IsDerivedFromClass_Final_(OpenViBE::Kernel::IBoxIO, OVK_ClassId_Kernel_Player_SimulatedBox);

protected:

	/// @name Call back methods and listeners for events processing.
	//@{
	/// \brief Call back to process the "OpenViBEDataUpdate" event
	/// \return see \ref OMKValuedEventListenerCallBack::CallBackFct
	virtual bool processOpenViBEDataUpdateEvent( OMK::ValuedEvent< OpenViBE::Kernel::CTypeChunk > *e ) ;
	/// \brief Call back to process the "MaskStart" event
	/// \return see \ref OMKEventListenerCallBack::CallBackFct
	virtual bool processMaskStartEvent( OMK::Event *e ) ;
	/// \brief Call back to process the "MaskStop" event
	/// \return see \ref OMKEventListenerCallBack::CallBackFct
	virtual bool processMaskStopEvent( OMK::Event *e ) ;
	//@}

	OpenViBE::uint32 m_ui32CrashCount;
	OpenViBE::boolean m_bReadyToProcess;
	OpenViBE::boolean m_bActive;

	OpenViBE::Plugins::IBoxAlgorithm* m_pBoxAlgorithm;
	const OpenViBE::Kernel::IScenario* m_pScenario;
	const OpenViBE::Kernel::IBox* m_pBox;

	OpenViBE::uint64 m_ui64LastClockActivationDate;
	OpenViBE::uint64 m_ui64ClockFrequency;
	OpenViBE::uint64 m_ui64ClockActivationStep;
	OpenViBE::boolean m_bAlreadyClockActivatedOnce;

	GtkOgre* m_pGtkOgreWidget;

private:

	System::CChrono m_oBenchmarkChronoProcessClock;
	System::CChrono m_oBenchmarkChronoProcessInput;
	System::CChrono m_oBenchmarkChronoProcess;

public:

	using OpenViBE::Kernel::CSimulatedBoxBase::sendOpenViBEDataUpdateEvent;

	std::vector<std::list< OpenViBE::Kernel::CTypeChunk > > m_vInput;
	std::vector<std::list< OpenViBE::Kernel::CTypeChunk > > m_vOutput;
	std::list< OpenViBE::Kernel::CTypeChunk > m_vCurrentOutput;
	// std::vector<std::string> m_vSetting;
};

class CSimulatedBoxCreator : public OMK::SimulatedObjectCreator
{
public:

	friend class OpenViBE::Kernel::CPlayer;

	OMK::SimulatedObject* operator()(OMK::Controller & ctrl, const OMK::ObjectDescriptor & rObjectDescriptor) const
	{
		OMK::SimulatedObject* l_pObject=new CSimulatedBox(ctrl, rObjectDescriptor, *ms_pKernelContext, *ms_pScenario);
		if(l_pObject)
		{
			l_pObject->postConstruction();
		}
		return l_pObject;
	}

protected:

	static const IKernelContext* ms_pKernelContext;
	static const IScenario* ms_pScenario;
};
};
};

#endif // defined _SIMULATED_OBJECTS_OVKPSSIMULATEDBOX_H_
