#include "ovpCBoxAlgorithmXDAWNSpatialFilterTrainer.h"

#include <system/Memory.h>

#include <complex>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <map>

#include <itpp/base/algebra/eigen.h>
#include <itpp/base/algebra/inv.h>
#include <itpp/base/converters.h>
#include <itpp/stat/misc_stat.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessingGpl;

using namespace itpp;

// Taken from http://techlogbook.wordpress.com/2009/08/12/adding-generalized-eigenvalue-functions-to-it
//            http://techlogbook.wordpress.com/2009/08/12/calling-lapack-functions-from-c-codes
//            http://sourceforge.net/projects/itpp/forums/forum/115656/topic/3363490?message=7557038
//
// http://icl.cs.utk.edu/projectsfiles/f2j/javadoc/org/netlib/lapack/DSYGV.html
// http://www.lassp.cornell.edu/sethna/GeneDynamics/NetworkCodeDocumentation/lapack_8h.html#a17

namespace
{
	extern "C"
	{
		// This symbol comes from LAPACK
/*
		void zggev_(char *jobvl, char *jobvr, int *n, std::complex<double> *a,
			int *lda, std::complex<double> *b, int *ldb, std::complex<double> *alpha,
			std::complex<double> *beta, std::complex<double> *vl,
			int *ldvl, std::complex<double> *vr, int *ldvr,
			std::complex<double> *work, int *lwork, double *rwork, int *info);
*/
		int dsygv_(int* itype, char* jobz, char* uplo, int* n, double* a,
			int* lda, double* b, int* ldb, double* w, double* work, int* lwork, int* info);
	}
};

namespace itppext
{
	bool eig(const itpp::mat &A, const itpp::mat &B, itpp::vec &d, itpp::mat &V)
	{
		it_assert_debug(A.rows() == A.cols(), "eig: Matrix A is not square");
		it_assert_debug(B.rows() == B.cols(), "eig: Matrix B is not square");
		it_assert_debug(A.rows() == B.cols(), "eig: Matrix A and B don't have the same size");

		int worksize=4*A.rows(); // This may be choosen better!
		itpp::mat l_A(A);
		itpp::mat l_B(B);
		itpp::vec l_W(A.rows());
		itpp::vec l_Work(worksize);
		l_W.zeros();
		l_Work.zeros();

		int itype=1; // 1: Ax=lBx 2: ABx=lx 3: BAx=lx
		char jobz = 'V', uplo = 'U';
		int n=l_A.rows();
		double* a=l_A._data();
		int lda=n;
		double* b=l_B._data();
		int ldb=n;
		double* w=l_W._data();
		int lwork=worksize;
		double* work=l_Work._data();
		int info=0;

		dsygv_(&itype, &jobz, &uplo, &n, a, &lda, b, &ldb, w, work, &lwork, &info);

		d=l_W;
		V=l_A;

		return (info == 0);
	}

	itpp::mat convert(const IMatrix& rMatrix)
	{
		itpp::mat l_oResult(
			rMatrix.getDimensionSize(1),
			rMatrix.getDimensionSize(0));
		System::Memory::copy(l_oResult._data(), rMatrix.getBuffer(), rMatrix.getBufferElementCount()*sizeof(float64));
		return l_oResult.transpose();
	}
}

boolean CBoxAlgorithmXDAWNSpatialFilterTrainer::initialize(void)
{
	// CString   l_sSettingValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	// uint64 l_ui64SettingValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	// float64 l_f64SettingValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	// ...

	m_pStimulationDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pStimulationDecoder->initialize();

	m_pSignalDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
	m_pSignalDecoder->initialize();

	m_pEvokedPotentialDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
	m_pEvokedPotentialDecoder->initialize();

	m_oStimulationEncoder.initialize(*this);

	m_ui64StimulationIdentifier=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_sSpatialFilterConfigurationFilename=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_ui64FilterDimension=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);

	return true;
}

boolean CBoxAlgorithmXDAWNSpatialFilterTrainer::uninitialize(void)
{
	m_pEvokedPotentialDecoder->uninitialize();
	m_pSignalDecoder->uninitialize();
	m_pStimulationDecoder->uninitialize();

	this->getAlgorithmManager().releaseAlgorithm(*m_pEvokedPotentialDecoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_pSignalDecoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_pStimulationDecoder);

	m_oStimulationEncoder.uninitialize();

	m_pEvokedPotentialDecoder=NULL;
	m_pSignalDecoder=NULL;
	m_pStimulationDecoder=NULL;

	return true;
}

boolean CBoxAlgorithmXDAWNSpatialFilterTrainer::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

namespace
{
	typedef struct
	{
		uint64 m_ui64StartTime;
		uint64 m_ui64EndTime;
		CMatrix* m_pMatrix;
	} SChunk;
};

