#include "ovpCBufferDatabase.h"

#include <system/Memory.h>

#include <algorithm>

#include <cmath>
#include <cstring>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

using namespace OpenViBEToolkit;

using namespace std;

CBufferDatabase::CBufferDatabase(OpenViBEToolkit::TBoxAlgorithm<Plugins::IBoxAlgorithm>& oPlugin)
	:m_i64NbElectrodes(0)
	,m_bFirstBufferReceived(false)
	,m_ui32SamplingFrequency(0)
	,m_bChannelLookupTableInitialized(false)
	,m_ui64NumberOfBufferToDisplay(2)
	,m_f64MaximumValue(-DBL_MAX)
	,m_f64MinimumValue(+DBL_MAX)
	,m_f64TotalDuration(0)
	,m_ui64TotalDuration(0)
	,m_ui64BufferDuration(0)
	,m_ui64TotalStep(0)
	,m_ui64BufferStep(0)
	,m_pDrawable(NULL)
	,m_oParentPlugin(oPlugin)
	,m_bError(false)
	,m_bRedrawOnNewData(true)
	,m_pChannelLocalisationStreamDecoder(NULL)
	,m_bChannelLocalisationHeaderReceived(false)
	,m_bDynamicChannelLocalisation(false)
	,m_bCartesianStreamedCoords(false)
	,m_oDisplayMode(OVP_TypeId_SignalDisplayMode_Scan)
{
	m_pChannelLocalisationStreamDecoder = &m_oParentPlugin.getAlgorithmManager().getAlgorithm(
		m_oParentPlugin.getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_ChannelLocalisationStreamDecoder));

	m_pChannelLocalisationStreamDecoder->initialize();

	m_pDimmensionSizes[0] = m_pDimmensionSizes[1] = 0;
}

CBufferDatabase::~CBufferDatabase()
{
	m_pChannelLocalisationStreamDecoder->uninitialize();
	m_oParentPlugin.getAlgorithmManager().releaseAlgorithm(*m_pChannelLocalisationStreamDecoder);

	//delete all the remaining buffers
	while(m_oSampleBuffers.size() > 0)
	{
		delete[] m_oSampleBuffers.front();
		m_oSampleBuffers.pop_front();
	}

	//delete channel localisation matrices
	while(m_oChannelLocalisationStreamedCoords.size() > 0)
	{
		delete m_oChannelLocalisationStreamedCoords.front().first;
		m_oChannelLocalisationStreamedCoords.pop_front();
	}

	/*while(m_oChannelLocalisationAlternateCoords.size() > 0)
	{
		delete[] m_oChannelLocalisationAlternateCoords.front().first;
		m_oChannelLocalisationAlternateCoords.pop_front();
	}*/
}

