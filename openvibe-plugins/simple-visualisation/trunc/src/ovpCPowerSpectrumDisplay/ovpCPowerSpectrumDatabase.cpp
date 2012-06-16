#include "ovpCPowerSpectrumDatabase.h"
#include "../ovpCBufferDatabase.h"

#include <system/Memory.h>

#include <algorithm>

#include <cmath>

using namespace OpenViBE;
using namespace Plugins;
using namespace Kernel;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

using namespace OpenViBEToolkit;

using namespace std;

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{

		CPowerSpectrumDatabase::CPowerSpectrumDatabase(OpenViBEToolkit::TBoxAlgorithm<Plugins::IBoxAlgorithm>& oPlugin) :
			m_oParentPlugin(oPlugin),
			m_pDrawable(NULL),
			m_pBuffer(NULL),
			m_bFirstBufferReceived(false),
			m_ui32MinDisplayedFrequencyBand(0),
			m_ui32MaxDisplayedFrequencyBand(0),
			m_f64MinDisplayedValue(+DBL_MAX),
			m_f64MaxDisplayedValue(-DBL_MAX)
		{
		}

		CPowerSpectrumDatabase::~CPowerSpectrumDatabase()
		{
			if(m_pBuffer)
			{
				delete[] m_pBuffer;
			}
		}

		void CPowerSpectrumDatabase::setChannelCount(const uint32 ui32ChannelCount)
		{
			m_pChannelLabels.resize(ui32ChannelCount);
			m_oMinMaxDisplayedValues.resize(ui32ChannelCount);
		}

		void CPowerSpectrumDatabase::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
		{
			if(ui32ChannelIndex >= m_pChannelLabels.size())
				m_pChannelLabels.resize(ui32ChannelIndex+1);

			m_pChannelLabels[ui32ChannelIndex] = sChannelName;
		}

		void CPowerSpectrumDatabase::setFrequencyBandCount(const uint32 ui32FrequencyBandCount)
		{
			m_pFrequencyBandLabels.resize(ui32FrequencyBandCount);
			m_pFrequencyBands.resize(ui32FrequencyBandCount);
		}

		void CPowerSpectrumDatabase::setFrequencyBandName(const uint32 ui32FrequencyBandIndex, const char* sFrequencyBandName)
		{
			if(ui32FrequencyBandIndex >= m_pFrequencyBandLabels.size())
				m_pFrequencyBandLabels.resize(ui32FrequencyBandIndex+1);

			m_pFrequencyBandLabels[ui32FrequencyBandIndex] = sFrequencyBandName;
		}

		void CPowerSpectrumDatabase::setFrequencyBandStart(const uint32 ui32FrequencyBandIndex, const float64 f64FrequencyBandStart)
		{
			if(ui32FrequencyBandIndex >= m_pFrequencyBandLabels.size())
				m_pFrequencyBands.resize(ui32FrequencyBandIndex+1);

			m_pFrequencyBands[ui32FrequencyBandIndex].first = f64FrequencyBandStart;
		}

		void CPowerSpectrumDatabase::setFrequencyBandStop(const uint32 ui32FrequencyBandIndex, const float64 f64FrequencyBandStop)
		{
			if(ui32FrequencyBandIndex >= m_pFrequencyBandLabels.size())
				m_pFrequencyBands.resize(ui32FrequencyBandIndex+1);

			m_pFrequencyBands[ui32FrequencyBandIndex].second = f64FrequencyBandStop;
		}

		void CPowerSpectrumDatabase::setBuffer(const float64* pBuffer)
		{
			//init some members when receiving first buffer
			if(m_bFirstBufferReceived == false)
			{
				//initialize displayed frequency bands
				if(m_pFrequencyBands.size() > 0)
				{
					m_ui32MinDisplayedFrequencyBand = 0;
					m_ui32MaxDisplayedFrequencyBand = m_pFrequencyBands.size()-1;
				}

				m_pDrawable->init();

				m_pBuffer = new float64[(size_t)m_pChannelLabels.size()*m_pFrequencyBandLabels.size()];

				m_bFirstBufferReceived = true;
			}

			System::Memory::copy(m_pBuffer, pBuffer, m_pChannelLabels.size()*m_pFrequencyBandLabels.size()*sizeof(float64));

			const float64* pCurChannel = pBuffer;

			m_f64MinDisplayedValue = 0;
			m_f64MaxDisplayedValue = 0;

			//for each channel
			for(uint32 c=0; c<m_pChannelLabels.size(); c++, pCurChannel += m_pFrequencyBands.size())
			{
				m_oMinMaxDisplayedValues[c].first = 0;
				m_oMinMaxDisplayedValues[c].second = 0;

				//for each displayed frequency band
				for(uint64 i=m_ui32MinDisplayedFrequencyBand; i<m_ui32MaxDisplayedFrequencyBand; i++)
				{
					//update channel min/max values
					if(pCurChannel[i] < m_oMinMaxDisplayedValues[c].first)
					{
						m_oMinMaxDisplayedValues[c].first = pCurChannel[i];
					}
					else if(pCurChannel[i] > m_oMinMaxDisplayedValues[c].second)
					{
						m_oMinMaxDisplayedValues[c].second = pCurChannel[i];
					}
				}

				//update global min/max values
				if(m_oMinMaxDisplayedValues[c].first < m_f64MinDisplayedValue)
				{
					m_f64MinDisplayedValue = m_oMinMaxDisplayedValues[c].first;
				}
				if(m_oMinMaxDisplayedValues[c].second > m_f64MaxDisplayedValue)
				{
					m_f64MaxDisplayedValue = m_oMinMaxDisplayedValues[c].second;
				}
			}

			//tells the drawable to redraw himself since the signal information has been updated
			m_pDrawable->redraw();
		}

		uint32 CPowerSpectrumDatabase::getChannelCount()
		{
			return m_pChannelLabels.size();
		}

		boolean CPowerSpectrumDatabase::getChannelLabel(uint32 ui32ChannelIndex, CString& rChannelLabel)
		{
			if(ui32ChannelIndex >= m_pChannelLabels.size())
			{
				rChannelLabel = "";
				return false;
			}
			else
			{
				rChannelLabel = m_pChannelLabels[ui32ChannelIndex].c_str();
				return true;
			}
		}

		boolean CPowerSpectrumDatabase::setMinDisplayedFrequency(float64 f64MinDisplayedFrequency)
		{
			if(m_pFrequencyBands.size() == 0)
			{
				return false;
			}

			if(f64MinDisplayedFrequency < m_pFrequencyBands[0].first)
			{
				m_ui32MinDisplayedFrequencyBand = 0;
			}
			else
			{
				uint32 i=0;
				for(i=0; i<m_pFrequencyBands.size(); i++)
				{
					if(f64MinDisplayedFrequency < m_pFrequencyBands[i].second)
					{
						break;
					}
				}
				m_ui32MinDisplayedFrequencyBand = i;
			}
			return true;
		}

		boolean CPowerSpectrumDatabase::setMaxDisplayedFrequency(float64 f64MaxDisplayedFrequency)
		{
			if(m_pFrequencyBands.size() == 0)
			{
				return false;
			}

			if(f64MaxDisplayedFrequency > m_pFrequencyBands.back().second)
			{
				m_ui32MaxDisplayedFrequencyBand = m_pFrequencyBands.size()-1;
			}
			else
			{
				uint32 i=0;
				for(i=0; i<m_pFrequencyBands.size(); i++)
				{
					if(f64MaxDisplayedFrequency <= m_pFrequencyBands[i].second)
					{
						break;
					}
				}
				m_ui32MaxDisplayedFrequencyBand = i;
			}
			return true;
		}

		boolean CPowerSpectrumDatabase::getInputFrequencyRange(float64& f64MinInputFrequency, float64& f64MaxInputFrequency)
		{
			if(m_pFrequencyBands.size() == 0)
			{
				return false;
			}
			f64MinInputFrequency = m_pFrequencyBands.front().first;
			f64MaxInputFrequency = m_pFrequencyBands.back().second;
			return true;
		}

		uint32 CPowerSpectrumDatabase::getDisplayedFrequencyBandCount()
		{
			if(m_ui32MaxDisplayedFrequencyBand < m_ui32MinDisplayedFrequencyBand)
			{
				return 0;
			}
			else
			{
				return m_ui32MaxDisplayedFrequencyBand - m_ui32MinDisplayedFrequencyBand + 1;
			}
		}

		uint32 CPowerSpectrumDatabase::getMinDisplayedFrequencyBandIndex()
		{
			return m_ui32MinDisplayedFrequencyBand;
		}

		uint32 CPowerSpectrumDatabase::getMaxDisplayedFrequencyBandIndex()
		{
			return m_ui32MaxDisplayedFrequencyBand;
		}

		boolean CPowerSpectrumDatabase::getFrequencyBandRange(uint32 ui32FrequencyBandIndex, float64& rFrequencyBandStart, float64& rFrequencyBandStop)
		{
			if(ui32FrequencyBandIndex >= m_pFrequencyBands.size())
			{
				return false;
			}
			rFrequencyBandStart = m_pFrequencyBands[ui32FrequencyBandIndex].first;
			rFrequencyBandStop = m_pFrequencyBands[ui32FrequencyBandIndex].second;
			return true;
		}

		void CPowerSpectrumDatabase::setDrawable(CSignalDisplayDrawable * pDrawable)
		{
			m_pDrawable=pDrawable;
		}

		boolean CPowerSpectrumDatabase::getLastBufferChannelMinMaxValue(uint32 ui32Channel, float64& f64Min, float64& f64Max)
		{
			if(ui32Channel >= m_oMinMaxDisplayedValues.size())
			{
				return false;
			}
			f64Min = m_oMinMaxDisplayedValues[ui32Channel].first;
			f64Max = m_oMinMaxDisplayedValues[ui32Channel].second;
			return true;
		}

		boolean CPowerSpectrumDatabase::getLastBufferMinMaxValue(float64& f64Min, float64& f64Max)
		{
			if(m_bFirstBufferReceived == false)
			{
				return false;
			}
			f64Min = m_f64MinDisplayedValue;
			f64Max = m_f64MaxDisplayedValue;
			return true;
		}

		float64* CPowerSpectrumDatabase::getLastBufferChannelPointer(uint32 ui32Channel)
		{
			if(m_pBuffer == NULL)
			{
				return NULL;
			}
			else
			{
				return m_pBuffer + ui32Channel * m_pFrequencyBands.size();
			}
		}
/*
		void CPowerSpectrumDatabase::getDisplayedChannelMinMaxValue(uint32 ui32Channel, float64& f64Min, float64& f64Max)
		{
			f64Min = +DBL_MAX;
			f64Max = -DBL_MAX;

			for(uint64 i=0 ; i<m_oLocalMinMaxValue[ui32Channel].size() ; i++)
			{
				if(f64Min > m_oLocalMinMaxValue[ui32Channel][i].first)
				{
					f64Min = m_oLocalMinMaxValue[ui32Channel][i].first;
				}
				if(f64Max < m_oLocalMinMaxValue[ui32Channel][i].second)
				{
					f64Max = m_oLocalMinMaxValue[ui32Channel][i].second;
				}
			}
		}

		void CPowerSpectrumDatabase::getDisplayedGlobalMinMaxValue(float64& f64Min, float64& f64Max)
		{
			for(uint32 c=0 ; c<m_oLocalMinMaxValue.size() ; c++)
			{
				for(uint64 i=0 ; i<m_oLocalMinMaxValue[c].size() ; i++)
				{
					if(f64Min > m_oLocalMinMaxValue[c][i].first)
					{
						f64Min = m_oLocalMinMaxValue[c][i].first;
					}
					if(f64Max < m_oLocalMinMaxValue[c][i].second)
					{
						f64Max = m_oLocalMinMaxValue[c][i].second;
					}
				}
			}
		}
*/
	}
}