boolean CBoxAlgorithmXDAWNSpatialFilterTrainer::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	boolean l_bShouldTrain=false;
	uint64 l_ui64TrainDate, l_ui64TrainChunkStartTime, l_ui64TrainChunkEndTime;
	uint32 i, j, k;
	std::vector < SChunk >::const_iterator it;

	for(i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		TParameterHandler < const IMemoryBuffer* > ip_pMemoryBuffer(m_pStimulationDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
		ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		m_pStimulationDecoder->process();
		if(m_pStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_oStimulationEncoder.encodeHeader(0);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0,l_rDynamicBoxContext.getInputChunkStartTime(0, i),l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		}
		if(m_pStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			TParameterHandler < IStimulationSet* > op_pStimulationSet(m_pStimulationDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));
			for(j=0; j<op_pStimulationSet->getStimulationCount(); j++)
			{
				l_bShouldTrain |= (op_pStimulationSet->getStimulationIdentifier(j)==m_ui64StimulationIdentifier);
			}
			if(l_bShouldTrain)
			{
				l_ui64TrainDate = op_pStimulationSet->getStimulationDate(j);
				l_ui64TrainChunkStartTime = l_rDynamicBoxContext.getInputChunkStartTime(0, i);
				l_ui64TrainChunkEndTime = l_rDynamicBoxContext.getInputChunkEndTime(0, i);
			}
		}
		if(m_pStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_oStimulationEncoder.encodeEnd(0);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0,l_rDynamicBoxContext.getInputChunkStartTime(0, i),l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		}
		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	if(l_bShouldTrain)
	{
		this->getLogManager() << LogLevel_Info << "Received train stimulation - be patient\n";

		this->getLogManager() << LogLevel_Trace << "Decoding signal...\n";

		boolean l_bIsContinuous=true;
		uint64 l_ui64StartTime=uint64(-1);
		uint64 l_ui64EndTime=uint64(-1);
		std::vector < SChunk > l_vSignalChunk;
		for(i=0; i<l_rDynamicBoxContext.getInputChunkCount(1); i++)
		{
			TParameterHandler<const IMemoryBuffer*> ip_pMemoryBuffer(m_pSignalDecoder->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));
			ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(1, i);
			m_pSignalDecoder->process();
			if(m_pSignalDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
				for(it=l_vSignalChunk.begin(); it!=l_vSignalChunk.end(); it++) delete it->m_pMatrix;
				l_vSignalChunk.clear();

				l_bIsContinuous=true;
				l_ui64StartTime=l_rDynamicBoxContext.getInputChunkStartTime(1, i);
				l_ui64EndTime=l_rDynamicBoxContext.getInputChunkEndTime(1, i);
			}
			if(m_pSignalDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				TParameterHandler<IMatrix*> ip_pMatrix(m_pSignalDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_Matrix));
				SChunk l_oChunk;
				l_oChunk.m_ui64StartTime=l_rDynamicBoxContext.getInputChunkStartTime(1, i);
				l_oChunk.m_ui64EndTime=l_rDynamicBoxContext.getInputChunkEndTime(1, i);
				l_oChunk.m_pMatrix=new CMatrix;
				OpenViBEToolkit::Tools::Matrix::copy(
					*l_oChunk.m_pMatrix,
					*ip_pMatrix);
				l_vSignalChunk.push_back(l_oChunk);

				if(l_oChunk.m_ui64StartTime!=l_ui64EndTime)
				{
					l_bIsContinuous=false;
				}
				l_ui64StartTime=l_oChunk.m_ui64StartTime;
				l_ui64EndTime=l_oChunk.m_ui64EndTime;
			}
			if(m_pSignalDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
			}
			l_rDynamicBoxContext.markInputAsDeprecated(1, i);
		}

		this->getLogManager() << LogLevel_Trace << "Decoding evoked response potential...\n";

		std::vector < SChunk > l_vEvokedPotential;
		for(i=0; i<l_rDynamicBoxContext.getInputChunkCount(2); i++)
		{
			TParameterHandler<const IMemoryBuffer*> ip_pMemoryBuffer(m_pEvokedPotentialDecoder->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));
			ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(2, i);
			m_pEvokedPotentialDecoder->process();
			if(m_pSignalDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
				for(it=l_vEvokedPotential.begin(); it!=l_vEvokedPotential.end(); it++) delete it->m_pMatrix;
				l_vEvokedPotential.clear();
			}
			if(m_pEvokedPotentialDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				TParameterHandler<IMatrix*> ip_pMatrix(m_pEvokedPotentialDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_Matrix));
				SChunk l_oChunk;
				l_oChunk.m_ui64StartTime=l_rDynamicBoxContext.getInputChunkStartTime(2, i);
				l_oChunk.m_ui64EndTime=l_rDynamicBoxContext.getInputChunkEndTime(2, i);
				l_oChunk.m_pMatrix=new CMatrix;
				OpenViBEToolkit::Tools::Matrix::copy(
					*l_oChunk.m_pMatrix,
					*ip_pMatrix);
				l_vEvokedPotential.push_back(l_oChunk);
			}
			if(m_pEvokedPotentialDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
			}
			l_rDynamicBoxContext.markInputAsDeprecated(2, i);
		}

		if(!l_bIsContinuous)
		{
			this->getLogManager() << LogLevel_Error << "Input signal is not continuous... Can't continue\n";
			return true;
		}

		if(l_vEvokedPotential.size()==0)
		{
			this->getLogManager() << LogLevel_Error << "No evoked potential received... Can't continue\n";
			return true;
		}

		this->getLogManager() << LogLevel_Trace << "Averaging evoked response potential...\n";

		it=l_vEvokedPotential.begin();
		CMatrix l_oAveragedERPMatrixOV;
		OpenViBEToolkit::Tools::Matrix::copy(
			l_oAveragedERPMatrixOV,
			*it->m_pMatrix);
		for(it++; it!=l_vEvokedPotential.end(); it++)
		{
			const float64* l_pEvokedPotentialBuffer=it->m_pMatrix->getBuffer();
			float64* l_pAveragedBuffer=l_oAveragedERPMatrixOV.getBuffer();
			for(j=0; j<l_oAveragedERPMatrixOV.getBufferElementCount(); j++)
			{
				*(l_pAveragedBuffer++)+=*(l_pEvokedPotentialBuffer++);
			}
		}
		float64* l_pAveragedBuffer=l_oAveragedERPMatrixOV.getBuffer();
		for(j=0; j<l_oAveragedERPMatrixOV.getBufferElementCount(); j++)
		{
			(*l_pAveragedBuffer++)/=l_vEvokedPotential.size();
		}

		// WARNING - OpenViBE matrices are transposed ITPP matrices !

		this->getLogManager() << LogLevel_Trace << "Converting OpenViBE matrices to IT++ matrices...\n";

		uint32 l_ui32ChunkCount=l_vSignalChunk.size();
		uint32 l_ui32ChannelCount=l_vSignalChunk.begin()->m_pMatrix->getDimensionSize(0);
		uint32 l_ui32SampleCountPerChunk=l_vSignalChunk.begin()->m_pMatrix->getDimensionSize(1);
		uint32 l_ui32SampleCountPerERP=l_oAveragedERPMatrixOV.getDimensionSize(1);
		uint64 l_ui64SignalStartTime=l_vSignalChunk.begin()->m_ui64StartTime;
		uint64 l_ui64SignalEndTime=l_vSignalChunk.rbegin()->m_ui64EndTime;

		itpp::mat l_oSignalMatrix(l_ui32ChannelCount, l_ui32ChunkCount*l_ui32SampleCountPerChunk);
		for(i=0, it=l_vSignalChunk.begin(); it!=l_vSignalChunk.end(); it++, i++)
		{
			itpp::mat l_oMatrix=itppext::convert(*it->m_pMatrix);
			l_oSignalMatrix.set_submatrix(0, i*l_ui32SampleCountPerChunk, l_oMatrix);
		}

		itpp::mat l_oAveragedERPMatrix(l_ui32ChannelCount, l_ui32SampleCountPerERP);
		l_oAveragedERPMatrix=itppext::convert(l_oAveragedERPMatrixOV);

		itpp::mat l_oDMatrix(l_ui32ChunkCount*l_ui32SampleCountPerChunk, l_ui32SampleCountPerERP);
		l_oDMatrix.clear();
		for(it=l_vEvokedPotential.begin(); it!=l_vEvokedPotential.end(); it++)
		{
			uint64 l_ui64ERPStartTime=it->m_ui64StartTime;
			uint32 l_ui32ERPStartIndex=((l_ui64ERPStartTime-l_ui64SignalStartTime)*(l_ui32SampleCountPerChunk*l_ui32ChunkCount))/(l_ui64SignalEndTime-l_ui64SignalStartTime);
			for(k=0; k<l_ui32SampleCountPerERP; k++)
			{
				l_oDMatrix(l_ui32ERPStartIndex+k,k)=1;
			}
		}

