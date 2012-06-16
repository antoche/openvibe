#if defined TARGET_HAS_ThirdPartyMatlab

#include "ovpCBoxAlgorithmMatlabFilter.h"


#include <system/Memory.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <mex.h>
#include <engine.h>
#include <string>

#if defined OVP_OS_Windows
	#include <windows.h>
#endif

#define MATLAB_BUFFER 2048
#define m_pMatlabEngine ((Engine*)m_pMatlabEngineHandle)
#define m_pMatlabStimulation ((mxArray*)m_pMatlabStimulationHandle)
#define m_pMatlabMatrix ((mxArray*)m_pMatlabMatrixHandle)
#define m_pMatlabBCIContext ((mxArray*)m_pMatlabBCIContextHandle)

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Tools;

#define boolean OpenViBE::boolean

boolean CBoxAlgorithmMatlabFilter::OpenMatlabEngineSafely(void)
{
	this->getLogManager() << LogLevel_Trace << "Trying to open Matlab engine\n";
#if defined OVP_OS_Linux
	m_pMatlabEngineHandle=::engOpen(m_sMatlabPath.toASCIIString());
#elif defined OVP_OS_Windows
	__try
	{
		m_pMatlabEngineHandle=::engOpen(NULL);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		this->getLogManager() << LogLevel_Error << "First call to MATLAB engine failed.\n"
			<< "\tTo use this box you must have MATLAB (32 bits version) installed on your computer.\n";

		return false;
	}
#else
#endif
	if(!m_pMatlabEngine)
	{
		this->getLogManager() << LogLevel_Error << "Could not open Matlab engine\n";
		return false;
	}
	return true;
}

boolean CBoxAlgorithmMatlabFilter::initialize(void)
{
	
	m_pStimulationDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pStimulationEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pStreamedMatrixDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));
	m_pStreamedMatrixEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));

	m_pStimulationDecoder->initialize();
	m_pStimulationEncoder->initialize();
 	m_pStreamedMatrixDecoder->initialize();
	m_pStreamedMatrixEncoder->initialize();

	op_pStimulationSet.initialize(m_pStimulationDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));
	ip_pStimulationSet.initialize(m_pStimulationEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
	op_pMatrix.initialize(m_pStreamedMatrixDecoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));
	ip_pMatrix.initialize(m_pStreamedMatrixEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix));

	m_ui64LatestStimulationChunkEndTime=0;
	uint32 l_ui32FirstCommonIndex = 0;

	getStaticBoxContext().getSettingValue(0, m_sMatlabPath);

#if defined OVP_OS_Windows
	char * l_sPath = getenv("PATH");
	if(l_sPath == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Could not access the environment variable PATH to add Matlab.\n";
		return false;
	}
	CString l_sCommandForPathModification = l_sPath + m_sMatlabPath+ ";";
	if(_putenv_s("PATH",l_sCommandForPathModification) != 0)
	{
		this->getLogManager() << LogLevel_Error << "Failed to modify the environment PATH with the Matlab path.\n";
		return false;
	}
#endif

	if(!OpenMatlabEngineSafely()) return false;
	
	CString l_sSettingValue;
	getStaticBoxContext().getSettingValue(1, l_sSettingValue); // working directory
	l_sSettingValue=CString("cd ")+l_sSettingValue;
	::engEvalString(m_pMatlabEngine, l_sSettingValue.toASCIIString());

	getStaticBoxContext().getSettingValue(2,m_sInitializeFunction);
	getStaticBoxContext().getSettingValue(3,m_sProcessFunction);

	m_pMatlabStimulationHandle=NULL;
	m_pMatlabMatrixHandle=NULL;
	m_pMatlabBCIContextHandle=::mxCreateDoubleMatrix(2,1,mxREAL);


	return true;
}

