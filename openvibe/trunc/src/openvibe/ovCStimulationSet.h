#ifndef __OpenViBE_CStimulationSet_H__
#define __OpenViBE_CStimulationSet_H__

#include "ovIStimulationSet.h"

namespace OpenViBE
{
	/**
	 * \class CStimulationSet
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2007-11-21
	 * \brief Basic standalone §OpenViBE§ stimulation set implementation
	 * \ingroup Group_Base
	 *
	 * This class offers a basic standalone impementation of the OpenViBE::IStimulationSet
	 * interface. This class can be directly instanciated and used.
	 *
	 * Instances of this class use an internal implementation of the OpenViBE::IStimulationSet
	 * interface and redirect their calls to this implementation.
	 */
	class OV_API CStimulationSet : public OpenViBE::IStimulationSet
	{
	public:

		/** \name Constructors */
		//@{

		/**
		 * \brief Default constructor
		 *
		 * This constructor builds the internal implementation of this stimulation set.
		 */
		CStimulationSet(void);
		/**
		 * \brief Destructor
		 *
		 * The internal implementation is released.
		 */
		virtual ~CStimulationSet(void);

		//@}

		virtual void clear(void);
		virtual const OpenViBE::uint64 getStimulationCount(void) const;
		virtual const OpenViBE::uint64 getStimulationIdentifier(
			const OpenViBE::uint64 ui64StimulationIndex) const;
		virtual const OpenViBE::uint64 getStimulationDate(
			const OpenViBE::uint64 ui64StimulationIndex) const;
		virtual const OpenViBE::uint64 getStimulationDuration(
			const OpenViBE::uint64 ui64StimulationIndex) const;

		virtual OpenViBE::boolean setStimulationCount(
			const OpenViBE::uint64 ui64StimulationCount);
		virtual OpenViBE::boolean setStimulationIdentifier(
			const OpenViBE::uint64 ui64StimulationIndex,
			const OpenViBE::uint64 ui64StimulationIdentifier);
		virtual OpenViBE::boolean setStimulationDate(
			const OpenViBE::uint64 ui64StimulationIndex,
			const OpenViBE::uint64 ui64StimulationDate);
		virtual OpenViBE::boolean setStimulationDuration(
			const OpenViBE::uint64 ui64StimulationIndex,
			const OpenViBE::uint64 ui64StimulationDuration);

		virtual OpenViBE::uint64 appendStimulation(
			const OpenViBE::uint64 ui64StimulationIdentifier,
			const OpenViBE::uint64 ui64StimulationDate,
			const OpenViBE::uint64 ui64StimulationDuration);
		virtual OpenViBE::uint64 insertStimulation(
			const OpenViBE::uint64 ui64StimulationIndex,
			const OpenViBE::uint64 ui64StimulationIdentifier,
			const OpenViBE::uint64 ui64StimulationDate,
			const OpenViBE::uint64 ui64StimulationDuration);
		virtual OpenViBE::boolean removeStimulation(
			const OpenViBE::uint64 ui64StimulationIndex);

		_IsDerivedFromClass_Final_(OpenViBE::IStimulationSet, OV_ClassId_StimulationSetBridge);

	private:

		OpenViBE::IStimulationSet* m_pStimulationSetImpl; //!< Internal implementation
	};
};

#endif // __OpenViBE_CStimulationSet_H__
