#ifndef __OpenViBEPlugins_SimpleVisualisation_IStreamDatabase_H__
#define __OpenViBEPlugins_SimpleVisualisation_IStreamDatabase_H__

#include "ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		class IStreamDisplayDrawable;

		/**
		 * \brief Abstract class of objects than can be updated by an IStreamDatabase object
		 */
		class IStreamDisplayDrawable
		{
		public:
			virtual OpenViBE::boolean init() = 0;
			virtual OpenViBE::boolean redraw() = 0;
			virtual ~IStreamDisplayDrawable(){}
		};

		class IStreamDatabase
		{
		public:
			virtual ~IStreamDatabase() {}

			/**
			 * \brief Initialize the database, including creating decoder
			 * \return True if initialization succeeded
			 */
			virtual OpenViBE::boolean initialize() = 0;

			/**
			 * \brief Set drawable object to update.
			 * \param pDrawable drawable object to update.
			 */
			virtual void setDrawable(
				IStreamDisplayDrawable* pDrawable) = 0;

			/**
			 * \brief Set flag stating whether or not to redraw upon new data reception
			 * \param bRedrawOnNewData Redraw flag
			 */
			virtual void setRedrawOnNewData(
				OpenViBE::boolean bRedrawOnNewData) = 0;

			/**
			 * \brief Determine whether first buffer has been received yet
			 * \return True if first buffer has been received already, false otherwise
			 */
			virtual OpenViBE::boolean isFirstBufferReceived() = 0;

			/**
			 * \brief Set max buffer count
			 * Set max buffer count directly (as opposed to computing it from time scale)
			 * \remarks This method sets m_bIgnoreTimeScale to true
			 */
			virtual OpenViBE::boolean setMaxBufferCount(
				OpenViBE::uint32 ui32MaxBufferCount) = 0;

			/**
			 * \brief Set time scale
			 * Computes the maximum number of buffers that can be displayed simultaneously
			 * \remarks This method sets m_bIgnoreTimeScale to false
			 * \param f64TimeScale Time window's width in seconds.
			 * \return True if buffer count changed, false otherwise
			 */
			virtual OpenViBE::boolean setTimeScale(
				OpenViBE::float64 f64TimeScale) = 0;

			/**
			 * \brief Decode a memory buffer using proxy
			 * \param pMemoryBuffer Memory buffer to decode
			 * \param ui64StartTime Start time of memory buffer
			 * \param ui64EndTime End time of memory buffer
			 * \return True if memory buffer could be properly decoded, false otherwise
			 */
			virtual OpenViBE::boolean decodeMemoryBuffer(
				const OpenViBE::IMemoryBuffer* pMemoryBuffer,
				OpenViBE::uint64 ui64StartTime,
				OpenViBE::uint64 ui64EndTime) = 0;

			/**
			 * \brief Get number of buffers necessary to cover time scale
			 * \remarks Can't be computed before 2 buffers have been received, because
			 * the time step between the start of 2 consecutive buffers must be known
			 * \return Maximum number of buffers stored in this object
			 */
			virtual OpenViBE::uint32 getMaxBufferCount() = 0;

			/**
			 * \brief Get current buffer count
			 * \return Current buffer count
			 */
			virtual OpenViBE::uint32 getCurrentBufferCount() = 0;

			/**
			 * \brief Get pointer on a given buffer
			 * \param ui32Index Index of buffer to retrieve
			 * \return Buffer pointer if buffer exists, NULL otherwise
			 */
			virtual const OpenViBE::float64* getBuffer(
				OpenViBE::uint32 ui32Index) = 0;

			/**
			 * \brief Get start time of a given buffer
			 * \param ui32BufferIndex Index of buffer whose start time is to be retrieved
			 * \return Start time if buffer exists, 0 otherwise
			 */
			virtual OpenViBE::uint64 getStartTime(
				OpenViBE::uint32 ui32BufferIndex) = 0;

			/**
			 * \brief Get end time of a given buffer
			 * \param ui32BufferIndex Index of buffer whose end time is to be retrieved
			 * \return End time if buffer exists, 0 otherwise
			 */
			virtual OpenViBE::uint64 getEndTime(
				OpenViBE::uint32 ui32BufferIndex) = 0;

			/**
			 * \brief Get number of elements contained in a buffer
			 * \return Buffer element count or 0 if no buffer has been received yet
			 */
			virtual OpenViBE::uint32 getBufferElementCount() = 0;

			/**
			 * \brief Get time span covered by a buffer
			 * \return Buffer time span
			 */
			virtual OpenViBE::uint64 getBufferDuration() = 0;

			/**
			 * \brief Determine whether buffer time step has been computed yet
			 * \return True if buffer time step has been computed
			 */
			virtual OpenViBE::boolean isBufferTimeStepComputed() = 0;

			/**
			 * \brief Get time step between the start of 2 consecutive buffers
			 * \remarks This value can't be computed before the first 2 buffers are received
			 * \return Buffer time step
			 */
			virtual OpenViBE::uint64 getBufferTimeStep() = 0;

			/**
			 * \brief Get number of samples per buffer
			 * \return Number of samples per buffer
			 */
			virtual OpenViBE::uint32 getSampleCountPerBuffer() = 0;

			/**
			 * \brief Get number of channels
			 * \return Number of channels
			 */
			virtual OpenViBE::uint32 getChannelCount() = 0;

			/**
			 * \brief Get channel label
			 * \param[in] ui32ChannelIndex index of channel
			 * \param[out] rChannelLabel channel label
			 * \return true if channel label could be retrieved, false otherwise
			 */
			virtual OpenViBE::boolean getChannelLabel(
				const OpenViBE::uint32 ui32ChannelIndex,
				OpenViBE::CString& rElectrodeLabel) = 0;

			/** \name Min/max values retrieval */
			//@{

			/**
			 * \brief Compute min/max values currently displayed for a given channel
			 * \param [in] ui32Channel Index of channel
			 * \param [out] f64Min Minimum displayed value for channel of interest
			 * \param [out] f64Max Maximum displayed value for channel of interest
			 * \return True if values could be computed, false otherwise
			 */
			virtual OpenViBE::boolean getChannelMinMaxValues(
				OpenViBE::uint32 ui32Channel,
				OpenViBE::float64& f64Min,
				OpenViBE::float64& f64Max) = 0;

			/**
			 * \brief Compute min/max values currently displayed, taking all channels into account
			 * \param [out] f64Min Minimum displayed value
			 * \param [out] f64Max Maximum displayed value
			 * \return True if values could be computed, false otherwise
			 */
			virtual OpenViBE::boolean getGlobalMinMaxValues(
				OpenViBE::float64& f64Min,
				OpenViBE::float64& f64Max) = 0;

			/**
			 * \brief Compute min/max values in last buffer for a given channel
			 * \param [in] ui32Channel Index of channel
			 * \param [out] f64Min Minimum value for channel of interest
			 * \param [out] f64Max Maximum value for channel of interest
			 * \return True if values could be computed, false otherwise
			 */
			virtual OpenViBE::boolean getLastBufferChannelMinMaxValues(
				OpenViBE::uint32 ui32Channel,
				OpenViBE::float64& f64Min,
				OpenViBE::float64& f64Max) = 0;

			/**
			 * \brief Compute min/max values in last buffer, taking all channels into account
			 * \param [out] f64Min Minimum value
			 * \param [out] f64Max Maximum value
			 * \return True if values could be computed, false otherwise
			 */
			virtual OpenViBE::boolean getLastBufferGlobalMinMaxValues(
				OpenViBE::float64& f64Min,
				OpenViBE::float64& f64Max) = 0;

			//@}
		};
	}
}

#endif