boolean CBoxAlgorithmMatlabFilter::CloseMatlabEngineSafely(void)
{
	if(m_pMatlabEngine == NULL)
	{
		return true;
	}
	this->getLogManager() << LogLevel_Trace << "Trying to close Matlab engine\n";
#if defined OVP_OS_Windows
	__try
	{
#endif
		if(m_pMatlabMatrix)
		{
			::mxDestroyArray(m_pMatlabMatrix);
		}

		::mxDestroyArray(m_pMatlabBCIContext);

		if(m_pMatlabEngine)
		{
			if(::engClose(m_pMatlabEngine)!=0)
			{
				this->getLogManager() << LogLevel_ImportantWarning << "Could not close Matlab engine.\n";
			}
		}
#if defined OVP_OS_Windows
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		this->getLogManager() << LogLevel_Error << "Closing MATLAB engine failed.\n"
			<< "\tTo use this box you must have MATLAB (32 bits version) installed on your computer.\n";

		return false;
	}
#endif

	return true;
}

boolean CBoxAlgorithmMatlabFilter::uninitialize(void)
{
	CloseMatlabEngineSafely();

	ip_pMatrix.uninitialize();
	op_pMatrix.uninitialize();
	ip_pStimulationSet.uninitialize();
	op_pStimulationSet.uninitialize();

	m_pStreamedMatrixEncoder->uninitialize();
	m_pStreamedMatrixDecoder->uninitialize();
	m_pStimulationEncoder->uninitialize();
	m_pStimulationDecoder->uninitialize();

	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamedMatrixEncoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamedMatrixDecoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_pStimulationEncoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_pStimulationDecoder);

	return true;
}

