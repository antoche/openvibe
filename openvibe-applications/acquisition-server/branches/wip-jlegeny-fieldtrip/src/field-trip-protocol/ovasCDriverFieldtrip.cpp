/* This driver uses the FieldTrip buffer open source library. 
 * See http://www.ru.nl/fcdonders/fieldtrip for details.
 */

#include "ovasCDriverFieldtrip.h"
#include "ovasCConfigurationFieldtrip.h"
#include <openvibe-toolkit/ovtk_all.h>

#include <pthread.h>
#include "fieldtrip/buffer.h"
#include "fieldtrip/extern.h"
#include "fieldtrip/extern.c"
#include "fieldtrip/util.c"
#include "fieldtrip/printstruct.c"
#include "fieldtrip/tcprequest.c"
#include "fieldtrip/dmarequest.c"
#include "fieldtrip/clientrequest.c"


#include <system/Time.h>
//#include "GetCpuTime.h"

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

//___________________________________________________________________//
//                                                                   //

CDriverFieldtrip::CDriverFieldtrip(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pCallback(NULL)
	,m_ui32SampleCountPerSentBlock(0)
	,m_pSample(NULL)
	,m_sHostName("localhost")
	,m_ui32PortNumber(1979)
	,m_i32ConnectionID(-1)
	,m_ui32DataType(DATATYPE_UNKNOWN)
	,m_ui32MinSamples(1)
	,m_bFirstGetDataRequest(false)
	,m_bCorrectNonIntegerSR(true)
{
	m_oHeader.setSamplingFrequency(0);
	m_oHeader.setChannelCount(0);

	m_pWaitData_Request = new message_t();
	m_pWaitData_Request->def = new messagedef_t();
	m_pWaitData_Request->buf = NULL;

	m_pGetData_Request = new message_t();
	m_pGetData_Request->def = new messagedef_t();
	m_pGetData_Request->buf = NULL;

}

CDriverFieldtrip::~CDriverFieldtrip(void)
{
	if ( m_pWaitData_Request )
	{
		//m_pWaitData_Request->buf deleted with m_pWaitData_Request->def
		if ( m_pWaitData_Request->def ) delete m_pWaitData_Request->def;
		delete m_pWaitData_Request;
	}

	if ( m_pGetData_Request )
	{
		//m_pGetData_Request->buf deleted with m_pGetData_Request->def
		if ( m_pGetData_Request->def ) delete m_pGetData_Request->def;
		delete m_pGetData_Request;
	}
}

const char* CDriverFieldtrip::getName(void)
{
	return "Fieldtrip Driver";
}

//___________________________________________________________________//
//                                                                   //

OpenViBE::boolean CDriverFieldtrip::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	if (m_rDriverContext.isConnected()) return false;

	// ...
	// initialize hardware and get available header information
	// from it :

	// connect to buffer
	if ( m_i32ConnectionID != -1 )
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Already connected to Fieldtrip buffer "
										 <<  m_sHostName.toASCIIString() << ":" << m_ui32PortNumber <<"\n";
		return false;
	}

	m_i32ConnectionID = open_connection(m_sHostName.toASCIIString(), (int)m_ui32PortNumber);
	if ( m_i32ConnectionID < 0 )
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Failed to connect to Fieldtrip buffer :\n"
										 <<  m_sHostName.toASCIIString() << ":" << m_ui32PortNumber <<"\n";
		m_i32ConnectionID = -1;
		return false;
	}
	else
	{
		m_rDriverContext.getLogManager() << LogLevel_Info << "Connection to Fieldtrip buffer succeeded !\n";
	}

	// request header
	if ( !requestHeader() )
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Request header failed, disconnecting.\n";
		if ( close_connection(m_i32ConnectionID) != 0 )
		{
			m_rDriverContext.getLogManager() << LogLevel_Error << "Failed to disconnect correctly from Fieldtrip buffer\n";
		}
		m_i32ConnectionID = -1;
		return false;
	}


	if (!m_oHeader.isChannelCountSet()||!m_oHeader.isSamplingFrequencySet()) return false;

	// Builds up a buffer to store acquired samples. This buffer
	// will be sent to the acquisition server later...
	m_pSample = new float32[m_oHeader.getChannelCount()*ui32SampleCountPerSentBlock];
	if (!m_pSample)
	{
		delete [] m_pSample;
		m_pSample = NULL;
		return false;
	}

	// Saves parameters
	m_pCallback = &rCallback;
	m_ui32SampleCountPerSentBlock = ui32SampleCountPerSentBlock;

	if ( m_ui32MinSamples < 1 )
	{
		m_ui32MinSamples = 1;
	}
	if ( m_ui32MinSamples > m_ui32SampleCountPerSentBlock )
	{
		m_ui32MinSamples = m_ui32SampleCountPerSentBlock;
	}

	return true;
}