#if 0
		std::stringstream s1;
		std::stringstream s2;
		std::stringstream s3;

		s1 << "Averaged ERP :\n" << l_oAveragedERPMatrix << "\n";
		s2 << "Signal Matrix :\n" << l_oSignalMatrix << "\n";
		s3 << "D Matrix :\n" << l_oDMatrix.transpose() << "\n";

		this->getLogManager() << LogLevel_Fatal << s1.str().c_str() << "\n";
		this->getLogManager() << LogLevel_Fatal << s2.str().c_str() << "\n";
		this->getLogManager() << LogLevel_Fatal << s3.str().c_str() << "\n";

		this->getLogManager() << LogLevel_Info << l_oAveragedERPMatrix.rows() << "x" << l_oAveragedERPMatrix.cols() << "\n";
		this->getLogManager() << LogLevel_Info << l_oDMatrix.rows() << "x" << l_oDMatrix.cols() << "\n";
		this->getLogManager() << LogLevel_Info << l_oSignalMatrix.rows() << "x" << l_oSignalMatrix.cols() << "\n";
#endif

		itpp::mat A=(l_oAveragedERPMatrix*itpp::inv(l_oDMatrix.transpose()*l_oDMatrix)*l_oAveragedERPMatrix.transpose()) * double(l_vEvokedPotential.size()) / double(l_ui32SampleCountPerChunk*l_ui32ChunkCount);