boolean CBufferDatabase::decodeChannelLocalisationMemoryBuffer(const IMemoryBuffer* pMemoryBuffer, uint64 ui64StartTime, uint64 ui64EndTime)
{
	//feed memory buffer to decoder
	m_pChannelLocalisationStreamDecoder->getInputParameter(
		OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_InputParameterId_MemoryBufferToDecode)->setReferenceTarget(&pMemoryBuffer);

	//process buffer
	m_pChannelLocalisationStreamDecoder->process();

	//copy header if needed
	if(m_pChannelLocalisationStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_OutputTriggerId_ReceivedHeader) == true)
	{
		//retrieve matrix header
		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > l_oMatrix;
		l_oMatrix.initialize(m_pChannelLocalisationStreamDecoder->getOutputParameter(OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_OutputParameterId_Matrix));

		//copy channel labels
		m_oChannelLocalisationLabels.resize(l_oMatrix->getDimensionSize(0));
		for(vector<CString>::size_type i=0; i<m_oChannelLocalisationLabels.size(); i++)
		{
			m_oChannelLocalisationLabels[i] = l_oMatrix->getDimensionLabel(0, i);
		}

		//retrieve dynamic flag
		OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > l_bDynamic;
		l_bDynamic.initialize(m_pChannelLocalisationStreamDecoder->getOutputParameter(OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_OutputParameterId_Dynamic));
		m_bDynamicChannelLocalisation = l_bDynamic;

		if(l_oMatrix->getDimensionSize(1) == 3)
		{
			m_bCartesianStreamedCoords = true;
			/*m_pChannelLocalisationCartesianCoords = &m_oChannelLocalisationStreamedCoords;
			m_pChannelLocalisationSphericalCoords = &m_oChannelLocalisationAlternateCoords;*/
		}
		else if(l_oMatrix->getDimensionSize(1) == 2)
		{
			m_bCartesianStreamedCoords = false;
			/*m_pChannelLocalisationCartesianCoords = &m_oChannelLocalisationAlternateCoords;
			m_pChannelLocalisationSphericalCoords = &m_oChannelLocalisationStreamedCoords;*/
		}
		else
		{
			m_oParentPlugin.getLogManager() << LogLevel_Error
					<< "Wrong size found for dimension 1 of Channel localisation header! Can't process header!\n";
			return false;
		}

		//header information received
		m_bChannelLocalisationHeaderReceived = true;
	}

	//has a chanloc buffer been received?
	if(m_pChannelLocalisationStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_OutputTriggerId_ReceivedBuffer) == true)
	{
		//number of buffers required to cover displayed time range
		uint64 l_ui64MaxBufferCount = 1;

		//resize channel localisation queue if necessary
		if(m_bDynamicChannelLocalisation == true)
		{
			uint64 l_ui64BufferDuration = ui64EndTime - ui64StartTime;
			if(l_ui64BufferDuration != 0)
			{
				l_ui64MaxBufferCount = static_cast<uint64>(ceil(m_f64TotalDuration / l_ui64BufferDuration));
				if(l_ui64MaxBufferCount == 0)
				{
					l_ui64MaxBufferCount = 1;
				}
			}

			//if new number of buffers decreased, resize list and destroy useless buffers
			while(m_oChannelLocalisationStreamedCoords.size() > l_ui64MaxBufferCount)
			{
				delete[] m_oChannelLocalisationStreamedCoords.front().first;
				m_oChannelLocalisationStreamedCoords.pop_front();
				// delete[] m_oChannelLocalisationAlternateCoords.front().first;
				// m_oChannelLocalisationAlternateCoords.pop_front();
				m_oChannelLocalisationTimes.pop_front();
			}
		}

		//retrieve coordinates matrix
		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > l_oMatrix;
		l_oMatrix.initialize(m_pChannelLocalisationStreamDecoder->getOutputParameter(OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_OutputParameterId_Matrix));

		//get pointer to destination matrix
		CMatrix* l_pChannelLocalisation = NULL;
		//CMatrix* l_pAlternateChannelLocalisation = NULL;
		if(m_oChannelLocalisationStreamedCoords.size() < l_ui64MaxBufferCount)
		{
			//create a new matrix and resize it
			l_pChannelLocalisation = new CMatrix();
			Tools::Matrix::copyDescription(*l_pChannelLocalisation, *l_oMatrix);
			// l_pAlternateChannelLocalisation = new CMatrix();
			// TODO : resize it appropriately depending on whether it is spherical or cartesian
		}
		else //m_oChannelLocalisationStreamedCoords.size() == l_ui64MaxBufferCount
		{
			l_pChannelLocalisation = m_oChannelLocalisationStreamedCoords.front().first;
			m_oChannelLocalisationStreamedCoords.pop_front();
			// l_pAlternateChannelLocalisation = m_oChannelLocalisationAlternateCoords.front().first;
			// m_oChannelLocalisationAlternateCoords.pop_front();
			m_oChannelLocalisationTimes.pop_front();
		}

		if(l_pChannelLocalisation)
		{
			//copy coordinates and times
			Tools::Matrix::copyContent(*l_pChannelLocalisation, *l_oMatrix);
			m_oChannelLocalisationStreamedCoords.push_back(std::pair<CMatrix*, boolean>(l_pChannelLocalisation, true));
			//m_oChannelLocalisationAlternateCoords.push_back(std::pair<CMatrix*, boolean>(l_pAlternateChannelLocalisation, true));
			m_oChannelLocalisationTimes.push_back(std::pair<uint64, uint64>(ui64StartTime, ui64EndTime));
		}
	}

	return true;
}

void CBufferDatabase::setDrawable(CSignalDisplayDrawable* pDrawable)
{
	m_pDrawable=pDrawable;
}

boolean CBufferDatabase::getErrorStatus()
{
	return m_bError;
}