OpenViBE::boolean CDriverFieldtrip::start(void)
{
	if (!m_rDriverContext.isConnected()) return false;
	if (m_rDriverContext.isStarted()) return false;

	// ...
	// request hardware to start
	// sending data
	// ...
	m_bFirstGetDataRequest = true;
	m_ui32WaitingTimeMs = (m_oHeader.getSamplingFrequency()>1000 ? 1:(1000/m_oHeader.getSamplingFrequency()));  //time for 1 sample if >= 1ms //(1000*m_ui32SampleCountPerSentBlock)
	m_ui32TotalSampleCount=0;

	m_f64DiffPerSample = (m_f64RealSamplingRate - m_oHeader.getSamplingFrequency()) / m_f64RealSamplingRate;
	if ( m_f64DiffPerSample <= 0.0 )
		m_f64DiffPerSample = 0.0;
	m_f64DriftSinceLastCorrection = 0.0;

	return true;
}

OpenViBE::boolean CDriverFieldtrip::loop(void)
{
	if (!m_rDriverContext.isConnected()) return false;
	if (!m_rDriverContext.isStarted()) return true;

	OpenViBE::CStimulationSet l_oStimulationSet;
	l_oStimulationSet.setStimulationCount(0);

	// ...
	// receive samples from hardware
	// put them the correct way in the sample array
	// whether the buffer is full, send it to the acquisition server
	//...
	int32 l_iSampleCount = requestChunk(l_oStimulationSet);
	if ( l_iSampleCount < 0 )
	{
		return false;
	}
	else if ( l_iSampleCount == 0 )
	{
		return true;
	}
	m_pCallback->setSamples(m_pSample, l_iSampleCount);
	m_pCallback->setStimulationSet(l_oStimulationSet);

	m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());

	return true;
}

OpenViBE::boolean CDriverFieldtrip::stop(void)
{
	if (!m_rDriverContext.isConnected()) return false;
	if (!m_rDriverContext.isStarted()) return false;

	return true;
}

OpenViBE::boolean CDriverFieldtrip::uninitialize(void)
{
	if (!m_rDriverContext.isConnected()) return false;
	if (m_rDriverContext.isStarted()) return false;

	if ( close_connection(m_i32ConnectionID) != 0 )
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Failed to disconnect correctly from Fieldtrip buffer\n";
	}
	m_i32ConnectionID = -1;

	delete [] m_pSample;
	m_pSample=NULL;
	m_pCallback=NULL;

	return true;
}

//___________________________________________________________________//
//                                                                   //
OpenViBE::boolean CDriverFieldtrip::isConfigurable(void)
{
	return true; // change to false if your device is not configurable
}

OpenViBE::boolean CDriverFieldtrip::configure(void)
{
	CConfigurationFieldtrip l_oConfiguration("../share/openvibe-applications/acquisition-server/interface-Fieldtrip.ui");
	l_oConfiguration.setMinSamples(m_ui32MinSamples);
	l_oConfiguration.setHostPort(m_ui32PortNumber);
	l_oConfiguration.setHostName(m_sHostName);
	l_oConfiguration.setSRCorrection(m_bCorrectNonIntegerSR);

	if (l_oConfiguration.configure(m_oHeader))
	{
		m_ui32MinSamples = l_oConfiguration.getMinSamples();
		m_ui32PortNumber = l_oConfiguration.getHostPort();
		m_sHostName = l_oConfiguration.getHostName();
		m_bCorrectNonIntegerSR = l_oConfiguration.getSRCorrection();
		return true;
	}
	return false;
}

