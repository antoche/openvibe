#ifndef __OpenViBE_AcquisitionServer_IHeader_H__
#define __OpenViBE_AcquisitionServer_IHeader_H__

#include "ovas_base.h"

namespace OpenViBEAcquisitionServer
{
	/**
	 * \class IHeader
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2007-04-01
	 * \brief Base class for an §OpenViBE§ header container
	 *
	 * IHeader objects are used by IDriver objects to give all the header
	 * information to the acquisition server. The IDriver developer may
	 * implement his own IHeader derived class or use the one provided
	 * with the acquisition server. To get a standard header, refer to
	 * the \c CHeader class.
	 *
	 * The IHeader objects mainly consist in get/set/isSet functions
	 * that allow user code to modify, read back and check state of some
	 * single header information.
	 *
	 * \sa IDriver
	 * \sa IDriver::getHeader
	 * \sa CHeader
	 */
	class IHeader
	{
	public:

		/** \name General purpose functions */
		//@{

		/**
		 * \brief Resets this header
		 *
		 * When called, this function resets all the header content to its
		 * default values. Most of the is*Set will return \e false after
		 * this call.
		 */
		virtual void reset(void)=0;

		//@}
		/** \name Experimentation information */
		//@{

		/**
		 * \brief Sets the experiment identifier
		 * \param ui32ExperimentIdentifier [in] : the experiment identifier to send to
		 *        the §OpenViBE§ platform.
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 *
		 * The experiment identifier may be used by the platform
		 * to get details from a database, for example a description
		 * of the experiment, what is done, where etc...
		 */
		virtual OpenViBE::boolean setExperimentIdentifier(const OpenViBE::uint32 ui32ExperimentIdentifier)=0;
		/**
		 * \brief Sets the subject age
		 * \param ui32SubjectAge [in] : the subject age in years
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 */
		virtual OpenViBE::boolean setSubjectAge(const OpenViBE::uint32 ui32SubjectAge)=0;
		/**
		 * \brief Sets the subject gender
		 * \param ui32SubjectGender [in] : the subject gender
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 *
		 * The subject gender is given as an integer and should
		 * be ISO 5218 conformant... Allowed values are :
		 *  - 0 : unknown
		 *  - 1 : male
		 *  - 2 : female
		 *  - 3 : not specified
		 *
		 * \note This values are defined in the OpenViBE toolkit.
		 */
		virtual OpenViBE::boolean setSubjectGender(const OpenViBE::uint32 ui32SubjectGender)=0;
		/**
		 * \brief Gets the experiment identifier
		 * \return the experiement identifier.
		 * \sa setExperimentIdentifier
		 */
		virtual OpenViBE::uint32 getExperimentIdentifier(void) const=0;
		/**
		 * \brief Gets the subject age
		 * \return the subject age.
		 * \sa setSubjectAge
		 */
		virtual OpenViBE::uint32 getSubjectAge(void) const=0;
		/**
		 * \brief Gets the subject gender
		 * \return the subject gender.
		 * \sa setSubjectGender
		 */
		virtual OpenViBE::uint32 getSubjectGender(void) const=0;
		/**
		 * \brief Tests if experiment identifier has been set
		 * \return \e true if experiment identifier has been set since last \c reset.
		 * \return \e false if experiment identifier has not been set.
		 * \sa setExperimentIdentifier
		 */
		virtual OpenViBE::boolean isExperimentIdentifierSet(void) const=0;
		/**
		 * \brief Tests if subject age has been set
		 * \return \e true if the subject age has been set since last \c reset.
		 * \return \e false if the subject age has not been set.
		 * \sa setSubjectAge
		 */
		virtual OpenViBE::boolean isSubjectAgeSet(void) const=0;
		/**
		 * \brief Tests if subject gender has been set
		 * \return \e true if the subject gender has been set since last \c reset.
		 * \return \e false if the subject gender has not been set.
		 * \sa setSubjectGender
		 */
		virtual OpenViBE::boolean isSubjectGenderSet(void) const=0;

		//@}
		/** \name Chanel information */
		//@{