boolean CBufferDatabase::onChannelLocalisationBufferReceived(uint32 ui32ChannelLocalisationBufferIndex)
{
	m_oChannelLocalisationStreamedCoords[ui32ChannelLocalisationBufferIndex].second = false;

	return true;
}

boolean CBufferDatabase::isFirstBufferReceived()
{
	return m_bFirstBufferReceived;
}

boolean CBufferDatabase::isFirstChannelLocalisationBufferProcessed()
{
	//at least one chanloc buffer must have been received and processed
	return (m_oChannelLocalisationStreamedCoords.size() > 0) && (m_oChannelLocalisationStreamedCoords[0].second == false);
}

boolean CBufferDatabase::adjustNumberOfDisplayedBuffers(float64 f64NumberOfSecondsToDisplay)
{
	boolean l_bNumberOfBufferToDisplayChanged = false;

	if(f64NumberOfSecondsToDisplay>0)
	{
		m_f64TotalDuration = f64NumberOfSecondsToDisplay;
		m_ui64TotalDuration = 0;
		m_ui64TotalStep = 0;
	}

	//return if buffer length is not known yet
	if(m_pDimmensionSizes[1] == 0)
	{
		return false;
	}

	uint64 l_ui64NewNumberOfBufferToDisplay =  static_cast<uint64>(ceil( (m_f64TotalDuration*m_ui32SamplingFrequency) / m_pDimmensionSizes[1]));

	//displays at least one buffer
	l_ui64NewNumberOfBufferToDisplay = (l_ui64NewNumberOfBufferToDisplay == 0) ? 1 : l_ui64NewNumberOfBufferToDisplay;
	if(l_ui64NewNumberOfBufferToDisplay != m_ui64NumberOfBufferToDisplay || f64NumberOfSecondsToDisplay<=0)
	{
		m_ui64NumberOfBufferToDisplay = l_ui64NewNumberOfBufferToDisplay;
		l_bNumberOfBufferToDisplayChanged = true;

		//if new number of buffers decreased, resize lists and destroy useless buffers
		while(m_ui64NumberOfBufferToDisplay < m_oSampleBuffers.size())
		{
			delete[] m_oSampleBuffers.front();
			m_oSampleBuffers.pop_front();

			m_oStartTime.pop_front();

			m_oEndTime.pop_front();

			//suppress the corresponding minmax values
			for(uint32 c=0 ; c<m_pDimmensionSizes[0] ; c++)
			{
				m_oLocalMinMaxValue[c].pop_front();
			}
		}
	}

	return l_bNumberOfBufferToDisplayChanged;
}

uint64 CBufferDatabase::getChannelCount() const
{
	return m_pDimmensionSizes[0];
}

float64 CBufferDatabase::getDisplayedTimeIntervalWidth() const
{
	return (m_ui64NumberOfBufferToDisplay * ((m_pDimmensionSizes[1]*1000.0) / m_ui32SamplingFrequency));
}

void CBufferDatabase::setMatrixDimmensionCount(const uint32 ui32DimmensionCount)
{
	if(ui32DimmensionCount != 2)
	{
		m_oParentPlugin.getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Error dimmension count isn't 2!\n";
	}
}

void CBufferDatabase::setMatrixDimmensionSize(const uint32 ui32DimmensionIndex, const uint32 ui32DimmensionSize)
{
	m_pDimmensionSizes[ui32DimmensionIndex] = ui32DimmensionSize;
	m_pDimmesionLabels[ui32DimmensionIndex].resize(ui32DimmensionSize);

	if(ui32DimmensionIndex == 0)
	{
		m_i64NbElectrodes = m_pDimmensionSizes[ui32DimmensionIndex];

		//resize min/max values vector
		m_oLocalMinMaxValue.resize((size_t)m_i64NbElectrodes);
	}
}

void CBufferDatabase::setMatrixDimmensionLabel(const uint32 ui32DimmensionIndex, const uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel)
{
	m_pDimmesionLabels[ui32DimmensionIndex][ui32DimmensionEntryIndex] = sDimmensionLabel;
}

