#include <system/Memory.h>
#include <algorithm>
#include <cmath>

#include "ovpCStreamedMatrixDatabase.h"

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

using namespace OpenViBEToolkit;

using namespace std;

#define convert_floating_time(f) ((uint64)f<<32) + (uint64)((f - (uint64)f) * (float64)((uint64)1<<32))
//#define convert_uinteger_time(i) (float64)(i>>32) + (float64)((float64)(i&0xFFFFFFFF) / (float64)((uint64)1<<32))

CStreamedMatrixDatabase::CStreamedMatrixDatabase(OpenViBEToolkit::TBoxAlgorithm<Plugins::IBoxAlgorithm>& oPlugin) :
	m_oParentPlugin(oPlugin),
	m_pDecoder(NULL),
	m_pDrawable(NULL),
	m_bRedrawOnNewData(true),
	m_bFirstBufferReceived(false),
	m_bBufferTimeStepComputed(false),
	m_ui64BufferTimeStep(0),
	//m_ui32SamplingFrequency(0),
	m_ui32MaxBufferCount(2), //store at least 2 buffers so that it is possible to determine whether they overlap
	m_bIgnoreTimeScale(false),
	m_f64TimeScale(10)
	//m_bError(false)
{
}

CStreamedMatrixDatabase::~CStreamedMatrixDatabase()
{
	if(m_pDecoder != NULL)
	{
		m_pDecoder->uninitialize();
		m_oParentPlugin.getAlgorithmManager().releaseAlgorithm(*m_pDecoder);
	}

	while(m_oStreamedMatrices.size() >0)
	{
		delete m_oStreamedMatrices.front();
		m_oStreamedMatrices.pop_front();
	}
}

boolean CStreamedMatrixDatabase::initialize()
{
	if(m_pDecoder != NULL)
	{
		return false;
	}

	m_pDecoder = &m_oParentPlugin.getAlgorithmManager().getAlgorithm(
		m_oParentPlugin.getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));

	m_pDecoder->initialize();

	return true;
}

void CStreamedMatrixDatabase::setDrawable(IStreamDisplayDrawable* pDrawable)
{
	m_pDrawable = pDrawable;
}

void CStreamedMatrixDatabase::setRedrawOnNewData(boolean bRedrawOnNewData)
{
	m_bRedrawOnNewData = bRedrawOnNewData;
}

boolean CStreamedMatrixDatabase::isFirstBufferReceived()
{
	return m_bFirstBufferReceived;
}

boolean CStreamedMatrixDatabase::setMaxBufferCount(uint32 ui32MaxBufferCount)
{
	//max buffer count computed directly
	m_bIgnoreTimeScale = true;

	m_ui32MaxBufferCount = ui32MaxBufferCount;

	onBufferCountChanged();

	return true;
}

boolean CStreamedMatrixDatabase::setTimeScale(float64 f64TimeScale)
{
	if(f64TimeScale <= 0)
	{
		return false;
	}

	//max buffer count computed from time scale
	m_bIgnoreTimeScale = false;

	//update time scale
	m_f64TimeScale = f64TimeScale;

	//if step between buffers is not known yet, this method will have to be called again later
	if(m_bBufferTimeStepComputed == false)
	{
		return false;
	}

	//compute maximum number of buffers needed to cover time scale
	uint32 l_ui32MaxBufferCount = 0;

	if(m_ui64BufferTimeStep > 0)
	{
		uint64 l_ui64TimeScale = convert_floating_time(m_f64TimeScale);
		l_ui32MaxBufferCount = (uint32)(ceil((float64)l_ui64TimeScale / m_ui64BufferTimeStep));
	}

	//display at least one buffer
	if(l_ui32MaxBufferCount == 0)
	{
		l_ui32MaxBufferCount = 1;
	}

	//acknowledge maximum buffer count
	boolean l_bMaxBufferCountChanged = false;
	if(l_ui32MaxBufferCount != m_ui32MaxBufferCount)
	{
		m_ui32MaxBufferCount = l_ui32MaxBufferCount;
		l_bMaxBufferCountChanged = true;
		onBufferCountChanged();
	}

	return l_bMaxBufferCountChanged;
}