		/**
		 * \brief Sets channel count for the recorded signal
		 * \param ui32ChannelCount [in] : the number of the channel for the recorder signal
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 *
		 * The number of channels will be used by the IDriver and the acquisition server
		 * to calculate the sample buffer size (that is \c nSamplesPerChannel x \c nChannel).
		 */
		virtual OpenViBE::boolean setChannelCount(const OpenViBE::uint32 ui32ChannelCount)=0;
		/**
		 * \brief Sets a channel' name
		 * \param ui32ChannelIndex [in] : the index of the channel which name should be set
		 * \param sChannelName [in] : the new name for this channel
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 * \note As soon as a channel name is set, all the yet-unset channel names are
		 *       considered to be set to empty string.
		 */
		virtual OpenViBE::boolean setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName)=0;
		/**
		 * \brief Sets a channel' gain
		 * \param ui32ChannelIndex [in] : the index of the channel which gain should be set
		 * \param f32ChannelGain [in] : the gain value for this channel
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 * \note As soon as a channel gain is set, all the yet-unset channel gains are
		 *       considered to be set to 1.
		 *
		 * Gains are multiplicator coefficients that are used by the §OpenViBE§ platform to
		 * to transform measured values into physical dimmension.
		 */
		virtual OpenViBE::boolean setChannelGain(const OpenViBE::uint32 ui32ChannelIndex, const OpenViBE::float32 f32ChannelGain)=0;
		/// \todo setChannelLocation
		// virtual OpenViBE::boolean setChannelLocation(const OpenViBE::uint32 ui32ChannelIndex, const OpenViBE::float32 ui32ChannelLocationX, const OpenViBEAcquisitionServer::float32 ui32ChannelLocationY, const OpenViBEAcquisitionServer::float32 ui32ChannelLocationZ)=0;
		/**
		 * \brief Gets the number of channels for this header
		 * \return the number of channels.
		 * \sa setChannelCount
		 */
		virtual OpenViBE::uint32 getChannelCount(void) const=0;
		/**
		 * \brief Gets the name of a channel
		 * \param ui32ChannelIndex [in] : the index of the channel which name is wanted
		 * \return the name of the \c ui32ChannelIndex th channel if in the correct range
		 *         and name has been specified.
		 * \return an empty string if in the correct range but name has not been specified.
		 * \return an empty string when \c ui32ChannelIndex is out of range.
		 * \sa setChannelName
		 */
		virtual const char* getChannelName(const OpenViBE::uint32 ui32ChannelIndex) const=0;
		/**
		 * \brief Gets the gain of a channel
		 * \param ui32ChannelIndex [in] : the index of the channel which gain is wanted
		 * \return the gain of the \c ui32ChannelIndex th channel if in the correct range
		 *         and gain has been specified.
		 * \return 1 if in the correct range but gain has not been specified.
		 * \return 0 when \c ui32ChannelIndex is out of range.
		 * \sa setChannelGain
		 */
		virtual OpenViBE::float32 getChannelGain(const OpenViBE::uint32 ui32ChannelIndex) const=0;
		/// \todo getChannelLocation
		// virtual getChannelLocation(const OpenViBE::uint32 ui32ChannelIndex) const=0;
		/**
		 * \brief Tests if channel count has been set
		 * \return \e true if channel count has been set since last \c reset.
		 * \return \e false if channel count has not been set.
		 * \sa setChannelCount
		 */
		virtual OpenViBE::boolean isChannelCountSet(void) const=0;
		/**
		 * \brief Tests if channel name has been set at least once
		 * \return \e true if channel name has been set at least once since last \c reset.
		 * \return \e false if channel name has not been set.
		 * \sa setChannelName
		 */
		virtual OpenViBE::boolean isChannelNameSet(void) const=0;
		/**
		 * \brief Tests if channel gain has been set at least once
		 * \return \e true if channel gain has been set at least once since last \c reset.
		 * \return \e false if channel gain has not been set.
		 * \sa setChannelGain
		 */
		virtual OpenViBE::boolean isChannelGainSet(void) const=0;
		/// \todo isChannelLocationSet
		// virtual OpenViBE::boolean isChannelLocationSet(void) const=0;

		//@}
		/** \name Samples information */
		//@{

		/**
		 * \brief Sets measured signal sampling rate
		 * \param ui32SamplingFrequency [in] : the sampling rate for the measured signal
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 * \note the sampling rate is a global value. It can not be specified per channel.
		 */
		virtual OpenViBE::boolean setSamplingFrequency(const OpenViBE::uint32 ui32SamplingFrequency)=0;
		/**
		 * \brief Gets the sampling rate of the measured signal
		 * \return the sampling rate of the measured signal
		 * \sa setSamplingFrequency
		 */
		virtual OpenViBE::uint32 getSamplingFrequency(void) const=0;
		/**
		 * \brief Tests if sampling frequency has been set
		 * \return \e true if sampling frequency has been set since last \c reset.
		 * \return \e false if sampling frequency has not been set.
		 * \sa setSamplingFrequency
		 */
		virtual OpenViBE::boolean isSamplingFrequencySet(void) const=0;

		//@}

		/**
		 * \brief Destructor
		 */
		virtual ~IHeader(void) { }

	public:

		static void copy(OpenViBEAcquisitionServer::IHeader& rDestination, const OpenViBEAcquisitionServer::IHeader& rSource)
		{
			OpenViBE::uint32 i, l_ui32ChannelCount=rSource.getChannelCount();
			rDestination.setExperimentIdentifier(rSource.getExperimentIdentifier());
			rDestination.setSubjectAge(rSource.getSubjectAge());
			rDestination.setSubjectGender(rSource.getSubjectGender());
			rDestination.setChannelCount(rSource.getChannelCount());
			rDestination.setSamplingFrequency(rSource.getSamplingFrequency());
			rDestination.setChannelCount(rSource.getChannelCount());
			for(i=0; i<l_ui32ChannelCount; i++)
			{
				rDestination.setChannelName(i, rSource.getChannelName(i));
				rDestination.setChannelGain(i, rSource.getChannelGain(i));
			}
		}
	};
};

#endif // __OpenViBE_AcquisitionServer_IHeader_H__
