#ifndef __OpenViBEPlugins_SimpleVisualisation_CBufferDatabase_H__
#define __OpenViBEPlugins_SimpleVisualisation_CBufferDatabase_H__

#include "ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/IReader.h>

#include <vector>
#include <deque>
#include <queue>
#include <string>
#include <cfloat>

#ifdef OVP_OS_Windows
#ifndef NDEBUG
		//#define ELAN_VALIDATION
		#define NB_ELAN_CHANNELS 143
#endif
#endif

#include <iostream>

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		class CSignalDisplayDrawable;

		/**
		* Abtract class of objects than can be updated by a CBufferDatabase
		*/
		class CSignalDisplayDrawable
		{
			public:

				virtual ~CSignalDisplayDrawable(){}
				virtual void init() = 0;
				virtual void redraw() = 0;
		};

		/**
		* This class is used to store information about the incoming signal stream. It can request a CSignalDisplayDrawable
		* object to redraw himself in case of some changes in its data.
		*/
		class CBufferDatabase
		{
		public:
			//! Number of channels
			OpenViBE::int64 m_i64NbElectrodes;

			//! Number of channels and number of samples per buffer
			OpenViBE::uint64 m_pDimmensionSizes[2];

			//! Channel labels, buffer labels
			std::vector<std::string> m_pDimmesionLabels[2];

			//! Flag set to true once first buffer is received
			OpenViBE::boolean m_bFirstBufferReceived;

			//! Sampling frequency of the incoming stream
			OpenViBE::uint32 m_ui32SamplingFrequency;

			//! double-linked list of pointers to the samples buffers of the current time window
			std::deque<OpenViBE::float64 *> m_oSampleBuffers;

			//! stimulations to display. pair values are <date, stimcode>
			std::deque<std::pair<OpenViBE::uint64, OpenViBE::uint64> > m_oStimulations;

			//electrode spherical coordinates (in degrees)
			//OpenViBE::CMatrix m_oElectrodesSphericalCoords;

			//flag set to true once channel lookup indices are determined
			OpenViBE::boolean m_bChannelLookupTableInitialized;

			//indices of electrodes in channel localisation database
			std::vector<OpenViBE::uint32> m_oChannelLookupIndices;

			//electrode labels (standardized)
			//std::vector<OpenViBE::CString> m_oElectrodesLabels;

			//! Number of buffer to display at the same time
			OpenViBE::uint64 m_ui64NumberOfBufferToDisplay;

			//! The global maximum value of the signal (up to now)
			OpenViBE::float64 m_f64MaximumValue;

			//! The global minimum value of the signal (up to now)
			OpenViBE::float64 m_f64MinimumValue;

			//! Double-linked list of the start times of the current buffers
			std::deque<OpenViBE::uint64> m_oStartTime;

			//! Double-linked list of the end times of the current buffers
			std::deque<OpenViBE::uint64> m_oEndTime;

			//! Duration to display in seconds
			OpenViBE::float64 m_f64TotalDuration;

			/*! Duration to display in openvibe time units.
			Computed once every time the user changes the total duration to display,
			when the maximum number of buffers to store are received.*/
			OpenViBE::uint64 m_ui64TotalDuration;

			/*! Duration of a single buffer.
			Computed once, but not constant when sampling frequency is not a multiple of buffer size!*/
			OpenViBE::uint64 m_ui64BufferDuration;

			/*! Time step separating the start times of m_ui64NumberOfBufferToDisplay+1 buffers.
			Recomputed once every time the user changes the total duration to display,
			but not constant when sampling frequency is not a multiple of buffer size!*/
			OpenViBE::uint64 m_ui64TotalStep;

			/*! Time step separating the start times of 2 consecutive buffers.
			Computed once, but not constant when sampling frequency is not a multiple of buffer size!*/
			OpenViBE::uint64 m_ui64BufferStep;

			//! Pointer to the drawable object to update (if needed)
			CSignalDisplayDrawable * m_pDrawable;

			std::vector<std::deque<std::pair<OpenViBE::float64, OpenViBE::float64> > > m_oLocalMinMaxValue;

			OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>& m_oParentPlugin;

			OpenViBE::boolean m_bError;

			//! Redraws the associated SignalDisplayDrawable upon new data reception if true (default)
			OpenViBE::boolean m_bRedrawOnNewData;

		protected:
			/* \name Channel localisation */
			//@{
			//channel localisation decoder
			OpenViBE::Kernel::IAlgorithmProxy* m_pChannelLocalisationStreamDecoder;
			//flag set to true once channel localisation buffer is received
			OpenViBE::boolean m_bChannelLocalisationHeaderReceived;
			//dynamic channel localisation flag (e.g. localisation is constantly updated with MEG)
			OpenViBE::boolean m_bDynamicChannelLocalisation;
			//channel labels database
			std::vector<OpenViBE::CString> m_oChannelLocalisationLabels;
			//flag stating whether streamed coordinates are cartesian (as opposed to spherical)
			OpenViBE::boolean m_bCartesianStreamedCoords;
			//! double-linked list of streamed channel coordinates (if cartesian, expressed in normalized space (X right Y front Z up))
			std::deque< std::pair<OpenViBE::CMatrix*, OpenViBE::boolean> > m_oChannelLocalisationStreamedCoords;
			//! double-linked list of channel coordinates (spherical if streamed coords aere cartesian and vice versa)
			//std::deque<  std::pair<OpenViBE::CMatrix*, OpenViBE::boolean> > m_oChannelLocalisationAlternateCoords;
			//pointer to double linked list of cartesian coordinates
			//std::deque< std::pair<OpenViBE::CMatrix*, OpenViBE::boolean> > * m_pChannelLocalisationCartesianCoords;
			//pointer to double linked list of spherical coordinates
			//std::deque< std::pair<OpenViBE::CMatrix*, OpenViBE::boolean> > * m_pChannelLocalisationSphericalCoords;
			//! double-linked list of start/end times of channel coordinates
			std::deque< std::pair<OpenViBE::uint64, OpenViBE::uint64> > m_oChannelLocalisationTimes;
			//@}

			//! Redraw mode (shift or scan)
			OpenViBE::CIdentifier m_oDisplayMode;

		public:
			CBufferDatabase(
				OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>& oPlugin);

			virtual ~CBufferDatabase();

			/**
			 * \brief Decode a channel localisation memory buffer
			 * \param pMemoryBuffer Memory buffer to decode
			 * \param ui64StartTime Start time of memory buffer
			 * \param ui64EndTime End time of memory buffer
			 * \return True if memory buffer could be properly decoded, false otherwise
			 */
			virtual OpenViBE::boolean decodeChannelLocalisationMemoryBuffer(
				const OpenViBE::IMemoryBuffer* pMemoryBuffer,
				OpenViBE::uint64 ui64StartTime,
				OpenViBE::uint64 ui64EndTime);

			/**
			 * \brief Callback called upon channel localisation buffer reception
			 * \param uint32 Index of newly received channel localisation buffer
			 * \return True if buffer data was correctly processed, false otherwise
			 */
			virtual OpenViBE::boolean onChannelLocalisationBufferReceived(
				OpenViBE::uint32 ui32ChannelLocalisationBufferIndex);

			/**
			 * \brief Sets the drawable object to update.
			 * \param pDrawable drawable object to update.
			 */
			virtual void setDrawable(
				CSignalDisplayDrawable * pDrawable);

			/**
			 * \brief Get error status
			 * \return Error status. If true, an error occured.
			 */
			virtual OpenViBE::boolean getErrorStatus();

			/**
			 * \brief Determines whether first buffer has been received yet
			 * \return True if first buffer has been received already, false otherwise
			 */
			virtual OpenViBE::boolean isFirstBufferReceived();

			/**
			 * \brief Determines whether first channel localisation buffer has been processed yet
			 * When this condition is true, channel coordinates may be retrieved using the
			 * corresponding methods in this class.
			 * \return True if first chanloc buffer was processed
			 */
			virtual OpenViBE::boolean isFirstChannelLocalisationBufferProcessed();

			/**
			 * Compute the number of buffers needed to display the signal for a certain time period.
			 * \param f64NumberOfMsToDisplay the time window's width in seconds.
			 */
			virtual OpenViBE::boolean adjustNumberOfDisplayedBuffers(OpenViBE::float64 f64NumberOfSecondsToDisplay);

			/**
			 * \brief Get time interval covered by data held in this object
			 * \return Time interval in ms
			 */
			virtual OpenViBE::float64 getDisplayedTimeIntervalWidth() const;

			/**
			 * \brief Determine whether time passed in parameter lies in displayed data interval
			 * \param ui64Time Time to test
			 * \return True if time lies in displayed time interval, false otherwise
			 */
			virtual OpenViBE::boolean isTimeInDisplayedInterval(const OpenViBE::uint64& ui64Time) const;

			/**
			 * \brief Get index of sample buffer which starts at a given time
			 * \param ui64Time[in] Start time of buffer
			 * \param rIndex[out] Buffer index
			 * \return True if buffer index could be determined, false otherwise
			 */
			virtual OpenViBE::boolean getIndexOfBufferStartingAtTime(const OpenViBE::uint64& ui64Time, OpenViBE::uint32& rIndex) const;

			//! Returns the min/max values currently displayed for the given channel
			virtual void getDisplayedChannelLocalMinMaxValue(OpenViBE::uint32 ui32Channel, OpenViBE::float64& f64Min, OpenViBE::float64& f64Max);
			//! Returns the min/max values currently displayed (all channels taken into account)
			virtual void getDisplayedGlobalMinMaxValue(OpenViBE::float64& f64Min, OpenViBE::float64& f64Max);

			//! Returns the min/max values of the last buffer arrived for the given channel
			virtual void getLastBufferChannelLocalMinMaxValue(OpenViBE::uint32 ui32Channel, OpenViBE::float64& f64Min, OpenViBE::float64& f64Max)
			{
				f64Min = m_oLocalMinMaxValue[ui32Channel].back().first;
				f64Max = m_oLocalMinMaxValue[ui32Channel].back().second;
			}

			//! Returns the min/max values of the last buffer arrived  (all channels taken into account)
			virtual void getLastBufferMinMaxValue(OpenViBE::float64& f64Min, OpenViBE::float64& f64Max)
			{
				f64Min = +DBL_MAX;
				f64Max = -DBL_MAX;

				for(OpenViBE::uint32 c=0 ; c<m_oLocalMinMaxValue.size() ; c++)
				{
					f64Min = (m_oLocalMinMaxValue[c].back().first < f64Min) ? m_oLocalMinMaxValue[c].back().first : f64Min;
					f64Max = (m_oLocalMinMaxValue[c].back().second > f64Max) ? m_oLocalMinMaxValue[c].back().second : f64Max;
				}
			}

			/**
			 * \brief Get number of eletrodes in database
			 * \return Number of electrodes
			 */
			virtual OpenViBE::uint64 getElectrodeCount();

			/**
			 * \brief Get electrode normalized position
			 * \remarks Position expressed in normalized cartesian frame where X is right, Y front, Z up
			 * \param[in] ui32ElectrodeIndex Index of electrode in database whose position is to be retrieved
			 * \param[out] pElectrodePosition Pointer to an array of 3 floats where to store coordinates
			 * \return True if electrode position could be retrieved
			 */
			virtual OpenViBE::boolean getElectrodePosition(
				const OpenViBE::uint32 ui32ElectrodeIndex,
				OpenViBE::float64* pElectrodePosition);

			/**
			 * \brief Get electrode normalized position
			 * \remarks Position expressed in normalized cartesian frame where X is right, Y front, Z up
			 * \param[in] rElectrodeLabel Label of electrode whose position is to be retrieved
			 * \param[out] pElectrodePosition Pointer to an array of 3 floats where to store coordinates
			 * \return True if electrode position could be retrieved
			 */
			virtual OpenViBE::boolean getElectrodePosition(
				const OpenViBE::CString& rElectrodeLabel,
				OpenViBE::float64* pElectrodePosition);

			/**
			 * \brief Get electrode label
			 * \param[in] ui32ElectrodeIndex Index of electrode in database whose label is to be retrieved
			 * \param[out] rElectrodeLabel Electrode label
			 * \return True if electrode label could be retrieved
			 */
			virtual OpenViBE::boolean getElectrodeLabel(
				const OpenViBE::uint32 ui32ElectrodeIndex,
				OpenViBE::CString& rElectrodeLabel);

			/**
			 * \brief Get number of channels
			 * \return Number of channels
			 */
			virtual OpenViBE::uint64 getChannelCount() const;

			/**
			 * \brief Get channel normalized position
			 * \remarks Position expressed in normalized cartesian frame where X is right, Y front, Z up
			 * \param[in] ui32ChannelIndex Index of channel whose position is to be retrieved
			 * \param[out] rChannelPosition Reference on a float64 pointer
			 * \return True if channel position could be retrieved (rChannelPosition then points to an array of 3 floats)
			 */
			virtual OpenViBE::boolean getChannelPosition(
				const OpenViBE::uint32 ui32ChannelIndex,
				OpenViBE::float64*& rChannelPosition);

			/**
			 * \brief Get channel spherical coordinates in degrees
			 * \param[in] ui32ChannelIndex Index of channel whose coordinates are to be retrieved
			 * \param[out] rTheta Reference on a float to be set with theta angle
			 * \param[out] rPhi Reference on a float to be set with phi angle
			 * \return True if channel coordinates could be retrieved
			 */
			virtual OpenViBE::boolean getChannelSphericalCoordinates(
				const OpenViBE::uint32 ui32ChannelIndex,
				OpenViBE::float64& rTheta,
				OpenViBE::float64& rPhi);

			/**
			 * \brief Get channel label
			 * \param[in] ui32ChannelIndex Index of channel whose label is to be retrieved
			 * \param[out] rChannelLabel Channel label
			 * \return True if channel label could be retrieved
			 */
			virtual OpenViBE::boolean getChannelLabel(
				const OpenViBE::uint32 ui32ChannelIndex,
				OpenViBE::CString& rChannelLabel);

			virtual void setMatrixDimmensionCount(
				const OpenViBE::uint32 ui32DimmensionCount);
			virtual void setMatrixDimmensionSize(
				const OpenViBE::uint32 ui32DimmensionIndex,
				const OpenViBE::uint32 ui32DimmensionSize);
			virtual void setMatrixDimmensionLabel(
				const OpenViBE::uint32 ui32DimmensionIndex,
				const OpenViBE::uint32 ui32DimmensionEntryIndex,
				const char* sDimmensionLabel);
			virtual void setMatrixBuffer(
				const OpenViBE::float64* pBuffer,
				OpenViBE::uint64 ui64StartTime,
				OpenViBE::uint64 ui64EndTime);

			virtual void setStimulationCount(
				const OpenViBE::uint32 ui32StimulationCount);
			virtual void setStimulation(
				const OpenViBE::uint32 ui32StimulationIndex,
				const OpenViBE::uint64 ui64StimulationIdentifier,
				const OpenViBE::uint64 ui64StimulationDate);

			/**
			 * \brief Set display mode
			 * \remarks Used by signal display and time ruler to determine how they should be updated
			 * \param oDisplayMode New display mode
			 */
			virtual void setDisplayMode(
				OpenViBE::CIdentifier oDisplayMode);

			/**
			 * \brief Get current display mode
			 * \return Current display mode
			 */
			virtual OpenViBE::CIdentifier getDisplayMode();

			/**
			 * \brief Set flag stating whether to redraw associated SignalDisplayDrawable objet when new data is available
			 * \param bSet Value to set flag with
			 */
			virtual void setRedrawOnNewData(
				OpenViBE::boolean bSet);

		protected:
			/**
			 * \brief Initialize table storing indices of electrodes in channel localisation database
			 * \return True if table could be initialized
			 */
			virtual OpenViBE::boolean fillChannelLookupTable();

			/**
			 * \brief Convert a cartesian coordinates triplet to spherical coordinates
			 * \param[in] pCartesianCoords Pointer to cartesian coordinates triplet
			 * \param[out] rTheta Equivalent theta angle
			 * \param[out] rPhi Equivalent phi angle
			 * \return True if coordinates were successfully converted
			 */
			OpenViBE::boolean convertCartesianToSpherical(
				const OpenViBE::float64* pCartesianCoords,
				OpenViBE::float64& rTheta,
				OpenViBE::float64& rPhi);
		};
	}

}

#endif