boolean CStreamedMatrixDatabase::onBufferCountChanged()
{
	//if new number of buffers is smaller than before, destroy extra buffers
	while(m_oStreamedMatrices.size() > m_ui32MaxBufferCount)
	{
		delete m_oStreamedMatrices.front();
		m_oStreamedMatrices.pop_front();
		m_oStartTime.pop_front();
		m_oEndTime.pop_front();
		for(uint32 c=0 ; c<getChannelCount(); c++)
		{
			m_oChannelMinMaxValues[c].pop_front();
		}
	}

	return true;
}

boolean CStreamedMatrixDatabase::decodeMemoryBuffer(const IMemoryBuffer* pMemoryBuffer, uint64 ui64StartTime, uint64 ui64EndTime)
{
	//feed memory buffer to algorithm
	m_pDecoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode)->setReferenceTarget(&pMemoryBuffer);

	//process buffer
	m_pDecoder->process();

	//has flow header been received?
	if(m_pDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader) == true)
	{
		decodeHeader();

		//create widgets
		m_pDrawable->init();
	}

	//has a buffer been received?
	if(m_pDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer) == true)
	{
		decodeBuffer(ui64StartTime, ui64EndTime);

		//redraw widgets
		if(m_bRedrawOnNewData)
		{
			m_pDrawable->redraw();
		}
	}

	return true;
}

uint32 CStreamedMatrixDatabase::getMaxBufferCount()
{
	return m_ui32MaxBufferCount;
}

uint32 CStreamedMatrixDatabase::getCurrentBufferCount()
{
	return (uint32)m_oStreamedMatrices.size();
}

const float64* CStreamedMatrixDatabase::getBuffer(uint32 ui32Index)
{
	if(ui32Index >= m_oStreamedMatrices.size())
	{
		return NULL;
	}
	else
	{
		return m_oStreamedMatrices[ui32Index]->getBuffer();
	}
}

uint64 CStreamedMatrixDatabase::getStartTime(uint32 ui32BufferIndex)
{
	if(ui32BufferIndex >= m_oStartTime.size())
	{
		return 0;
	}
	else
	{
		return m_oStartTime[ui32BufferIndex];
	}
}

uint64 CStreamedMatrixDatabase::getEndTime(uint32 ui32BufferIndex)
{
	if(ui32BufferIndex >= m_oEndTime.size())
	{
		return 0;
	}
	else
	{
		return m_oEndTime[ui32BufferIndex];
	}
}

uint32 CStreamedMatrixDatabase::getBufferElementCount()
{
	if(m_oStreamedMatrices.size() == 0)
	{
		return 0;
	}
	else
	{
		return m_oStreamedMatrices[0]->getBufferElementCount();
	}
}

uint64 CStreamedMatrixDatabase::getBufferDuration()
{
	if(m_oStartTime.size() == 0 || m_oEndTime.size() == 0)
	{
		return 0;
	}
	else
	{
		return m_oEndTime[0] - m_oStartTime[0];
	}
}

boolean CStreamedMatrixDatabase::isBufferTimeStepComputed()
{
	return m_bBufferTimeStepComputed;
}

uint64 CStreamedMatrixDatabase::getBufferTimeStep()
{
	if(m_bBufferTimeStepComputed == false)
	{
		return 0;
	}
	else
	{
		return m_ui64BufferTimeStep;
	}
}

uint32 CStreamedMatrixDatabase::getSampleCountPerBuffer()
{
	if(m_oStreamedMatrixHeader.getDimensionCount() == 0)
	{
		return 0;
	}
	else
	{
		return m_oStreamedMatrixHeader.getDimensionSize(1);
	}
}