//___________________________________________________________________//
//                                                                   //
OpenViBE::boolean CDriverFieldtrip::requestHeader()
{
	m_pWaitData_Request->def->command = GET_HDR;
	m_pWaitData_Request->def->version = VERSION;
	m_pWaitData_Request->def->bufsize = 0;
	m_pWaitData_Request->buf = NULL;

	message_t *l_pResponse = NULL;

	int l_iRes = clientrequest(m_i32ConnectionID, m_pWaitData_Request, &l_pResponse);

	if ( l_iRes!=0 )
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Error while asking for header. Buffer aborted ?\n";
		if ( l_pResponse )
		{
			if (l_pResponse->buf) free(l_pResponse->buf);
			if (l_pResponse->def) free(l_pResponse->def);
			free(l_pResponse);
			l_pResponse = NULL;
		}
		return false;
	}
	else if ( l_pResponse==NULL || l_pResponse->def==NULL )
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Error while asking for header\n";
		if ( l_pResponse )
		{
			if (l_pResponse->buf) free(l_pResponse->buf);
			if (l_pResponse->def) free(l_pResponse->def);
			free(l_pResponse);
			l_pResponse = NULL;
		}
		return false;
	}
	else if ( l_pResponse->def->command!=GET_OK || l_pResponse->def->bufsize==0 )
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "No header in the buffer\n";
		if ( l_pResponse )
		{
			if (l_pResponse->buf) free(l_pResponse->buf);
			if (l_pResponse->def) free(l_pResponse->def);
			free(l_pResponse);
			l_pResponse = NULL;
		}
		return false;
	}
	else
	{
		int l_iResponseBufSize = l_pResponse->def->bufsize;
		headerdef_t* l_pHeaderDef = (headerdef_t*) l_pResponse->buf;

		if ( l_iResponseBufSize < sizeof(headerdef_t) )
		{
			m_rDriverContext.getLogManager() << LogLevel_Error << "Header received has wrong format\n";
			if ( l_pResponse )
			{
				if (l_pResponse->buf) free(l_pResponse->buf);
				if (l_pResponse->def) free(l_pResponse->def);
				free(l_pResponse);
				l_pResponse = NULL;
			}
			return false;
		}

		m_oHeader.setSamplingFrequency((uint32)l_pHeaderDef->fsample);
		m_f64RealSamplingRate = l_pHeaderDef->fsample;
		m_oHeader.setChannelCount(l_pHeaderDef->nchans);
		m_ui32DataType = l_pHeaderDef->data_type;
		if ( m_ui32DataType != DATATYPE_FLOAT32 && m_ui32DataType != DATATYPE_FLOAT64 )
		{
			m_rDriverContext.getLogManager() << LogLevel_Error << "Data type is not supported\n";
			if ( l_pResponse )
			{
				if (l_pResponse->buf) free(l_pResponse->buf);
				if (l_pResponse->def) free(l_pResponse->def);
				free(l_pResponse);
				l_pResponse = NULL;
			}
			return false;
		}

		if ( l_iResponseBufSize == sizeof(headerdef_t) ) //no chunk attached to the header
		{
			for ( uint32 i = 0; i < l_pHeaderDef->nchans; i++ )
			{
				char l_sName[1024];
				sprintf(l_sName, "Channel %i", i);
				m_oHeader.setChannelName(i, l_sName);
			}
		}
		else //chunk(s) attached to the header, maybe channel names
		{
			int l_iBytesInHeaderBuffer = l_pHeaderDef->bufsize;
			void* l_pChunk = (headerdef_t*) l_pHeaderDef + 1;
			OpenViBE::boolean l_bFoundChannelNames = false;
			while( l_iBytesInHeaderBuffer > 0 )
			{
				if ( ((ft_chunk_t*)l_pChunk)->def.type == FT_CHUNK_CHANNEL_NAMES )
				{
					l_bFoundChannelNames = true;
					char* l_pChunkdata = ((ft_chunk_t*)l_pChunk)->data;
					for ( uint32 i = 0;  i < l_pHeaderDef->nchans; i++ )
					{
						std::string l_sChanName = l_pChunkdata;
						m_oHeader.setChannelName(i, l_sChanName.c_str());
						l_pChunkdata = (char*) l_pChunkdata + l_sChanName.size() + 1;
					}
				}

				l_iBytesInHeaderBuffer -= ((ft_chunk_t*)l_pChunk)->def.size + sizeof(ft_chunkdef_t);
				if ( l_iBytesInHeaderBuffer > 0 )
				{
					l_pChunk = (char*) l_pChunk + ((ft_chunk_t*)l_pChunk)->def.size + sizeof(ft_chunkdef_t);
				}
			}

			if ( !l_bFoundChannelNames )
			{
				for ( uint32 i = 0; i < l_pHeaderDef->nchans; i++ )
				{
					char l_sName[1024];
					sprintf(l_sName, "Channel %i", i);
					m_oHeader.setChannelName(i, l_sName);
				}
			}
		}
	} /* end valid header */

	if ( l_pResponse )
	{
		if (l_pResponse->buf) free(l_pResponse->buf);
		if (l_pResponse->def) free(l_pResponse->def);
		free(l_pResponse);
	}

	return true;
}