void CBufferDatabase::setMatrixBuffer(const float64* pBuffer, uint64 ui64StartTime, uint64 ui64EndTime)
{
	//if an error has occured, do nothing
	if(m_bError)
	{
		return;
	}

	//if this the first buffer, perform some precomputations
	if(m_bFirstBufferReceived == false)
	{
		m_ui64BufferDuration = ui64EndTime - ui64StartTime;

		//test if it is equal to zero : Error
		if(m_ui64BufferDuration == 0)
		{
			m_oParentPlugin.getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Error : buffer start time and end time are equal : " << ui64StartTime << "\n";

			m_bError = true;
			return;
		}

		//computes the sampling frequency
		m_ui32SamplingFrequency = (uint32) ( ((uint64)1<<32) * m_pDimmensionSizes[1] / (m_ui64BufferDuration) );

		//computes the number of buffer necessary to display the interval
		adjustNumberOfDisplayedBuffers(-1);

		m_pDrawable->init();

		m_bFirstBufferReceived = true;
	}

	if(m_bChannelLookupTableInitialized == false)
	{
		fillChannelLookupTable();
	}
	else
	{
		//look for chanloc buffers recently received
		for(uint32 i=0; i < m_oChannelLocalisationStreamedCoords.size(); i++)
		{
			//if a new set of coordinates was received
			if(m_oChannelLocalisationStreamedCoords[i].second == true)
			{
				onChannelLocalisationBufferReceived(i);
			}
		}
	}

#ifdef ELAN_VALIDATION
	return;
#endif

	float64* l_pBufferToWrite = NULL;
	uint64 l_ui64NumberOfSamplesPerBuffer = m_pDimmensionSizes[0] * m_pDimmensionSizes[1];

	//if old buffers need to be removed
	if(m_oSampleBuffers.size() == m_ui64NumberOfBufferToDisplay)
	{
		if(m_ui64TotalDuration == 0)
		{
			m_ui64TotalDuration = (m_oStartTime.back()-m_oStartTime.front()) + (m_oEndTime.back() - m_oStartTime.back());
		}
		if(m_ui64BufferStep == 0)
		{
			if(m_oStartTime.size()<=1)
			{
				m_ui64BufferStep = m_ui64TotalDuration;
			}
			else
			{
				m_ui64BufferStep = m_oStartTime[1] - m_oStartTime[0];
			}
		}
		if(m_ui64TotalStep == 0)
		{
			m_ui64TotalStep = (m_oStartTime.back()-m_oStartTime.front()) + m_ui64BufferStep;
		}

		//save first buffer pointer
		l_pBufferToWrite = m_oSampleBuffers.front();

		//pop first element from queues
		m_oSampleBuffers.pop_front();
		m_oStartTime.pop_front();
		m_oEndTime.pop_front();
		for(uint32 c=0 ; c<m_pDimmensionSizes[0] ; c++)
		{
			m_oLocalMinMaxValue[c].pop_front();
		}
	}

	//do we need to allocate a new buffer?
	if(l_pBufferToWrite == NULL)
	{
		l_pBufferToWrite = new float64[(size_t)l_ui64NumberOfSamplesPerBuffer];
	}

	//copy new buffer into internal buffer
	System::Memory::copy(l_pBufferToWrite, pBuffer,	l_ui64NumberOfSamplesPerBuffer*sizeof(float64));

	//push new buffer and its timestamps
	m_oSampleBuffers.push_back(l_pBufferToWrite);
	m_oStartTime.push_back(ui64StartTime);
	m_oEndTime.push_back(ui64EndTime);

	//compute and push min and max values of new buffer
	uint64 l_ui64CurrentSample = 0;
	//for each channel
	for(uint32 c=0 ; c<m_pDimmensionSizes[0] ; c++)
	{
		float64 l_f64LocalMin = DBL_MAX;
		float64 l_f64LocalMax = -DBL_MAX;

		//for each sample
		for(uint64 i=0 ; i<m_pDimmensionSizes[1] ; i++, l_ui64CurrentSample++)
		{
			//get channel local min/max
			if(pBuffer[l_ui64CurrentSample] < l_f64LocalMin)
			{
				l_f64LocalMin = pBuffer[l_ui64CurrentSample];
			}
			if(pBuffer[l_ui64CurrentSample] > l_f64LocalMax)
			{
				l_f64LocalMax = pBuffer[l_ui64CurrentSample];
			}
		}

		//adds the minmax pair to the corresponding channel's list
		m_oLocalMinMaxValue[c].push_back(pair<float64, float64>(l_f64LocalMin, l_f64LocalMax));

		if(l_f64LocalMax > m_f64MaximumValue)
		{
			m_f64MaximumValue = l_f64LocalMax;
		}
		if(l_f64LocalMin < m_f64MinimumValue)
		{
			m_f64MinimumValue = l_f64LocalMin;
		}
	}

	//tells the drawable to redraw himself since the signal information has been updated
	if(m_bRedrawOnNewData)
	{
		m_pDrawable->redraw();
	}
}