uint32 CStreamedMatrixDatabase::getChannelCount()
{
	if(m_oStreamedMatrixHeader.getDimensionCount() == 0)
	{
		return 0;
	}
	else
	{
		return m_oStreamedMatrixHeader.getDimensionSize(0);
	}
}

boolean CStreamedMatrixDatabase::getChannelLabel(const uint32 ui32ChannelIndex, CString& rElectrodeLabel)
{
	if(m_oStreamedMatrixHeader.getDimensionCount() == 0 || m_oStreamedMatrixHeader.getDimensionSize(0) <= ui32ChannelIndex)
	{
		rElectrodeLabel = "";
		return false;
	}
	else
	{
		rElectrodeLabel = m_oStreamedMatrixHeader.getDimensionLabel(0, ui32ChannelIndex);
		return true;
	}
}

boolean CStreamedMatrixDatabase::getChannelMinMaxValues(uint32 ui32Channel, float64& f64Min, float64& f64Max)
{
	f64Min = +DBL_MAX;
	f64Max = -DBL_MAX;

	if(m_bFirstBufferReceived == false)
	{
		return false;
	}

	if(ui32Channel >= getChannelCount())
	{
		return false;
	}

	for(uint64 i=0; i<m_oChannelMinMaxValues[(size_t)ui32Channel].size(); i++)
	{
		if(f64Min > m_oChannelMinMaxValues[(size_t)ui32Channel][(size_t)i].first)
		{
			f64Min = m_oChannelMinMaxValues[(size_t)ui32Channel][(size_t)i].first;
		}
		if(f64Max < m_oChannelMinMaxValues[(size_t)ui32Channel][(size_t)i].second)
		{
			f64Max = m_oChannelMinMaxValues[(size_t)ui32Channel][(size_t)i].second;
		}
	}

	return true;
}

boolean CStreamedMatrixDatabase::getGlobalMinMaxValues(float64& f64Min, float64& f64Max)
{
	f64Min = +DBL_MAX;
	f64Max = -DBL_MAX;

	if(m_bFirstBufferReceived == false)
	{
		return false;
	}

	for(uint32 c=0; c<getChannelCount(); c++)
	{
		for(uint64 i=0; i<m_oChannelMinMaxValues[(size_t)c].size(); i++)
		{
			if(f64Min > m_oChannelMinMaxValues[(size_t)c][(size_t)i].first)
			{
				f64Min = m_oChannelMinMaxValues[(size_t)c][(size_t)i].first;
			}
			if(f64Max < m_oChannelMinMaxValues[(size_t)c][(size_t)i].second)
			{
				f64Max = m_oChannelMinMaxValues[(size_t)c][(size_t)i].second;
			}
		}
	}

	return true;
}

boolean CStreamedMatrixDatabase::getLastBufferChannelMinMaxValues(uint32 ui32Channel, float64& f64Min, float64& f64Max)
{
	f64Min = +DBL_MAX;
	f64Max = -DBL_MAX;

	if(m_bFirstBufferReceived == false)
	{
		return false;
	}

	if(ui32Channel >= getChannelCount())
	{
		return false;
	}

	f64Min = m_oChannelMinMaxValues[ui32Channel].back().first;
	f64Max = m_oChannelMinMaxValues[ui32Channel].back().second;
	return true;
}

boolean CStreamedMatrixDatabase::getLastBufferGlobalMinMaxValues(float64& f64Min, float64& f64Max)
{
	f64Min = +DBL_MAX;
	f64Max = -DBL_MAX;

	if(m_bFirstBufferReceived == false)
	{
		return false;
	}

	for(uint32 c=0; c<m_oChannelMinMaxValues.size(); c++)
	{
		if(f64Min > m_oChannelMinMaxValues[c].back().first)
		{
			f64Min = m_oChannelMinMaxValues[c].back().first;
		}
		if(f64Max < m_oChannelMinMaxValues[c].back().second)
		{
			f64Max = m_oChannelMinMaxValues[c].back().second;
		}
	}

	return true;
}

