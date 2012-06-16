#ifndef __OpenViBEToolkit_TTrainingBoxAlgorithm_H__
#define __OpenViBEToolkit_TTrainingBoxAlgorithm_H__

#include "ovtkTBoxAlgorithm.h"

#include "../training/ovtkISignalTrial.h"
#include "../training/ovtkISignalTrialSet.h"

#include "../deprecated/reader/ovtkIBoxAlgorithmSignalInputReaderCallback.h"
#include "../deprecated/reader/ovtkIBoxAlgorithmStimulationInputReaderCallback.h"

#include <ebml/CReader.h>

#include <vector>

namespace OpenViBEToolkit
{

	template <class CBoxAlgorithmParentClass>
	class TTrainingBoxAlgorithm : public OpenViBEToolkit::TBoxAlgorithm<CBoxAlgorithmParentClass>
	{
	public:

		TTrainingBoxAlgorithm(void);
		virtual ~TTrainingBoxAlgorithm(void);

		// Signal input reader callback
		virtual void setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
		virtual void setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
		virtual void setSampleCountPerBuffer(const OpenViBE::uint32 ui32SampleCountPerBuffer);
		virtual void setSamplingRate(const OpenViBE::uint32 ui32SamplingFrequency);
		virtual void setSampleBuffer(const OpenViBE::float64* pBuffer);

		// Stimulation input reader callback
		virtual void setStimulationCount(const OpenViBE::uint32 ui32StimulationCount);
		virtual void setStimulation(const OpenViBE::uint32 ui32StimulationIndex, const OpenViBE::uint64 ui64StimulationIdentifier, const OpenViBE::uint64 ui64StimulationDate);

		// What should be implemented by the derived class
		virtual OpenViBE::CIdentifier getStimulationIdentifierTrialStart(void)=0;
		virtual OpenViBE::CIdentifier getStimulationIdentifierTrialEnd(void)=0;
		virtual OpenViBE::CIdentifier getStimulationIdentifierTrialLabelRangeStart(void)=0;
		virtual OpenViBE::CIdentifier getStimulationIdentifierTrialLabelRangeEnd(void)=0;
		virtual OpenViBE::CIdentifier getStimulationIdentifierTrain(void)=0;
		virtual OpenViBE::boolean train(OpenViBEToolkit::ISignalTrialSet& rTrialSet)=0;

		_IsDerivedFromClass_(OpenViBEToolkit::TBoxAlgorithm<CBoxAlgorithmParentClass>, OVTK_ClassId_);

	private:

		OpenViBEToolkit::ISignalTrial* m_pPendingSignal;

		OpenViBE::uint64 m_ui64TrialStartTime;
		OpenViBE::uint64 m_ui64TrialEndTime;
		OpenViBE::uint32 m_ui32SampleCountPerBuffer;
		OpenViBE::CIdentifier m_oTrialLabel;

		std::vector<OpenViBEToolkit::ISignalTrial*> m_vSignalTrial;
	};
};

// ________________________________________________________________________________________________________________
//

namespace OpenViBEToolkit {

// ________________________________________________________________________________________________________________
//

#define _no_time_ 0xffffffffffffffffLL

template <class CBoxAlgorithmParentClass>
TTrainingBoxAlgorithm<CBoxAlgorithmParentClass>::TTrainingBoxAlgorithm(void)
	:m_pPendingSignal(NULL)
	,m_ui64TrialStartTime(_no_time_)
	,m_ui64TrialEndTime(_no_time_)
	,m_ui32SampleCountPerBuffer(0)
{
	m_pPendingSignal=OpenViBEToolkit::createSignalTrial();
}

template <class CBoxAlgorithmParentClass>
TTrainingBoxAlgorithm<CBoxAlgorithmParentClass>::~TTrainingBoxAlgorithm(void)
{
	std::vector<OpenViBEToolkit::ISignalTrial*>::iterator itSignalTrial;
	OpenViBEToolkit::releaseSignalTrial(m_pPendingSignal);
	for(itSignalTrial=m_vSignalTrial.begin(); itSignalTrial!=m_vSignalTrial.end(); itSignalTrial++)
	{
		OpenViBEToolkit::releaseSignalTrial(*itSignalTrial);
	}
}

// ________________________________________________________________________________________________________________
//
// Signal input reader callback
// ________________________________________________________________________________________________________________
//

template <class CBoxAlgorithmParentClass>
void TTrainingBoxAlgorithm<CBoxAlgorithmParentClass>::setChannelCount(const OpenViBE::uint32 ui32ChannelCount)
{
	m_pPendingSignal->setChannelCount(ui32ChannelCount);
}

template <class CBoxAlgorithmParentClass>
void TTrainingBoxAlgorithm<CBoxAlgorithmParentClass>::setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName)
{
	m_pPendingSignal->setChannelName(ui32ChannelIndex, sChannelName);
}