void CBufferDatabase::getDisplayedChannelLocalMinMaxValue(uint32 ui32Channel, float64& f64Min, float64& f64Max)
{
	f64Min = +DBL_MAX;
	f64Max = -DBL_MAX;

	for(uint64 i=0 ; i<m_oLocalMinMaxValue[(size_t)ui32Channel].size() ; i++)
	{
		if(f64Min > m_oLocalMinMaxValue[(size_t)ui32Channel][(size_t)i].first)
		{
			f64Min = m_oLocalMinMaxValue[(size_t)ui32Channel][(size_t)i].first;
		}
		if(f64Max < m_oLocalMinMaxValue[(size_t)ui32Channel][(size_t)i].second)
		{
			f64Max = m_oLocalMinMaxValue[(size_t)ui32Channel][(size_t)i].second;
		}
	}
}

boolean CBufferDatabase::isTimeInDisplayedInterval(const uint64& ui64Time) const
{
	if(m_oStartTime.size() == 0)
	{
		return false;
	}

	return ui64Time >= m_oStartTime.front() && ui64Time <= m_oEndTime.back();
}

boolean CBufferDatabase::getIndexOfBufferStartingAtTime(const OpenViBE::uint64& ui64Time, uint32& rIndex) const
{
	rIndex = 0;

	if(m_oSampleBuffers.size() == 0 || ui64Time < m_oStartTime.front() || ui64Time > m_oStartTime.back())
	{
		return false;
	}

	for(uint32 i=0; i<m_oStartTime.size(); i++)
	{
		if(m_oStartTime[i] == ui64Time)
		{
			rIndex = i;
			return true;
		}
	}

	return false;
}

void CBufferDatabase::getDisplayedGlobalMinMaxValue(float64& f64Min, float64& f64Max)
{
	f64Min = +DBL_MAX;
	f64Max = -DBL_MAX;

	for(uint32 c=0 ; c<m_oLocalMinMaxValue.size() ; c++)
	{
		for(uint64 i=0 ; i<m_oLocalMinMaxValue[(size_t)c].size() ; i++)
		{
			if(f64Min > m_oLocalMinMaxValue[(size_t)c][(size_t)i].first)
			{
				f64Min = m_oLocalMinMaxValue[(size_t)c][(size_t)i].first;
			}
			if(f64Max < m_oLocalMinMaxValue[(size_t)c][(size_t)i].second)
			{
				f64Max = m_oLocalMinMaxValue[(size_t)c][(size_t)i].second;
			}
		}
	}
}

uint64 CBufferDatabase::getElectrodeCount()
{
	return m_oChannelLocalisationLabels.size();
}

boolean CBufferDatabase::getElectrodePosition(const uint32 ui32ElectrodeIndex, float64* pElectrodePosition)
{
	//TODO : add time parameter and look for coordinates closest to that time!
	if(ui32ElectrodeIndex < m_oChannelLocalisationLabels.size())
	{
		//if(m_bCartesianStreamedCoords == true)
		//{
		*pElectrodePosition = *(m_oChannelLocalisationStreamedCoords[0].first->getBuffer() + 3*ui32ElectrodeIndex);
		*(pElectrodePosition+1) = *(m_oChannelLocalisationStreamedCoords[0].first->getBuffer() + 3*ui32ElectrodeIndex+1);
		*(pElectrodePosition+2) = *(m_oChannelLocalisationStreamedCoords[0].first->getBuffer() + 3*ui32ElectrodeIndex+2);
		//}
		return true;
	}
	return false;
}

