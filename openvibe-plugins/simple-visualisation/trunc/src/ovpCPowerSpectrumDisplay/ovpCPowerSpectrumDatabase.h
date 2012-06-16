#ifndef __OpenViBEPlugins_SimpleVisualisation_CPowerSpectrumDatabase_H__
#define __OpenViBEPlugins_SimpleVisualisation_CPowerSpectrumDatabase_H__

#include "../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/IReader.h>

#include <vector>
#include <deque>
#include <queue>
#include <string>
#include <cfloat>
#include <iostream>

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		class CSignalDisplayDrawable;

		/**
		* This class is used to store information about the incoming spectrum stream. It can request a CSignalDisplayDrawable
		* object to redraw himself in case of some changes in its data.
		*/
		class CPowerSpectrumDatabase
		{
		public:
			/**
			 * \brief Constructor
			 * \param oPlugin Reference to parent plugin
			 */
			CPowerSpectrumDatabase(
				OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>& oPlugin);

			/**
			 * \brief Destructor
			 */
			~CPowerSpectrumDatabase();

			/**
			* \brief Set the drawable object to update.
			* \param pDrawable drawable object to update.
			*/
			void setDrawable(
				CSignalDisplayDrawable* pDrawable);

			/**
			 * \name IBoxAlgorithmSpectrumInputReaderCallback::ICallback callbacks implementation
			 */
			//@{

			/**
			 * \brief Set number of channels
			 */
			void setChannelCount(
				const OpenViBE::uint32 ui32ChannelCount);

			/**
			 * \brief Set name of a channel
			 * \param ui32ChannelIndex Index of channel
			 * \param sChannelName Channel name
			 */
			void setChannelName(
				const OpenViBE::uint32 ui32ChannelIndex,
				const char* sChannelName);

			/**
			 * \brief Set frequency band count
			 * \param ui32FrequencyBandCount Number of frequency bands
			 */
			void setFrequencyBandCount(
				const OpenViBE::uint32 ui32FrequencyBandCount);

			/**
			 * \brief Set name of a frequency band
			 * \param ui32FrequencyBandIndex Index of frequency band
			 * \param sFrequencyBandName Frequency band name
			 */
			void setFrequencyBandName(
				const OpenViBE::uint32 ui32FrequencyBandIndex,
				const char* sFrequencyBandName);

			/**
			 * \brief Set frequency band start frequency
			 * \param ui32FrequencyBandIndex Index of frequency band
			 * \param f64FrequencyBandStart Start frequency
			 */
			void setFrequencyBandStart(
				const OpenViBE::uint32 ui32FrequencyBandIndex,
				const OpenViBE::float64 f64FrequencyBandStart);

			/**
			 * \brief Set frequency band stop frequency
			 * \param ui32FrequencyBandIndex Index of frequency band
			 * \param f64FrequencyBandStop Stop frequency
			 */
			void setFrequencyBandStop(
				const OpenViBE::uint32 ui32FrequencyBandIndex,
				const OpenViBE::float64 f64FrequencyBandStop);

			/**
			 * \brief Set data buffer
			 * \param pbuffer Pointer to data buffer
			 */
			void setBuffer(
				const OpenViBE::float64* pBuffer);

			//@}

			/**
			 * \name Buffer data management
			 */
			//@{

			/**
			 * \brief Get min/max values of last buffer for a given channel
			 * \param [in] ui32Channel Index of channel
			 * \param [out] f64Min Minimum value for a given channel
			 * \param [out] f64Min Maximum value for a given channel
			 * \return True if values could be retrieved, false otherwise
			 */
			OpenViBE::boolean getLastBufferChannelMinMaxValue(
				OpenViBE::uint32 ui32Channel,
				OpenViBE::float64& f64Min,
				OpenViBE::float64& f64Max);

			/**
			 * \brief Get min/max values of last buffer for all channels
			 * \param [out] f64Min Minimum value found in last buffer
			 * \param [out] f64Max Maximum value found in last buffer
			 * \return True if values could be retrieved, false otherwise
			 */
			OpenViBE::boolean getLastBufferMinMaxValue(
				OpenViBE::float64& f64Min,
				OpenViBE::float64& f64Max);

			/**
			 * \brief Get pointer to last buffer data for a given channel
			 * \param [in] ui32Channel Index of channel which data is to be retrieved
			 * \return Pointer to buffer data if it exists, NULL otherwise
			 */
			OpenViBE::float64* getLastBufferChannelPointer(
				OpenViBE::uint32 ui32Channel);

			//@}

			/**
			 * \name Channels and frequency bands management
			 */
			//@{

			/**
			 * \brief Get number of channels
			 * \return Number of channels
			 */
			OpenViBE::uint32 getChannelCount();

			/**
			 * \brief Get the label of a channel
			 * \param [in] ui32ChannelIndex index of channel which label is to be retrieved
			 * \param [out] rChannelLabel channel label to be retrieved
			 * \return True if channel label could be retrieved, false otherwise
			 */
			OpenViBE::boolean getChannelLabel(
				OpenViBE::uint32 ui32ChannelIndex,
				OpenViBE::CString& rChannelLabel);

			/**
			 * \brief Set the minimum frequency displayed by the power spectrum plugin
			 * \remarks This frequency should lie in the range of frequencies received from data
			 * buffers, and it is taken into account when computing min/max amplitude values. Also,
			 * this function should be called after header information has been received, e.g. upon
			 * first buffer reception.
			 * \param [in] f64MinDisplayedFrequency Minimum displayed frequency
			 * \return True if frequency was successfully set, false otherwise
			 */
			OpenViBE::boolean setMinDisplayedFrequency(
				OpenViBE::float64 f64MinDisplayedFrequency);

			/**
			 * \brief Set the maximum frequency displayed by the power spectrum plugin
			 * \remarks This frequency should lie in the range of frequencies received from data
			 * buffers, and it is taken into account when computing min/max amplitude values. Also,
			 * this function should be called after header information has been received, e.g. upon
			 * first buffer reception.
			 * \param [in] f64MaxDisplayedFrequency Maximum displayed frequency
			 * \return True if frequency was successfully set, false otherwise
			 */
			OpenViBE::boolean setMaxDisplayedFrequency(
				OpenViBE::float64 f64MaxDisplayedFrequency);

			/**
			 * \brief Get range of frequencies received from incoming buffers
			 * \param [out] f64MinInputFrequency Minimum input frequency
			 * \param [out] f64MaxInputFrequency Maximum input frequency
			 * \return True if input frequency range, false otherwise
			 */
			OpenViBE::boolean getInputFrequencyRange(
				OpenViBE::float64& f64MinInputFrequency,
				OpenViBE::float64& f64MaxInputFrequency);

			/**
			 * \brief Get number of frequency bands displayed by power spectrum plugin
			 * \return number of frequency bands displayed
			 */
			OpenViBE::uint32 getDisplayedFrequencyBandCount();

			/**
			 * \brief Get index of minimum frequency band displayed by power spectrum plugin
			 * \return Index of minimum frequency band displayed by power spectrum plugin
			 */
			OpenViBE::uint32 getMinDisplayedFrequencyBandIndex();

			/**
			 * \brief Get index of maximum frequency band displayed by power spectrum plugin
			 * \return Index of maximum frequency band displayed by power spectrum plugin
			 */
			OpenViBE::uint32 getMaxDisplayedFrequencyBandIndex();

			/**
			 * \brief Get frequency band range
			 * \param [in] ui32FrequencyBandIndex Frequency band index
			 * \param [out] rFrequencyBandStart Frequency band start
			 * \param [out] rFrequencyBandStop Frequency band stop
			 * \return True if frequency band index is valid, false otherwise
			 */
			OpenViBE::boolean getFrequencyBandRange(
				OpenViBE::uint32 ui32FrequencyBandIndex,
				OpenViBE::float64& rFrequencyBandStart,
				OpenViBE::float64& rFrequencyBandStop);

			//@}

		private:
			// Parent plugin
			OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>& m_oParentPlugin;

			// Pointer to the drawable object to update
			CSignalDisplayDrawable* m_pDrawable;

			/**
			 * \name Buffer data
			 */
			//@{

			// Pointer to last buffer received
			OpenViBE::float64* m_pBuffer;
			// Flag set to true upon first buffer reception
			OpenViBE::boolean m_bFirstBufferReceived;

			//@}

			/**
			 * \name Channels and frequency bands data
			 */
			//@{

			// Vector of channel labels
			std::vector<std::string> m_pChannelLabels;
			// Vector of frequency band labels
			std::vector<std::string> m_pFrequencyBandLabels;
			// Vector of frequency band values (lower and upper frequencies for each band)
			std::vector<std::pair<OpenViBE::float64, OpenViBE::float64> > m_pFrequencyBands;
			// Index of minimum frequency band displayed in power spectrum plugin
			OpenViBE::uint32 m_ui32MinDisplayedFrequencyBand;
			// Index of maximum frequency band displayed in power spectrum plugin
			OpenViBE::uint32 m_ui32MaxDisplayedFrequencyBand;
			// Min/max displayed values per channel for last buffer
			std::vector<std::pair<OpenViBE::float64, OpenViBE::float64> > m_oMinMaxDisplayedValues;
			// Minimum displayed value for last buffer
			OpenViBE::float64 m_f64MinDisplayedValue;
			// Maximum displayed value for last buffer
			OpenViBE::float64 m_f64MaxDisplayedValue;

			//@}
		};
	}

}

#endif