OpenViBE::int32 CDriverFieldtrip::requestChunk(OpenViBE::CStimulationSet& oStimulationSet)
{
	// "wait data" request
	m_pWaitData_Request->def->command = WAIT_DAT;
	m_pWaitData_Request->def->version = VERSION;

	if (m_pWaitData_Request->buf == NULL)
	{
		m_pWaitData_Request->def->bufsize = 0;
		waitdef_t* l_pWaitDef = new waitdef_t();
		unsigned int requestSize = 0;
		requestSize = append((void**)&m_pWaitData_Request->def, sizeof(messagedef_t), l_pWaitDef, sizeof(waitdef_t));
		m_pWaitData_Request->def->bufsize = requestSize - sizeof(messagedef_t);
		m_pWaitData_Request->buf = (messagedef_t*) m_pWaitData_Request->def+1;
	}
	waitdef_t* l_pWaitDef = (waitdef_t*) m_pWaitData_Request->buf;
	l_pWaitDef->threshold.nevents = 0xFFFFFFFF;
	l_pWaitDef->threshold.nsamples = m_ui32SampleCountPerSentBlock;
	l_pWaitDef->milliseconds = m_ui32WaitingTimeMs;

	message_t *l_pResponse = NULL;
	int l_iRes = clientrequest(m_i32ConnectionID, m_pWaitData_Request, &l_pResponse);

	uint32 l_ui32NbDataReceived = 0;
	uint32 l_ui32NbDataToSend = 0;

	if ( l_iRes )
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Error while asking for data. Buffer aborted ?\n";
		if ( l_pResponse )
		{
			if (l_pResponse->buf) free(l_pResponse->buf);
			if (l_pResponse->def) free(l_pResponse->def);
			free(l_pResponse);
			l_pResponse = NULL;
		}
		return -1;
	}
	else if ( l_pResponse==NULL || l_pResponse->def==NULL )
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Error while asking for data\n";
		if ( l_pResponse )
		{
			if (l_pResponse->buf) free(l_pResponse->buf);
			if (l_pResponse->def) free(l_pResponse->def);
			free(l_pResponse);
			l_pResponse = NULL;
		}
		return -1;
	}
	else if ( l_pResponse->def->command != WAIT_OK || l_pResponse->def->bufsize!=8 || l_pResponse->buf==NULL )
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "No header in buffer anymore\n";
		if ( l_pResponse )
		{
			if (l_pResponse->buf) free(l_pResponse->buf);
			if (l_pResponse->def) free(l_pResponse->def);
			free(l_pResponse);
			l_pResponse = NULL;
		}
		return -1;
	}
	else
	{
		// new header received ? stop acquisition
		if ( ((samples_events_t*) l_pResponse->buf)->nsamples < m_ui32TotalSampleCount )
		{
			m_rDriverContext.getLogManager() << LogLevel_Warning << "End of data\n";
			if ( l_pResponse )
			{
				if (l_pResponse->buf) free(l_pResponse->buf);
				if (l_pResponse->def) free(l_pResponse->def);
				free(l_pResponse);
				l_pResponse = NULL;
			}
			return -1;
		}

		// no new data
		if ( ((samples_events_t*) l_pResponse->buf)->nsamples <= m_ui32TotalSampleCount+m_ui32MinSamples )
		{
			m_rDriverContext.getLogManager() << LogLevel_Trace << "No new data\n";
			if ( l_pResponse )
			{
				if (l_pResponse->buf) free(l_pResponse->buf);
				if (l_pResponse->def) free(l_pResponse->def);
				free(l_pResponse);
				l_pResponse = NULL;
			}
			return 0;
		}

		// get data
		uint32 l_ui32LastSample = ((samples_events_t*) l_pResponse->buf)->nsamples;
		if ( l_ui32LastSample > m_ui32TotalSampleCount + m_ui32SampleCountPerSentBlock )
		{
			if ( m_bFirstGetDataRequest )
			{
				m_ui32TotalSampleCount = l_ui32LastSample - m_ui32SampleCountPerSentBlock;
			}
			else
			{
				l_ui32LastSample = m_ui32TotalSampleCount + m_ui32SampleCountPerSentBlock;
			}
		}
		if ( m_bFirstGetDataRequest )
		{
			m_bFirstGetDataRequest = false;
		}

		if ( l_pResponse )
		{
			if (l_pResponse->buf) free(l_pResponse->buf);
			if (l_pResponse->def) free(l_pResponse->def);
			free(l_pResponse);
			l_pResponse = NULL;
		}

		// "get data" request
		m_pGetData_Request->def->command = GET_DAT;
		m_pGetData_Request->def->version = VERSION;

		if (m_pGetData_Request->buf == NULL)
		{
			m_pGetData_Request->def->bufsize = 0;
			datasel_t* l_pDataSel = new datasel_t();
			unsigned int requestSize = 0;
			requestSize = append((void**)&m_pGetData_Request->def, sizeof(messagedef_t), l_pDataSel, sizeof(datasel_t));
			m_pGetData_Request->def->bufsize = requestSize - sizeof(messagedef_t);
			m_pGetData_Request->buf = (messagedef_t*) m_pGetData_Request->def+1;
		}
		datasel_t* l_pDataSel = (datasel_t*) m_pGetData_Request->buf;
		l_pDataSel->begsample = m_ui32TotalSampleCount;
		l_pDataSel->endsample = l_ui32LastSample - 1;

		l_iRes = clientrequest(m_i32ConnectionID, m_pGetData_Request, &l_pResponse);

		if ( l_iRes || !l_pResponse || !l_pResponse->def || l_pResponse->def->version!=VERSION )
		{
			m_rDriverContext.getLogManager() << LogLevel_Error << "Error while asking for data\n";
			if ( l_pResponse )
			{
				if (l_pResponse->buf) free(l_pResponse->buf);
				if (l_pResponse->def) free(l_pResponse->def);
				free(l_pResponse);
				l_pResponse = NULL;
			}
			return -1;
		}
		else if ( l_pResponse->def->command != GET_OK || l_pResponse->def->bufsize==0 || l_pResponse->buf==NULL )
		{
			m_rDriverContext.getLogManager() << LogLevel_Error << "Data are not available anymore\n";
			if ( l_pResponse )
			{
				if (l_pResponse->buf) free(l_pResponse->buf);
				if (l_pResponse->def) free(l_pResponse->def);
				free(l_pResponse);
				l_pResponse = NULL;
			}
			return -1;
		}
		else // data received
		{
			datadef_t* l_pDatadef = (datadef_t*) l_pResponse->buf;
			void* l_pDatabuf = (datadef_t*) l_pResponse->buf + 1;
			if ( l_pDatadef->bufsize / (wordsize_from_type(l_pDatadef->data_type)*l_pDatadef->nchans) != l_ui32LastSample - m_ui32TotalSampleCount )
			{
				m_rDriverContext.getLogManager() << LogLevel_Error << "Data received from buffer are invalid\n";
				if ( l_pResponse )
				{
					if (l_pResponse->buf) free(l_pResponse->buf);
					if (l_pResponse->def) free(l_pResponse->def);
					free(l_pResponse);
					l_pResponse = NULL;
				}
				return -1;
			}
			else //data correct
			{
				l_ui32NbDataReceived = l_ui32LastSample - m_ui32TotalSampleCount;

				// Delete some samples if necessary.
				// Sampling rate is converted into integer in openvibe,
				// so we can have up to 1 sample too many per second.
				l_ui32NbDataToSend = l_ui32NbDataReceived;

				if (m_bCorrectNonIntegerSR)
				{
					m_f64DriftSinceLastCorrection += ( m_f64DiffPerSample * l_ui32NbDataReceived );
					if ( m_f64DriftSinceLastCorrection >= 1.0 )
					{
						// delete samples
						uint32 l_ui32DiffSamples = (uint32) m_f64DriftSinceLastCorrection;
						l_ui32NbDataToSend -= l_ui32DiffSamples;
						m_rDriverContext.getLogManager() << LogLevel_Trace
														 << "Correction for non-integer sampling rate : "
														 << l_ui32DiffSamples << " samples deleted\n";
						m_f64DriftSinceLastCorrection -= (float64) l_ui32DiffSamples;
						/*oStimulationSet.appendStimulation(
							OVTK_GDF_Missing,
							((uint64)l_ui32NbDataToSend << 32)/m_oHeader.getSamplingFrequency(),
							((uint64)l_ui32DiffSamples   << 32)/m_oHeader.getSamplingFrequency());*/
					}
				}

				// set data in m_pSample
				float64* l_pBuffer64;
				float32* l_pBuffer32;
				switch(m_ui32DataType)
				{
				case DATATYPE_FLOAT64 :
					l_pBuffer64 = (float64*) l_pDatabuf;
					for ( uint32 j = 0; j < m_oHeader.getChannelCount(); j++ )
					{
						for ( uint32 i = 0; i < l_ui32NbDataToSend; i++ )
						{
							float64 l_f64Value = l_pBuffer64[i*m_oHeader.getChannelCount() + j];
							/*if ( _isnan(l_f64Value) || !_finite(l_f64Value) || l_f64Value==DBL_MAX )
							{
								m_pSample[j*l_ui32NbDataToSend + i] = FLT_MAX;
								m_rDriverContext.getLogManager() << LogLevel_Trace << "NaN or infinite sample received.\n";
							}
							else
							{
								//data from IHM implant are in volts, must be in µvolts in openvibe
								m_pSample[j*l_ui32NbDataToSend + i] = (float32) 1000000.0f*l_f64Value;
							}*/
							m_pSample[j*l_ui32NbDataToSend + i] = (float32) 1000000.0f*l_f64Value;
						}
					}
					break;
				case DATATYPE_FLOAT32 :
					l_pBuffer32 = (float32*) l_pDatabuf;
					for ( uint32 j = 0; j < m_oHeader.getChannelCount(); j++ )
					{
						for ( uint32 i = 0; i < l_ui32NbDataToSend; i++ )
						{
							float32 l_f32Value = l_pBuffer32[i*m_oHeader.getChannelCount() + j];
							/*if ( _isnan(l_f32Value) || !_finite(l_f32Value) || l_f32Value==FLT_MAX )
							{
								m_pSample[j*l_ui32NbDataToSend + i] = FLT_MAX;
								m_rDriverContext.getLogManager() << LogLevel_Trace << "NaN or infinite sample received.\n";
							}
							else
							{
								//data from IHM implant are in volts, must be in µvolts in openvibe
								m_pSample[j*l_ui32NbDataToSend + i] = 1000000.0f*l_f32Value;
							}*/
							m_pSample[j*l_ui32NbDataToSend + i] = 1000000.0f*l_f32Value;
						}
					}
					break;
				default :
					m_rDriverContext.getLogManager() << LogLevel_Error << "DEV ERROR : data type not suppported\n";
					if ( l_pResponse )
					{
						if (l_pResponse->buf) free(l_pResponse->buf);
						if (l_pResponse->def) free(l_pResponse->def);
						free(l_pResponse);
						l_pResponse = NULL;
					}
					return -1;
				}//end switch

			}//end data correct

		}//end data received
		m_ui32TotalSampleCount = l_ui32LastSample;
	}

	return l_ui32NbDataToSend; // no error
}