boolean CBufferDatabase::getElectrodePosition(const CString& rElectrodeLabel, float64* pElectrodePosition)
{
	//TODO : add time parameter and look for coordinates closest to that time!
	for(unsigned int i=0; i<m_oChannelLocalisationLabels.size(); i++)
	{
		if(strcmp(rElectrodeLabel.toASCIIString(), m_oChannelLocalisationLabels[i].toASCIIString()) == 0)
		{
			//if(m_bCartesianStreamedCoords == true)
			//{
			*pElectrodePosition = *(m_oChannelLocalisationStreamedCoords[0].first->getBuffer() + 3*i);
			*(pElectrodePosition+1) = *(m_oChannelLocalisationStreamedCoords[0].first->getBuffer() + 3*i+1);
			*(pElectrodePosition+2) = *(m_oChannelLocalisationStreamedCoords[0].first->getBuffer() + 3*i+2);
			//}
			return true;
		}
	}

	return false;
}

boolean CBufferDatabase::getElectrodeLabel(const uint32 ui32ElectrodeIndex, CString& rElectrodeLabel)
{
	if(ui32ElectrodeIndex >= m_oChannelLocalisationLabels.size())
	{
		return false;
	}
	rElectrodeLabel = m_oChannelLocalisationLabels[ui32ElectrodeIndex].toASCIIString();
	return true;
}

boolean CBufferDatabase::getChannelPosition(const uint32 ui32ChannelIndex, float64*& rChannelPosition)
{
	//TODO : add time parameter and look for coordinates closest to that time!
	if(ui32ChannelIndex >= 0 && ui32ChannelIndex < m_oChannelLookupIndices.size())
	{
		if(m_bCartesianStreamedCoords == true)
		{
			rChannelPosition = m_oChannelLocalisationStreamedCoords[0].first->getBuffer() + 3*m_oChannelLookupIndices[ui32ChannelIndex];
		}/*
		else
		{
			//TODO
		}*/
		return true;
	}

	return false;
}

boolean CBufferDatabase::getChannelSphericalCoordinates(const uint32 ui32ChannelIndex, float64& rTheta, float64& rPhi)
{
	//TODO : add time parameter and look for coordinates closest to that time!
	if(ui32ChannelIndex >= 0 && ui32ChannelIndex < m_oChannelLookupIndices.size())
	{
		if(m_bCartesianStreamedCoords == true)
		{
			//get cartesian coords
			float64* l_pCoords = m_oChannelLocalisationStreamedCoords[0].first->getBuffer() + 3*m_oChannelLookupIndices[ui32ChannelIndex];

			//convert to spherical coords
			return convertCartesianToSpherical(l_pCoords, rTheta, rPhi);
		}
		else //streamed coordinates are spherical already
		{
			//TODO
			return false;
		}
	}
	else
	{
		return false;
	}
}

boolean CBufferDatabase::getChannelLabel(const uint32 ui32ChannelIndex, CString& rChannelLabel)
{
	if(ui32ChannelIndex >= 0 && ui32ChannelIndex < m_oChannelLookupIndices.size())
	{
		rChannelLabel = m_oChannelLocalisationLabels[m_oChannelLookupIndices[ui32ChannelIndex]];
		return true;
	}
	else
	{
		rChannelLabel = "";
		return false;
	}
}

void CBufferDatabase::setStimulationCount(const uint32 ui32StimulationCount)
{
}

void CBufferDatabase::setStimulation(const uint32 ui32StimulationIndex, const uint64 ui64StimulationIdentifier, const uint64 ui64StimulationDate)
{
	// m_oParentPlugin.getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Trace << "Received new stimulation id:" << ui64StimulationIdentifier << " date:" << ui64StimulationDate << "\n";

	m_oStimulations.push_back(std::pair<uint64, uint64>(ui64StimulationDate, ui64StimulationIdentifier));

	if(m_oStartTime.size()!=0)
	{
		std::deque<std::pair<uint64, uint64> >::iterator i;
		while(m_oStimulations.begin() != m_oStimulations.end() && m_oStimulations.begin()->first < m_oStartTime.front())
		{
			m_oStimulations.pop_front();
		}
	}
}

void CBufferDatabase::setDisplayMode(CIdentifier oDisplayMode)
{
	m_oDisplayMode = oDisplayMode;
}

CIdentifier CBufferDatabase::getDisplayMode()
{
	return m_oDisplayMode;
}

void CBufferDatabase::setRedrawOnNewData(boolean bSet)
{
	m_bRedrawOnNewData = bSet;
}

