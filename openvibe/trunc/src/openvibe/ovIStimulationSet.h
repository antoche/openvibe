#ifndef __OpenViBE_IStimulationSet_H__
#define __OpenViBE_IStimulationSet_H__

#include "ovIObject.h"

namespace OpenViBE
{
	/**
	 * \class IStimulationSet
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2007-11-21
	 * \brief Basic §OpenViBE§ stimulation set interface
	 *
	 * This interface offers functionalities to handle a collection of §OpenViBE§ stimulations.
	 * This collection basicaly consists in a list of stimulation information. Each stimulation
	 * has three information : an identifier, a date and a duration.
	 *
	 * §OpenViBE§ provides an standalone implementation of this interface in OpenViBE::CStimulationSet
	 */

	class OV_API IStimulationSet : public OpenViBE::IObject
	{
	public:

		/**
		 * \brief Clears this stimulation set removing every existing stimulation
		 */
		virtual void clear(void)=0;
		/**
		 * \brief Gets the number of stimulations contained in this stimulation set
		 * \return the number of stimulations contained in this stimulation set.
		 */
		virtual const OpenViBE::uint64 getStimulationCount(void) const=0;
		/**
		 * \brief Gets the identifier of a specific stimulation in this stimulation set
		 * \param ui64StimulationIndex [in] : the index of the stimulation which identifier has to be returned
		 * \return the identifier for the specified stimulation.
		 */
		virtual const OpenViBE::uint64 getStimulationIdentifier(
			const OpenViBE::uint64 ui64StimulationIndex) const=0;
		/**
		 * \brief Gets the date of a specific stimulation in this stimulation set
		 * \param ui64StimulationIndex [in] : the index of the stimulation which date has to be returned
		 * \return the date for the specified stimulation.
		 * \note the returned date is relative to the begining of this stimulation set
		 * \note dates and durations are returned in seconds fixed point 32:32. Refer to \Doc_TimeManagement for more details
		 */
		virtual const OpenViBE::uint64 getStimulationDate(
			const OpenViBE::uint64 ui64StimulationIndex) const=0;
		/**
		 * \brief Gets the duration of a specific stimulation in this stimulation set
		 * \param ui64StimulationIndex [in] : the index of the stimulation which duration has to be returned
		 * \return the duration for the specified stimulation.
		 * \note dates and durations are returned in seconds fixed point 32:32. Refer to \Doc_TimeManagement for more details
		 */
		virtual const OpenViBE::uint64 getStimulationDuration(
			const OpenViBE::uint64 ui64StimulationIndex) const=0;
		/**
		 * \brief Changes the stimulation count in this stimulation set
		 * \param ui64StimulationCount [in] : the new number of stimulations
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 */
		virtual OpenViBE::boolean setStimulationCount(
			const OpenViBE::uint64 ui64StimulationCount)=0;
		/**
		 * \brief Changes the identifier of a specific stimulation in this stimulation set
		 * \param ui64StimulationIndex [in] : the index of the stimulation which identifier should be changed
		 * \param ui64StimulationIdentifier [in] : the new identifier for the specified stimulation
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 */
		virtual OpenViBE::boolean setStimulationIdentifier(
			const OpenViBE::uint64 ui64StimulationIndex,
			const OpenViBE::uint64 ui64StimulationIdentifier)=0;
		/**
		 * \brief Changes the date of a specific stimulation in this stimulation set
		 * \param ui64StimulationIndex [in] : the index of the stimulation which date should be changed
		 * \param ui64StimulationDate [in] : the new date for the specified stimulation
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 * \note dates and durations are returned in seconds fixed point 32:32. Refer to \Doc_TimeManagement for more details
		 */
		virtual OpenViBE::boolean setStimulationDate(
			const OpenViBE::uint64 ui64StimulationIndex,
			const OpenViBE::uint64 ui64StimulationDate)=0;
		/**
		 * \brief Changes the duration of a specific stimulation in this stimulation set
		 * \param ui64StimulationIndex [in] : the index of the stimulation which duration should be changed
		 * \param ui64StimulationDuration [in] : the new duration for the specified stimulation
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 * \note the returned date is relative to the begining of this stimulation set
		 * \note dates and durations are returned in seconds fixed point 32:32. Refer to \Doc_TimeManagement for more details
		 */
		virtual OpenViBE::boolean setStimulationDuration(
			const OpenViBE::uint64 ui64StimulationIndex,
			const OpenViBE::uint64 ui64StimulationDuration)=0;
		/**
		 * \brief Appends a stimulation to this stimulation set
		 * \param ui64StimulationIdentifier [in] : the identifier of the stimulation to append
		 * \param ui64StimulationDate [in] : the date of the stimulation
		 * \param ui64StimulationDuration [in] : the duration of the stimulation
		 * \return the index of the appended stimulation in this stimulation set
		 */
		virtual OpenViBE::uint64 appendStimulation(
			const OpenViBE::uint64 ui64StimulationIdentifier,
			const OpenViBE::uint64 ui64StimulationDate,
			const OpenViBE::uint64 ui64StimulationDuration)=0;
		/**
		 * \brief Inserts a stimulation to this stimulation set
		 * \param ui64StimulationIndex [in] : the index of the stimulation to insert
		 * \param ui64StimulationIdentifier [in] : the identifier of the stimulation
		 * \param ui64StimulationDate [in] : the date of the stimulation
		 * \param ui64StimulationDuration [in] : the duration of the stimulation
		 * \return \e true in case of success
		 * \return \e false in case of error
		 * \warning stimulation indexing change after call to this function :
		 *          following stimulation(s) get one more indexed.
		 */
		virtual OpenViBE::uint64 insertStimulation(
			const OpenViBE::uint64 ui64StimulationIndex,
			const OpenViBE::uint64 ui64StimulationIdentifier,
			const OpenViBE::uint64 ui64StimulationDate,
			const OpenViBE::uint64 ui64StimulationDuration)=0;
		/**
		 * \brief Removes a stimulation from this stimulation set
		 * \param ui64StimulationIndex [in] : the index of the stimulation to remove
		 * \return \e true in case of success
		 * \return \e false in case of error
		 * \warning stimulation indexing change after call to this function :
		 *          following stimulation(s) get one less indexed.
		 */
		virtual OpenViBE::boolean removeStimulation(
			const OpenViBE::uint64 ui64StimulationIndex)=0;

		_IsDerivedFromClass_(OpenViBE::IObject, OV_ClassId_StimulationSet);

	public:

		static OpenViBE::uint64 sampleIndexToTime(OpenViBE::uint32 ui32SamplingRate, OpenViBE::uint64 ui64SampleIndex)
		{
			return ((ui64SampleIndex<<32)+(ui32SamplingRate-1))/ui32SamplingRate;
		}

		static OpenViBE::uint64 timeToSampleIndex(OpenViBE::uint32 ui32SamplingRate, OpenViBE::uint64 ui64Time)
		{
			return (ui64Time*ui32SamplingRate)>>32;
		}
	};
};

#endif // __OpenViBE_IStimulationSet_H__
