#ifndef __OpenViBE_Plugins_IBoxAlgorithm_H__
#define __OpenViBE_Plugins_IBoxAlgorithm_H__

#include "ovIPluginObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IMessageClock;
		class IMessageEvent;
		class IMessageSignal;

		/**
		 * \brief This enum lists all the way a box can be modified
		 * \sa OpenViBE::Plugins::IBoxListener::process
		 */
		enum EBoxModification
		{
			BoxModification_Initialized,
			BoxModification_NameChanged,
			BoxModification_IdentifierChanged,
			BoxModification_AlgorithmClassIdentifierChanged,
			BoxModification_ProcessingUnitChanged,
			BoxModification_InputConnected,
			BoxModification_InputDisconnected,
			BoxModification_InputAdded,
			BoxModification_InputRemoved,
			BoxModification_InputTypeChanged,
			BoxModification_InputNameChanged,
			BoxModification_OutputConnected,
			BoxModification_OutputDisconnected,
			BoxModification_OutputAdded,
			BoxModification_OutputRemoved,
			BoxModification_OutputTypeChanged,
			BoxModification_OutputNameChanged,
			BoxModification_SettingAdded,
			BoxModification_SettingRemoved,
			BoxModification_SettingTypeChanged,
			BoxModification_SettingNameChanged,
			BoxModification_SettingDefaultValueChanged,
			BoxModification_SettingValueChanged,
		};
	};

	// for backward compatibility
	typedef OpenViBE::Kernel::IMessageClock CMessageClock;
	typedef OpenViBE::Kernel::IMessageEvent CMessageEvent;
	typedef OpenViBE::Kernel::IMessageSignal CMessageSignal;

	namespace Kernel
	{
		class IBoxAlgorithmContext;
		class IBoxListenerContext;
	}

	namespace Plugins
	{
		/**
		 * \class IBoxAlgorithm
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-06-19
		 * \brief Algorithm to create/process/transform §OpenViBE§ data
		 * \ingroup Group_Extend
		 *
		 * This class should be derived by any plugin that is related
		 * to data processing. It can be data acquisition/production
		 * from an hardware device or from a file. It can be data
		 * processing/transforming, moving time information into
		 * frequency space for example. It can be data classification
		 * generating discrete classification events better than
		 * continuous data flow.
		 *
		 * This is the heart of the extension mechanism of the
		 * §OpenViBE§ platform.
		 *
		 * \sa OpenViBE::Kernel::IBoxAlgorithmDesc
		 *
		 * \todo details about building new plugins
		 */
		class OV_API IBoxAlgorithm : public OpenViBE::Plugins::IPluginObject
		{
		public:

			/** \name Behavioral configuration */
			//@{

			/**
			 * \brief Gets the clock frequency to call this algorithm
			 * \param rBoxAlgorithmContext [in] : The current box state
			 * \return The clock frequency to call this algorithm
			 * \note Default implementation returns 0
			 *
			 * This function is used for algorithms that are triggered on
			 * clock signals. The frequency is given in Hz, with 32:32 fixed
			 * point representation thus returning \c (1<<32) will make the
			 * algorithm to be called every second, returning \c (100<<32)
			 * will make the algorithm being called 100 times a second,
			 * returning \c (1<<31) will make the algorithm be called once
			 * every two seconds and so on...
			 *
			 * \note Returning 0 means the algorithm should not be
			 *       clock activated.
			 *
			 * \sa processClock
			 */
			virtual OpenViBE::uint64 getClockFrequency(
				OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext) { return 0; }

			//@}
			/** \name Initialization / Uninitialization */
			//@{

			/**
			 * \brief Prepares plugin object
			 * \param rBoxAlgorithmContext [in] : the plugin object context
			 * \return \e true when this object successfully initialized
			 *         or \e false if it didn't succeed to initialize.
			 * \note Default implementation simply returns \e true.
			 *
			 * After a successfull initialization, the caller knows
			 * the object can safely be used... On failure, this object
			 * should be ready to be uninitialized and then released.
			 *
			 * \sa uninitialize
			 */
			virtual OpenViBE::boolean initialize(
				OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext) { return true; }
			/**
			 * \brief Unprepares the object so it can be deleted
			 * \param rBoxAlgorithmContext [in] : the plugin object context
			 * \return \e true when this object sucessfully uninitialized
			 *         or \e false if didn't succeed to uninitialize.
			 * \note Default implementation simply returns \e true.
			 *
			 * If this function returns \e false, it means it could not
			 * uninitialize the object members correctly. Thus, the
			 * object won't be released and none of its method will
			 * be called anymore for security. Note that this results
			 * in memory leaks. This is why this method should return
			 * \e true as often as possible.
			 *
			 * \sa initialize
			 */
			virtual OpenViBE::boolean uninitialize(
				OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext) { return true; }

			//@}
			/** \name Several event processing callbacks */
			//@{

			/**
			 * \brief Reaction to an event launched by another box
			 * \param rBoxAlgorithmContext [in] : the box algorithm context to use
			 * \param rMessageEvent [in] : the message the box just received
			 * \return \e true when the message is processed.
			 * \return \e false when the message is not processed.
			 * \note Default implementation returns \e false
			 *
			 * This function is called by the §OpenViBE§ kernel when
			 * another box tries to send an event message to this
			 * box. This event message is described in the
			 * rMessageEvent parameter and can be interpreted by this
			 * algorithm.
			 *
			 * \sa OpenViBE::IBoxAlgorithmContext
			 */
			virtual OpenViBE::boolean processEvent(
				OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext,
				OpenViBE::Kernel::IMessageEvent& rMessageEvent) { return false; }
			/**
			 * \brief Reaction to a signal
			 * \param rBoxAlgorithmContext [in] : the box algorithm context to use
			 * \param rMessageSignal [in] : the signal the box just received
			 * \return \e true when the message is processed.
			 * \return \e false when the message is not processed.
			 * \note Default implementation returns \e false
			 *
			 * This function is called by the §OpenViBE§ kernel when
			 * it has sent a signal. Signal are special messages,
			 * mainly sent by the kernel to all of the §OpenViBE§
			 * boxes in order to tell them it is about to start,
			 * processing, stop processing, load a new scenario
			 * and so on...
			 *
			 * \sa OpenViBE::Kernel::IBoxAlgorithmContext
			 */
			virtual OpenViBE::boolean processSignal(
				OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext,
				OpenViBE::Kernel::IMessageSignal& rMessageSignal) { return false; }
			/**
			 * \brief Reaction to a clock tick
			 * \param rBoxAlgorithmContext [in] : the box algorithm context to use
			 * \param rMessageClock [in] : the clock message the box received
			 * \return \e true when the message is processed.
			 * \return \e false when the message is not processed.
			 * \note Default implementation returns \e false
			 *
			 * This function is called by the §OpenViBE§ kernel when
			 * it has sent clock messages. Clock messages are used for
			 * processes that should be executed regularly and which
			 * can not be triggered thanks to their inputs (for example
			 * acquisition modules). They also can be used for example
			 * when viewing inputs on smaller range than what input
			 * sends periodically, thus needing a moving
			 * 'viewed-window' on lastly received data.
			 *
			 * \sa OpenViBE::Kernel::IBoxAlgorithmContext
			 * \sa getClockFrequency
			 */
			virtual OpenViBE::boolean processClock(
				OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext,
				OpenViBE::Kernel::IMessageClock& rMessageClock) { return false; }
			/**
			 * \brief Reaction to an input update
			 * \param rBoxAlgorithmContext [in] : the box algorithm context to use
			 * \param ui32InputIndex [in] : the index of the input which has ben updated
			 * \return \e true when the message is processed.
			 * \return \e false when the message is not processed.
			 * \note Default implementation returns \e false
			 *
			 * This function is called by the §OpenViBE§ kernel each
			 * time an input of this box is updated. This allows the
			 * algorithm to decide to call the process function and
			 * eventually to the received data.
			 *
			 * \sa OpenViBE::Kernel::IBoxAlgorithmContext
			 */
			virtual OpenViBE::boolean processInput(
				OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext,
				OpenViBE::uint32 ui32InputIndex) { return false; }

			//@}
			/** \name Algorithm processing */
			//@{

			/**
			 * \brief Processing function
			 * \param rBoxAlgorithmContext [in] : the box algorithm context to use
			 * \return \e true on success, \e false when something went wrong.
			 *
			 * This function is used to process the arrived data and
			 * eventually generate results. See §OpenViBE§ global
			 * architecture to understand how the commponents interact
			 * and how an §OpenViBE§ box works internally.
			 *
			 * The processing function may use the provided context
			 * in order to read its inputs and write its outputs...
			 * Also it could use the provided context to send
			 * messages/events to other boxes. Finally, it may use
			 * the provided context in order to perform rendering
			 * tasks !
			 *
			 * \sa OpenViBE::Kernel::IBoxAlgorithmContext
			 */
			virtual OpenViBE::boolean process(
				OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext)=0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Plugins::IPluginObject, OV_ClassId_Plugins_BoxAlgorithm)
		};

		class OV_API IBoxListener : public OpenViBE::Plugins::IPluginObject
		{
		public:

			virtual ~IBoxListener(void)
			{
			}

			virtual void release(void)
			{
			}

			virtual OpenViBE::boolean initialize(
				OpenViBE::Kernel::IBoxListenerContext& rBoxListenerContext)
			{
				return true;
			}

			virtual OpenViBE::boolean uninitialize(
				OpenViBE::Kernel::IBoxListenerContext& rBoxListenerContext)
			{
				return true;
			}

			/** \name Box modifications callbacks */
			//@{

			/**
			 * \brief This callback is called when the box is modified in some way
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \param rBoxListenerContext [in] : the box listener context
			 *        containing the description of the box
			 * \param eBoxModificationType [in] : the type of modification
			 *        applied to the box
			 *
			 * This function is called as soon as a modification is done
			 * on the box which this listener is attached to. The box listener
			 * is then allowed to examine and check box status validity
			 * and to adpat the box itself according to this validity.
			 *
			 * \sa OpenViBE::Plugins::IBoxProto
			 * \sa OpenViBE::Plugins::IBoxListenerContext
			 * \sa OpenViBE::Plugins::EBoxModification
			 */
			virtual OpenViBE::boolean process(
				OpenViBE::Kernel::IBoxListenerContext& rBoxListenerContext,
				const OpenViBE::Kernel::EBoxModification eBoxModificationType)=0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Plugins::IPluginObject, OV_ClassId_Plugins_BoxListener)
		};
	};
};

#endif // __OpenViBE_Plugins_IBoxAlgorithm_H__