#if 1
		std::stringstream s4;
		s4 << "A :\n" << A << "\n";
		this->getLogManager() << LogLevel_Debug << s4.str().c_str() << "\n";
#endif

		itpp::mat B=(l_oSignalMatrix*l_oSignalMatrix.transpose()) / double(l_ui32SampleCountPerChunk*l_ui32ChunkCount);
#if 1
		std::stringstream s5;
		s5 << "B :\n" << B << "\n";
		this->getLogManager() << LogLevel_Debug << s5.str().c_str() << "\n";
#endif

		this->getLogManager() << LogLevel_Trace << "Computing generalized eigen vector decomposition...\n";

		itpp::mat l_oEigenVector;
		itpp::vec l_oEigenValue;

		if(itppext::eig(A, B, l_oEigenValue, l_oEigenVector))
		{
			std::map < double, itpp::vec > l_vEigenVector;
			for(i=0; i<l_ui32ChannelCount; i++)
			{
				itpp::vec l_oVector=l_oEigenVector.get_col(i);
				l_vEigenVector[l_oEigenValue[i]]=l_oVector/itpp::norm(l_oVector);
			}

#if 0
			std::stringstream s6;
			std::stringstream s7;

			s6 <<  "l_oEigenValues :\n" << l_oEigenValues << "\n";
			s7 <<  "l_oEigenVectors :\n" << l_oEigenVectors << "\n";

			this->getLogManager() << LogLevel_Fatal << s6.str().c_str() << "\n";
			this->getLogManager() << LogLevel_Fatal << s7.str().c_str() << "\n";
#endif

			std::map < double, itpp::vec >::const_reverse_iterator it;
			FILE* l_pFile=::fopen(m_sSpatialFilterConfigurationFilename.toASCIIString(), "wb");
			if(!l_pFile)
			{
				this->getLogManager() << LogLevel_Error << "The file [" << m_sSpatialFilterConfigurationFilename << "] could not be opened for writing...";
				return false;
			}

			::fprintf(l_pFile, "<OpenViBE-SettingsOverride>\n");
			::fprintf(l_pFile, "\t<SettingValue>");
			for(it=l_vEigenVector.rbegin(), i=0; it!=l_vEigenVector.rend() && i<m_ui64FilterDimension; it++, i++)
			{
				for(j=0; j<l_ui32ChannelCount; j++)
				{
					::fprintf(l_pFile, "%e ", it->second[j]);
				}
			}
			::fprintf(l_pFile, "</SettingValue>\n");
			::fprintf(l_pFile, "\t<SettingValue>%d</SettingValue>\n", uint32(m_ui64FilterDimension));
			::fprintf(l_pFile, "\t<SettingValue>%d</SettingValue>\n", l_ui32ChannelCount);
			::fprintf(l_pFile, "</OpenViBE-SettingsOverride>\n");
			::fclose(l_pFile);

			this->getLogManager() << LogLevel_Info << "Training finished... Eigen values are ";
			for(it=l_vEigenVector.rbegin(), i=0; it!=l_vEigenVector.rend() && i<m_ui64FilterDimension; it++, i++)
			{
				this->getLogManager() << " | " << float64(it->first);
			}
			this->getLogManager() << "\n";
		}
		else
		{
			this->getLogManager() << LogLevel_ImportantWarning << "Generalized eigen vector decomposition failed...\n";
			return true;
		}

		this->getLogManager() << LogLevel_Info << "xDAWN Spatial filter trained successfully.\n";
		m_oStimulationEncoder.getInputStimulationSet()->appendStimulation(OVTK_StimulationId_TrainCompleted, l_ui64TrainDate, 0);
		m_oStimulationEncoder.encodeBuffer(0);
		l_rDynamicBoxContext.markOutputAsReadyToSend(0,l_ui64TrainChunkStartTime, l_ui64TrainChunkEndTime);
	}

	return true;
}
