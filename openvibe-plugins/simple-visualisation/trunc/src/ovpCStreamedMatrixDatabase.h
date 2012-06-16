#ifndef __OpenViBEPlugins_SimpleVisualisation_CStreamedMatrixDatabase_H__
#define __OpenViBEPlugins_SimpleVisualisation_CStreamedMatrixDatabase_H__

#include "ovp_defines.h"
#include "ovpIStreamDatabase.h"

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
		* This class is used to store information about the incoming matrix stream. It can request a IStreamDisplayDrawable
		* object to redraw itself upon changes in its data.
		*/
		class CStreamedMatrixDatabase : public IStreamDatabase
		{
		public:
			/**
			 * \brief Constructor
			 * \param
			 */
			CStreamedMatrixDatabase(
				OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>& oPlugin);

			/**
			 * \brief Destructor
			 */
			virtual ~CStreamedMatrixDatabase();

			virtual OpenViBE::boolean initialize();

			virtual void setDrawable(
				IStreamDisplayDrawable* pDrawable);

			virtual void setRedrawOnNewData(
				OpenViBE::boolean bRedrawOnNewData);

			virtual OpenViBE::boolean isFirstBufferReceived();

			virtual OpenViBE::boolean setMaxBufferCount(
				OpenViBE::uint32 ui32MaxBufferCount);

			virtual OpenViBE::boolean setTimeScale(
				OpenViBE::float64 f64TimeScale);

			virtual OpenViBE::boolean decodeMemoryBuffer(
				const OpenViBE::IMemoryBuffer* pMemoryBuffer,
				OpenViBE::uint64 ui64StartTime,
				OpenViBE::uint64 ui64EndTime);

			virtual OpenViBE::uint32 getMaxBufferCount();

			virtual OpenViBE::uint32 getCurrentBufferCount();

			virtual const OpenViBE::float64* getBuffer(
				OpenViBE::uint32 ui32BufferIndex);

			virtual OpenViBE::uint64 getStartTime(
				OpenViBE::uint32 ui32BufferIndex);

			virtual OpenViBE::uint64 getEndTime(
				OpenViBE::uint32 ui32BufferIndex);

			virtual OpenViBE::uint32 getBufferElementCount();

			virtual OpenViBE::uint64 getBufferDuration();

			virtual OpenViBE::boolean isBufferTimeStepComputed();

			virtual OpenViBE::uint64 getBufferTimeStep();

			virtual OpenViBE::uint32 getSampleCountPerBuffer();

			virtual OpenViBE::uint32 getChannelCount();

			virtual OpenViBE::boolean getChannelLabel(
				const OpenViBE::uint32 ui32ChannelIndex,
				OpenViBE::CString& rElectrodeLabel);

			virtual OpenViBE::boolean getChannelMinMaxValues(
				OpenViBE::uint32 ui32Channel,
				OpenViBE::float64& f64Min,
				OpenViBE::float64& f64Max);

			virtual OpenViBE::boolean getGlobalMinMaxValues(
				OpenViBE::float64& f64Min,
				OpenViBE::float64& f64Max);

			virtual OpenViBE::boolean getLastBufferChannelMinMaxValues(
				OpenViBE::uint32 ui32Channel,
				OpenViBE::float64& f64Min,
				OpenViBE::float64& f64Max);

			virtual OpenViBE::boolean getLastBufferGlobalMinMaxValues(
				OpenViBE::float64& f64Min,
				OpenViBE::float64& f64Max);

		protected:
			OpenViBE::boolean onBufferCountChanged();

			virtual OpenViBE::boolean decodeHeader();

			virtual OpenViBE::boolean decodeBuffer(
				OpenViBE::uint64 ui64StartTime,
				OpenViBE::uint64 ui64EndTime);

		protected:
			// parent plugin
			OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>& m_oParentPlugin;
			//decoder algorithm
			OpenViBE::Kernel::IAlgorithmProxy* m_pDecoder;
			//drawable object to update (if needed)
			IStreamDisplayDrawable* m_pDrawable;
			//flag stating whether to redraw the IStreamDisplayDrawable upon new data reception if true (default)
			OpenViBE::boolean m_bRedrawOnNewData;
			//flag stating whether first samples buffer has been received
			OpenViBE::boolean m_bFirstBufferReceived;
			//flag stating whether buffer time step was computed
			OpenViBE::boolean m_bBufferTimeStepComputed;
			//time difference between start times of two consecutive buffers
			OpenViBE::uint64 m_ui64BufferTimeStep;
			//sampling frequency of incoming stream
			//OpenViBE::uint32 m_ui32SamplingFrequency;
			//maximum number of buffers stored in database
			OpenViBE::uint32 m_ui32MaxBufferCount;
			//flag stating whether time scale should be ignored (max buffer count externally set)
			OpenViBE::boolean m_bIgnoreTimeScale;
			//maximum duration of displayed buffers (in seconds)
			OpenViBE::float64 m_f64TimeScale;
			//double-linked list of start times of stored buffers
			std::deque<OpenViBE::uint64> m_oStartTime;
			//double-linked list of end times of stored buffers
			std::deque<OpenViBE::uint64> m_oEndTime;
			//streamed matrix header
			OpenViBE::CMatrix m_oStreamedMatrixHeader;
			//streamed matrix	history
			std::deque<OpenViBE::CMatrix*> m_oStreamedMatrices;
			//min/max values for each channel
			std::vector<std::deque<std::pair<OpenViBE::float64, OpenViBE::float64> > > m_oChannelMinMaxValues;
		};
	}
}

#endif //#ifndef __OpenViBEPlugins_SimpleVisualisation_CStreamedMatrixDatabase_H__