boolean CStreamedMatrixDatabase::decodeHeader()
{
	//copy streamed matrix header
	OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > l_oStreamedMatrix;
	l_oStreamedMatrix.initialize(m_pDecoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));
	Tools::Matrix::copyDescription(m_oStreamedMatrixHeader, *l_oStreamedMatrix);

	m_oChannelMinMaxValues.resize(getChannelCount());

	return true;
}

boolean CStreamedMatrixDatabase::decodeBuffer(uint64 ui64StartTime, uint64 ui64EndTime)
{
	//first buffer received
	if(m_bFirstBufferReceived == false)
	{
		uint64 l_ui64BufferDuration = ui64EndTime - ui64StartTime;

		if(l_ui64BufferDuration == 0)
		{
			m_oParentPlugin.getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Error : buffer start time and end time are equal : " << ui64StartTime << "\n";
			//m_bError = true;
			return false;
		}

		//compute sampling frequency
		//m_ui32SamplingFrequency = (uint32)((float64)(((uint64)1<<32)/(l_ui64BufferDuration)) * getSampleCountPerBuffer());

		m_bFirstBufferReceived = true;
	}

	//compute time step between two buffers
	if(m_bBufferTimeStepComputed == false && m_oStreamedMatrices.size() >= 2)
	{
		m_ui64BufferTimeStep = m_oStartTime[1] - m_oStartTime[0];
		m_bBufferTimeStepComputed = true;

		if(m_bIgnoreTimeScale == false)
		{
			//compute maximum number of buffers from time scale
			setTimeScale(m_f64TimeScale);
		}
	}

	//store new buffer data
	CMatrix* l_pCurrentMatrix = NULL;
	if(m_oStreamedMatrices.size() < m_ui32MaxBufferCount)
	{
		l_pCurrentMatrix = new CMatrix();
		Tools::Matrix::copyDescription(*l_pCurrentMatrix, m_oStreamedMatrixHeader);
		m_oStreamedMatrices.push_back(l_pCurrentMatrix);
	}
	else //reuse memory for new buffer
	{
		//move front matrix to back of list
		l_pCurrentMatrix = m_oStreamedMatrices.front();
		m_oStreamedMatrices.push_back(l_pCurrentMatrix);

		//remove first matrix data
		m_oStreamedMatrices.pop_front();
		m_oStartTime.pop_front();
		m_oEndTime.pop_front();
		for(uint32 c=0 ; c<getChannelCount(); c++)
		{
			m_oChannelMinMaxValues[c].pop_front();
		}
	}

	//store samples
	OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > l_oStreamedMatrix;
	l_oStreamedMatrix.initialize(m_pDecoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));
	Tools::Matrix::copyContent(*l_pCurrentMatrix, *l_oStreamedMatrix);

	//store time stamps
	m_oStartTime.push_back(ui64StartTime);
	m_oEndTime.push_back(ui64EndTime);

	//store min/max values
	float64* l_pBuffer = l_pCurrentMatrix->getBuffer();

	for(uint32 c=0; c<getChannelCount(); c++)
	{
		float64 l_f64ChannelMin = DBL_MAX;
		float64 l_f64ChannelMax = -DBL_MAX;

		for(uint64 i=0; i<getSampleCountPerBuffer(); i++, l_pBuffer++)
		{
			if(*l_pBuffer < l_f64ChannelMin)
			{
				l_f64ChannelMin = *l_pBuffer;
			}
			if(*l_pBuffer > l_f64ChannelMax)
			{
				l_f64ChannelMax = *l_pBuffer;
			}
		}

		m_oChannelMinMaxValues[c].push_back(pair<float64, float64>(l_f64ChannelMin, l_f64ChannelMax));
	}

	return true;
}