boolean CBufferDatabase::fillChannelLookupTable()
{
	if(m_bFirstBufferReceived == false || m_bChannelLocalisationHeaderReceived == false)
	{
		return false;
	}

#ifdef ELAN_VALIDATION
		setMatrixDimmensionSize(0, NB_ELAN_CHANNELS);
		for(uint32 i=0; i<m_pDimmensionSizes[0]; i++)
		{
			m_pDimmesionLabels[0][i] = m_oChannelLocalisationLabels[i].toASCIIString();	//use elan database
		}
#endif

	boolean res = true;

	//resize lookup array and initialize lookup indices to 0
	m_oChannelLookupIndices.resize((uint32)(m_i64NbElectrodes), 0);

	//for all channels
	for(uint32 i=0; i<m_pDimmensionSizes[0]; i++)
	{
		//trim leading spaces
		uint32 firstNonWhitespaceChar = 0;
		for(; firstNonWhitespaceChar < m_pDimmesionLabels[0][i].size(); firstNonWhitespaceChar++)
		{
			if(!isspace(m_pDimmesionLabels[0][i][firstNonWhitespaceChar]))
			{
				break;
			}
		}

		//trim trailing spaces
		uint32 lastNonWhitespaceChar = 0;
		if(m_pDimmesionLabels[0][i].size() > 0)
		{
			for(lastNonWhitespaceChar = m_pDimmesionLabels[0][i].size()-1; lastNonWhitespaceChar >= 0; lastNonWhitespaceChar--)
			{
				if(!isspace(m_pDimmesionLabels[0][i][lastNonWhitespaceChar]))
				{
					break;
				}
			}
		}

		//look for label in channel localisation labels database
		boolean l_bLabelRecognized = false;

		if(firstNonWhitespaceChar < lastNonWhitespaceChar)
		{
			std::string l_oChannelLabel(m_pDimmesionLabels[0][i].substr(firstNonWhitespaceChar, lastNonWhitespaceChar-firstNonWhitespaceChar+1));

			for(unsigned int j=0; j<m_oChannelLocalisationLabels.size(); j++)
			{
				if(strcmp(l_oChannelLabel.c_str(), m_oChannelLocalisationLabels[j].toASCIIString()) == 0)
				{
					l_bLabelRecognized = true;
					m_oChannelLookupIndices[i] = j;
					break;
				}
			}
		}

		//unrecognized electrode!
		if(l_bLabelRecognized == false)
		{
			m_oParentPlugin.getLogManager() << LogLevel_Trace
				<< "Unrecognized electrode name (index=" << (uint32)i
				<< ", name=" << m_pDimmesionLabels[0][i].c_str()
				<< ")!\n";
			res = false;
		}
	}

	m_oParentPlugin.getLogManager() << LogLevel_Trace << "Electrodes list : " ;

	for(uint32 i=0; i<m_pDimmensionSizes[0]; i++)
	{
		m_oParentPlugin.getLogManager() << CString(m_pDimmesionLabels[0][i].c_str());
		if(i<m_pDimmensionSizes[0]-1)
		{
			m_oParentPlugin.getLogManager() << ", ";
		}
		else
		{
			m_oParentPlugin.getLogManager() << "\n";
		}
	}

	if(res == true)
	{
		m_bChannelLookupTableInitialized = true;
	}

	return res;
}

boolean CBufferDatabase::convertCartesianToSpherical(const float64* pCartesianCoords, float64& rTheta, float64& rPhi)
{
#define MY_THRESHOLD 1e-3
#define PI 3.1415926535

	static float64 l_f64RadToDeg = 180 / PI;

	//compute theta
	rTheta = acos(pCartesianCoords[2]) * l_f64RadToDeg;

	//compute phi so that it lies in [0, 360]
	if(fabs(pCartesianCoords[0]) < MY_THRESHOLD)
	{
		rPhi = (pCartesianCoords[1] > 0) ? 90 : 270;
	}
	else
	{
		rPhi = atan(pCartesianCoords[1] / pCartesianCoords[0]) * l_f64RadToDeg;

		if(pCartesianCoords[0] < 0)
		{
			rPhi += 180;
		}
		else if(pCartesianCoords[1] < 0)
		{
			rPhi += 360;
		}
	}

	return true;
}