template <class CBoxAlgorithmParentClass>
void TTrainingBoxAlgorithm<CBoxAlgorithmParentClass>::setSampleCountPerBuffer(const OpenViBE::uint32 ui32SampleCountPerBuffer)
{
	m_ui32SampleCountPerBuffer=ui32SampleCountPerBuffer;
}

template <class CBoxAlgorithmParentClass>
void TTrainingBoxAlgorithm<CBoxAlgorithmParentClass>::setSamplingRate(const OpenViBE::uint32 ui32SamplingFrequency)
{
	m_pPendingSignal->setSamplingRate(ui32SamplingFrequency);
}

template <class CBoxAlgorithmParentClass>
void TTrainingBoxAlgorithm<CBoxAlgorithmParentClass>::setSampleBuffer(const OpenViBE::float64* pBuffer)
{
	OpenViBEToolkit::insertBufferSamples(*m_pPendingSignal, m_pPendingSignal->getSampleCount(), m_ui32SampleCountPerBuffer, pBuffer, m_pPendingSignal);

	this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager()
		<< OpenViBE::Kernel::LogLevel_Debug
		<< "Appended "
		<< m_ui32SampleCountPerBuffer
		<< " bytes resulting in "
		<< (m_pPendingSignal->getDuration() >> 32)
		<< " seconds of signal\n";
}

// ________________________________________________________________________________________________________________
//
// Stimulation input reader callback
// ________________________________________________________________________________________________________________
//

template <class CBoxAlgorithmParentClass>
void TTrainingBoxAlgorithm<CBoxAlgorithmParentClass>::setStimulationCount(const OpenViBE::uint32 ui32StimulationCount)
{
}