boolean CBoxAlgorithmMatlabFilter::processInput(uint32 ui32InputIndex)
{
	if(!m_pMatlabEngine)
	{
		return true;
	}

	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmMatlabFilter::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	uint32 i,j;
	
	TParameterHandler < IMemoryBuffer* > op_pMemoryBuffer(m_pStreamedMatrixEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	TParameterHandler < IMemoryBuffer* > op_pMemoryBufferStimulation(m_pStimulationEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	op_pMemoryBufferStimulation=l_rDynamicBoxContext.getOutputChunk(1);
	
	// We decode and save the received stimulations.
	for(i=0; i<l_rDynamicBoxContext.getInputChunkCount(1); i++)
	{
		TParameterHandler < const IMemoryBuffer* > ip_pMemoryBuffer(m_pStimulationDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
		ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(1, i);
		m_pStimulationDecoder->process();
		if(m_pStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
		}
		if(m_pStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			for(uint32 j=0; j<op_pStimulationSet->getStimulationCount(); j++)
			{
				m_oPendingStimulationSet.appendStimulation(
					op_pStimulationSet->getStimulationIdentifier(j),
					op_pStimulationSet->getStimulationDate(j),
					op_pStimulationSet->getStimulationDuration(j));
			}
		}
		if(m_pStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
		}
		m_ui64LatestStimulationChunkEndTime=l_rDynamicBoxContext.getInputChunkEndTime(1, i);
		l_rDynamicBoxContext.markInputAsDeprecated(1, i);
	}

	// We now decode the stream matrix
	for(i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		uint64 l_ui64StartTime=l_rDynamicBoxContext.getInputChunkStartTime(0, i);
		uint64 l_ui64EndTime=l_rDynamicBoxContext.getInputChunkEndTime(0, i);

		((float64*)::mxGetPr(m_pMatlabBCIContext))[0] = (l_ui64StartTime >> 16) / 65536.0;
		((float64*)::mxGetPr(m_pMatlabBCIContext))[1] = (l_ui64EndTime >> 16) / 65536.0;

		if(l_ui64EndTime<=m_ui64LatestStimulationChunkEndTime)
		{
			TParameterHandler < const IMemoryBuffer* > ip_pMemoryBuffer(m_pStreamedMatrixDecoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
			ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
			m_pStreamedMatrixDecoder->process();
			// we received the header
			if(m_pStreamedMatrixDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
				//we analyse the header
				if(op_pMatrix->getDimensionCount()!=2)
				{
					this->getLogManager() << LogLevel_ImportantWarning << "Streamed matrix dimension count is not 2\n";
				}
				else
				{
					m_pMatlabMatrixHandle=::mxCreateDoubleMatrix(
						op_pMatrix->getDimensionSize(1),
						op_pMatrix->getDimensionSize(0),
						mxREAL);
					if(!m_pMatlabMatrix)
					{
						this->getLogManager() << LogLevel_ImportantWarning << "Could not create Matlab matrix\n";
					}
					
					char l_pMatlabBuffer[MATLAB_BUFFER+1];
					l_pMatlabBuffer[MATLAB_BUFFER]='\0';
					std::stringstream l_sMatlabBuffer;
					::engOutputBuffer(m_pMatlabEngine, l_pMatlabBuffer,MATLAB_BUFFER);
					
					::engPutVariable(m_pMatlabEngine, "bci_context", m_pMatlabBCIContext);
					::engPutVariable(m_pMatlabEngine, "matrix_in", m_pMatlabMatrix);
					if(::engEvalString(m_pMatlabEngine, CString("matrix_out = ") + m_sInitializeFunction + CString("(bci_context, matrix_in);"))!=0)
					{
						l_sMatlabBuffer<<l_pMatlabBuffer;
						this->getLogManager()<< LogLevel_Error << "Matlab Error: "<<l_sMatlabBuffer.str().c_str()<<"\n";
						return false;
					}

					l_sMatlabBuffer<<l_pMatlabBuffer;

					// if the MatlabBuffer is not empty, we have to print it in the openvibe console
					// We have to check if it's error, warning, or simple info messages
					if(l_sMatlabBuffer.str().size()>0)
					{
						size_t l_oErrorIndex=l_sMatlabBuffer.str().find("??? ");
						size_t l_oWarningIndex=l_sMatlabBuffer.str().find("Warning: ");
						if(l_oWarningIndex > 0 && l_oErrorIndex > 0)
						{
							this->getLogManager()<<LogLevel_Info<<l_sMatlabBuffer.str().substr(0,(l_oWarningIndex<l_oErrorIndex)?l_oWarningIndex:l_oErrorIndex).c_str()<<"\n";
						
						}
						if(l_oWarningIndex!=std::string::npos)
						{
							this->getLogManager()<<LogLevel_Warning<<l_sMatlabBuffer.str().substr(l_oWarningIndex,l_oErrorIndex).c_str()<<"\n";
						}
						if(l_oErrorIndex!=std::string::npos)
						{
							this->getLogManager()<<LogLevel_Error<<l_sMatlabBuffer.str().substr(l_oErrorIndex).c_str()<<"\n";
						}
						
					}
					if(this->getLogManager().isActive(LogLevel_Debug))
					{
						::engOutputBuffer(m_pMatlabEngine, l_pMatlabBuffer, MATLAB_BUFFER);
						::engEvalString(m_pMatlabEngine, "result");
						l_sMatlabBuffer.clear();
						l_sMatlabBuffer<<l_pMatlabBuffer;
						this->getLogManager() << LogLevel_Debug << l_sMatlabBuffer.str().c_str()<<"\n";
					}
					
					// now we check the result of the initialization
					::mxArray* l_pMatlabMatrixOut=::engGetVariable(m_pMatlabEngine, "matrix_out");
					if(l_pMatlabMatrixOut)
					{
						if(!::mxIsDouble(l_pMatlabMatrixOut))
						{
							this->getLogManager() << LogLevel_Warning << "Expected double matrix output from " << m_sInitializeFunction << "\n";
						}
						else
						{
							// we send a header on the stream matrix output:
							ip_pMatrix->setDimensionCount(2);
							ip_pMatrix->setDimensionSize(0, ::mxGetDimensions(l_pMatlabMatrixOut)[1]);
							ip_pMatrix->setDimensionSize(1, ::mxGetDimensions(l_pMatlabMatrixOut)[0]);

							op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);
							m_pStreamedMatrixEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
							l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_ui64StartTime, l_ui64EndTime);
						}
						::mxDestroyArray(l_pMatlabMatrixOut);
					}
				}

				// we send a header on the stimulation output:
				m_pStimulationEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader);
				l_rDynamicBoxContext.markOutputAsReadyToSend(1, l_ui64StartTime,l_ui64EndTime );
			}
			// we received a buffer
			if(m_pStreamedMatrixDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				// we copy the actual pending stimulation in l_oStimulationSet
				CStimulationSet l_oStimulationSet;
				for(j=0; j<m_oPendingStimulationSet.getStimulationCount(); )
				{
					if(m_oPendingStimulationSet.getStimulationDate(j)<l_ui64StartTime)
					{
						m_oPendingStimulationSet.removeStimulation(j);
					}
					else
					{
						if(m_oPendingStimulationSet.getStimulationDate(j)<l_ui64EndTime)
						{
							l_oStimulationSet.appendStimulation(
								m_oPendingStimulationSet.getStimulationIdentifier(j),
								m_oPendingStimulationSet.getStimulationDate(j),
								m_oPendingStimulationSet.getStimulationDuration(j));
						}
						j++;
					}
				}

				// we copy the stimulation into the corresponding matlab matrix
				m_pMatlabStimulationHandle=::mxCreateDoubleMatrix(l_oStimulationSet.getStimulationCount(), 3, mxREAL);
				float64* l_pMatlabStimulationBuffer=(float64*)::mxGetPr(m_pMatlabStimulation);
				uint32 l_ui32StimulationCount=l_oStimulationSet.getStimulationCount();
				for(j=0; j<l_ui32StimulationCount; j++)
				{
					l_pMatlabStimulationBuffer[j+0*l_ui32StimulationCount]=l_oStimulationSet.getStimulationIdentifier(j);
					l_pMatlabStimulationBuffer[j+1*l_ui32StimulationCount]=((l_oStimulationSet.getStimulationDate(j)-l_ui64StartTime) >> 16) / 65536.0;
					l_pMatlabStimulationBuffer[j+2*l_ui32StimulationCount]=( l_oStimulationSet.getStimulationDuration(j) >> 16) / 65536.0;
				}

				// we copy the data
				System::Memory::copy(::mxGetPr(m_pMatlabMatrix), op_pMatrix->getBuffer(), op_pMatrix->getBufferElementCount()*sizeof(float64));

				// the call to the matlab engine
				::engPutVariable(m_pMatlabEngine, "bci_context", m_pMatlabBCIContext);
				::engPutVariable(m_pMatlabEngine, "stimulation_in", m_pMatlabStimulation);
				::engPutVariable(m_pMatlabEngine, "matrix_in", m_pMatlabMatrix);
				
				// the buffer for the console
				char l_pMatlabBuffer[MATLAB_BUFFER+1];
				l_pMatlabBuffer[MATLAB_BUFFER]='\0';
				std::stringstream l_sMatlabBuffer;
				::engOutputBuffer(m_pMatlabEngine, l_pMatlabBuffer,MATLAB_BUFFER);

				if(::engEvalString(m_pMatlabEngine, CString("[matrix_out,stimulation_out] = ") + m_sProcessFunction + CString("(bci_context, matrix_in, stimulation_in);"))!=0)
				{
					l_sMatlabBuffer<<l_pMatlabBuffer;
					this->getLogManager()<< LogLevel_Error << "Matlab Error: "<<l_sMatlabBuffer.str().c_str()<<"\n";
					return false;
				}
				l_sMatlabBuffer<<l_pMatlabBuffer;
				if(l_sMatlabBuffer.str().size()>0)
				{
					size_t l_oErrorIndex=l_sMatlabBuffer.str().find("??? ");
					size_t l_oWarningIndex=l_sMatlabBuffer.str().find("Warning: ");
					if(l_oErrorIndex!=0 && l_oWarningIndex!=0)
					{
						this->getLogManager()<<LogLevel_Info<<l_sMatlabBuffer.str().substr(0,(l_oWarningIndex<l_oErrorIndex)?l_oWarningIndex:l_oErrorIndex).c_str()<<"\n";
					}
					if(l_oWarningIndex!=std::string::npos)
					{
						this->getLogManager()<<LogLevel_Warning<<l_sMatlabBuffer.str().substr(l_oWarningIndex,l_oErrorIndex).c_str()<<"\n";
					}
					if(l_oErrorIndex!=std::string::npos)
					{
						this->getLogManager()<<LogLevel_Error<<l_sMatlabBuffer.str().substr(l_oErrorIndex).c_str()<<"\n";
					}
				}
				if(this->getLogManager().isActive(LogLevel_Debug))
				{
					::engOutputBuffer(m_pMatlabEngine, l_pMatlabBuffer, MATLAB_BUFFER);
					::engEvalString(m_pMatlabEngine, "result");
					l_sMatlabBuffer.clear();
					l_sMatlabBuffer<<l_pMatlabBuffer;
					this->getLogManager() << LogLevel_Debug << l_sMatlabBuffer.str().c_str()<<"\n";
				}
				::mxDestroyArray(m_pMatlabStimulation);

				// reading matrix_out from matlab
				::mxArray* l_pMatlabMatrixOut=::engGetVariable(m_pMatlabEngine, "matrix_out");
				if(l_pMatlabMatrixOut)
				{
					if(!::mxIsDouble(l_pMatlabMatrixOut))
					{
						this->getLogManager() << LogLevel_Warning << "Expected double matrix output from " << m_sProcessFunction << "\n";
					}
					else
					{
						System::Memory::copy(ip_pMatrix->getBuffer(), ::mxGetPr(l_pMatlabMatrixOut), ip_pMatrix->getBufferElementCount()*sizeof(float64));
 
						op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);
						m_pStreamedMatrixEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
						l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_ui64StartTime, l_ui64EndTime);
					}
					::mxDestroyArray(l_pMatlabMatrixOut);
				}

				// reading stimulation_out from matlab
				::mxArray* l_pMatlabStimulationOut=::engGetVariable(m_pMatlabEngine, "stimulation_out");
				if(l_pMatlabStimulationOut)
				{
					if(!::mxIsDouble(l_pMatlabStimulationOut))
					{
						this->getLogManager() << LogLevel_Warning << "Expected double matrix output from " << m_sProcessFunction << "\n";
					}
					else
					{
						if (::mxGetDimensions(l_pMatlabStimulationOut)[1] != 3)
						{
							this->getLogManager() << LogLevel_Warning << "Output Stimulation matrix must be of size (nb_stim x 3) after " << m_sProcessFunction << "\n";
						}
						else
						{
							ip_pStimulationSet->setStimulationCount(0);
							uint32 l_ui32StimulationCount=::mxGetDimensions(l_pMatlabStimulationOut)[0];
							float64* l_pStim=::mxGetPr(l_pMatlabMatrixOut);
							for(j=0; j<l_ui32StimulationCount; j++)
							{
								// we decode a stimulation from the matlab stimulation matrix
								ip_pStimulationSet->appendStimulation(
									l_pStim[j+0*l_ui32StimulationCount],
									l_ui64StartTime + ((int64)(l_pStim[j+1*l_ui32StimulationCount] * 65536.0)<<16),
									l_ui64StartTime + ((int64)(l_pStim[j+2*l_ui32StimulationCount] * 65536.0)<<16));
							}
							m_pStimulationEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
							l_rDynamicBoxContext.markOutputAsReadyToSend(1, l_ui64StartTime, l_ui64EndTime);
						}
					}
				}


			}
			// We received the End
			if(m_pStreamedMatrixDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
				// we send the End signal to the stream matrix output
				m_pStreamedMatrixEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
				// and to the stimulation output:
				m_pStimulationEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeEnd);
			}
			// The stream matrix chunk i has been process 
			l_rDynamicBoxContext.markInputAsDeprecated(0, i);
		}
	}

	return true;
}

#endif // TARGET_HAS_ThirdPartyMatlab
