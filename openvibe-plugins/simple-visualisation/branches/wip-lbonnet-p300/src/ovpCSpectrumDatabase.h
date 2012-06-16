#ifndef __OpenViBEPlugins_SimpleVisualisation_CSpectrumDatabase_H__
#define __OpenViBEPlugins_SimpleVisualisation_CSpectrumDatabase_H__

#include "ovp_defines.h"
#include "ovpCStreamedMatrixDatabase.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

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
		/**
		* This class is used to store information about the incoming spectrum stream. It can request a IStreamDisplayDrawable
		* object to redraw itself upon changes in its data.
		*/
		class CSpectrumDatabase : public CStreamedMatrixDatabase
		{
		public:
			CSpectrumDatabase(
				OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>& oPlugin);

			virtual ~CSpectrumDatabase();

			virtual OpenViBE::boolean initialize();

			/**
			 * \brief Set displayed frequency range
			 * \param f64MinimumDisplayedFrequency Minimum frequency to display
			 * \param f64MaximumDisplayedFrequency Maximum frequency to display
			 */
			//TODO (if min/max computation should be restricted to this range)
			/*
			void setDisplayedFrequencyRange(
				OpenViBE::float64 f64MinimumDisplayedFrequency,
				OpenViBE::float64 f64MaximumDisplayedFrequency);*/

			/** \name Frequency bands management */
			//@{

			/**
			 * \brief Get number of frequency bands
			 * \return Number of frequency bands
			 */
			OpenViBE::uint32 getFrequencyBandCount();

			/**
			 * \brief Get width of a frequency band (in Hz)
			 * \return Frequency band width
			 */
			OpenViBE::float64 getFrequencyBandWidth();

			/**
			 * \brief Get frequency band start frequency
			 * \param ui32FrequencyBandIndex Index of frequency band
			 * \return Frequency band start if it could be retrieved, 0 otherwise
			 */
			OpenViBE::float64 getFrequencyBandStart(
				OpenViBE::uint32 ui32FrequencyBandIndex);

			/**
			 * \brief Get frequency band stop frequency
			 * \param ui32FrequencyBandIndex Index of frequency band
			 * \return Frequency band stop if it could be retrieved, 0 otherwise
			 */
			OpenViBE::float64 getFrequencyBandStop(
				OpenViBE::uint32 ui32FrequencyBandIndex);

			//@}

		protected:
			virtual OpenViBE::boolean decodeHeader();

		private:
			//frequency band pairs (start and stop frequencies)
			std::vector< std::pair<OpenViBE::float64, OpenViBE::float64> > m_oFrequencyBands;
		};
	}
}

#endif