template <class CBoxAlgorithmParentClass>
void TTrainingBoxAlgorithm<CBoxAlgorithmParentClass>::setStimulation(const OpenViBE::uint32 ui32StimulationIndex, const OpenViBE::uint64 ui64StimulationIdentifier, const OpenViBE::uint64 ui64StimulationDate)
{
	if(ui64StimulationIdentifier==this->getStimulationIdentifierTrain())
	{
		std::vector<ISignalTrial*>::iterator itSignalTrial;

		this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager()
			<< OpenViBE::Kernel::LogLevel_Trace
			<< "Constituting a signal trial set based on previous signal trials...\n";

		ISignalTrialSet* l_pSignalTrialSet=OpenViBEToolkit::createSignalTrialSet();
		for(itSignalTrial=m_vSignalTrial.begin(); itSignalTrial!=m_vSignalTrial.end(); itSignalTrial++)
		{
			l_pSignalTrialSet->addSignalTrial(**itSignalTrial);
		}

		this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager()
			<< OpenViBE::Kernel::LogLevel_Info
			<< "Calling train function...\n";

#if 0
		{
			ISignalTrialSet& rTrialSet=*l_pSignalTrialSet;
			FILE* l_pDump=fopen("dump.txt", "wt");
			fprintf(l_pDump, "# dump\n");
			for(OpenViBE::uint32 i=0; i<rTrialSet.getSignalTrialCount(); i++)
			{
				ISignalTrial& l_rTrial=rTrialSet.getSignalTrial(i);
				for(OpenViBE::uint32 j=0; j<l_rTrial.getSampleCount(); j++)
				{
					for(OpenViBE::uint32 k=0; k<l_rTrial.getChannelCount(); k++)
					{
						fprintf(l_pDump, "%f ", l_rTrial.getChannelSampleBuffer(k)[j]);
					}
					fprintf(l_pDump, "\n");
				}
			}
			fclose(l_pDump);
			l_pDump=NULL;
		}
#endif

		this->train(*l_pSignalTrialSet);

		this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager()
			<< OpenViBE::Kernel::LogLevel_Trace
			<< "Training done... will clear signal trials and signal trial set now...\n";

		for(itSignalTrial=m_vSignalTrial.begin(); itSignalTrial!=m_vSignalTrial.end(); itSignalTrial++)
		{
			OpenViBEToolkit::releaseSignalTrial(*itSignalTrial);
		}
		OpenViBEToolkit::releaseSignalTrialSet(l_pSignalTrialSet);
		m_vSignalTrial.clear();

		this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager()
			<< OpenViBE::Kernel::LogLevel_Info
			<< "Training phase finished !\n";
	}
	else if(ui64StimulationIdentifier==this->getStimulationIdentifierTrialStart())
	{
		m_ui64TrialStartTime=ui64StimulationDate;
		this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager()
			<< OpenViBE::Kernel::LogLevel_Trace
			<< "Saved trial start time "
			<< OpenViBE::time64(m_ui64TrialStartTime)
			<< "...\n";
	}
	else if(ui64StimulationIdentifier==this->getStimulationIdentifierTrialEnd())
	{
		m_ui64TrialEndTime=ui64StimulationDate;
		this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager()
			<< OpenViBE::Kernel::LogLevel_Trace
			<< "Saved trial end time "
			<< OpenViBE::time64(m_ui64TrialEndTime)
			<< "...\n";
	}
	else if(this->getStimulationIdentifierTrialLabelRangeStart()<=OpenViBE::CIdentifier(ui64StimulationIdentifier) && OpenViBE::CIdentifier(ui64StimulationIdentifier)<=this->getStimulationIdentifierTrialLabelRangeEnd())
	{
		m_oTrialLabel=ui64StimulationIdentifier;
		this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager()
			<< OpenViBE::Kernel::LogLevel_Trace
			<< "Labeled trial "
			<< m_oTrialLabel
			<< "...\n";
	}
	else
	{
		this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager()
			<< OpenViBE::Kernel::LogLevel_Debug
			<< "Unhandled stimulation "
			<< OpenViBE::CIdentifier(ui64StimulationIdentifier)
			<< " at time "
			<< OpenViBE::time64(ui64StimulationDate)
			<< "\n";
	}

	if(m_ui64TrialEndTime!=_no_time_ && m_ui64TrialStartTime!=_no_time_ && m_ui64TrialEndTime > m_ui64TrialStartTime)
	{
		OpenViBE::uint32 l_ui32SampleCount=(OpenViBE::uint32)(((m_ui64TrialEndTime-m_ui64TrialStartTime)*m_pPendingSignal->getSamplingRate())>>32);

		this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager()
			<< OpenViBE::Kernel::LogLevel_Trace
			<< "Created trial "
			<< OpenViBE::time64(m_ui64TrialStartTime)
			<< "-"
			<< OpenViBE::time64(m_ui64TrialEndTime)
			<< " with "
			<< l_ui32SampleCount
			<< " samples\n";

		OpenViBEToolkit::ISignalTrial* l_pSignalTrial=OpenViBEToolkit::createSignalTrial();
		OpenViBEToolkit::copyHeader(*l_pSignalTrial, m_pPendingSignal);
		OpenViBEToolkit::selectTime(*l_pSignalTrial, m_ui64TrialStartTime, m_ui64TrialEndTime, m_pPendingSignal);
		l_pSignalTrial->setLabelIdentifier(m_oTrialLabel);

		m_vSignalTrial.push_back(l_pSignalTrial);

		m_ui64TrialStartTime=_no_time_;
		m_ui64TrialEndTime=_no_time_;
		m_oTrialLabel=OV_UndefinedIdentifier;
	}
}

// ________________________________________________________________________________________________________________
//

};

#endif // __OpenViBEToolkit_TTrainingBoxAlgorithm_H__
