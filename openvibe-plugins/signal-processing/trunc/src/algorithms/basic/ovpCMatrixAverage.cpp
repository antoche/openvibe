#include "ovpCMatrixAverage.h"

#include <system/Memory.h>
#include <cmath>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

// ________________________________________________________________________________________________________________
//

boolean CMatrixAverage::initialize(void)
{
	ip_ui64AveragingMethod.initialize(getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_AveragingMethod));
	ip_ui64MatrixCount.initialize(getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_MatrixCount));
	ip_pMatrix.initialize(getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_Matrix));
	op_pAveragedMatrix.initialize(getOutputParameter(OVP_Algorithm_MatrixAverage_OutputParameterId_AveragedMatrix));

	return true;
}

boolean CMatrixAverage::uninitialize(void)
{
	std::deque < OpenViBE::CMatrix* >::iterator it;
	for(it=m_vHistory.begin(); it!=m_vHistory.end(); it++)
	{
		delete *it;
	}
	m_vHistory.clear();

	op_pAveragedMatrix.uninitialize();
	ip_pMatrix.uninitialize();
	ip_ui64MatrixCount.uninitialize();
	ip_ui64AveragingMethod.uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CMatrixAverage::process(void)
{
	IMatrix* l_pInputMatrix=ip_pMatrix;
	IMatrix* l_pOutputMatrix=op_pAveragedMatrix;

	boolean l_bShouldPerformAverage=false;

	if(this->isInputTriggerActive(OVP_Algorithm_MatrixAverage_InputTriggerId_Reset))
	{
		std::deque < CMatrix* >::iterator it;
		for(it=m_vHistory.begin(); it!=m_vHistory.end(); it++)
		{
			delete *it;
		}
		m_vHistory.clear();

		OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pOutputMatrix, *l_pInputMatrix);
	}

	if(this->isInputTriggerActive(OVP_Algorithm_MatrixAverage_InputTriggerId_FeedMatrix))
	{
		if(ip_ui64AveragingMethod==OVP_TypeId_EpochAverageMethod_MovingAverage.toUInteger())
		{
			CMatrix* l_pSwapMatrix=NULL;

			if(m_vHistory.size()>=ip_ui64MatrixCount)
			{
				l_pSwapMatrix=m_vHistory.front();
				m_vHistory.pop_front();
			}
			else
			{
				l_pSwapMatrix=new CMatrix();
				OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pSwapMatrix, *l_pInputMatrix);
			}

			OpenViBEToolkit::Tools::Matrix::copyContent(*l_pSwapMatrix, *l_pInputMatrix);

			m_vHistory.push_back(l_pSwapMatrix);
			l_bShouldPerformAverage=(m_vHistory.size()==ip_ui64MatrixCount);
		}
		else if(ip_ui64AveragingMethod==OVP_TypeId_EpochAverageMethod_MovingAverageImmediate.toUInteger())
		{
			CMatrix* l_pSwapMatrix=NULL;

			if(m_vHistory.size()>=ip_ui64MatrixCount)
			{
				l_pSwapMatrix=m_vHistory.front();
				m_vHistory.pop_front();
			}
			else
			{
				l_pSwapMatrix=new CMatrix();
				OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pSwapMatrix, *l_pInputMatrix);
			}

			OpenViBEToolkit::Tools::Matrix::copyContent(*l_pSwapMatrix, *l_pInputMatrix);

			m_vHistory.push_back(l_pSwapMatrix);
			l_bShouldPerformAverage=(m_vHistory.size()>0);
		}
		else if(ip_ui64AveragingMethod==OVP_TypeId_EpochAverageMethod_BlockAverage.toUInteger())
		{
			CMatrix* l_pSwapMatrix=new CMatrix();

			if(m_vHistory.size()>=ip_ui64MatrixCount)
			{
				std::deque < CMatrix* >::iterator it;
				for(it=m_vHistory.begin(); it!=m_vHistory.end(); it++)
				{
					delete *it;
				}
				m_vHistory.clear();
			}

			OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pSwapMatrix, *l_pInputMatrix);
			OpenViBEToolkit::Tools::Matrix::copyContent(*l_pSwapMatrix, *l_pInputMatrix);

			m_vHistory.push_back(l_pSwapMatrix);
			l_bShouldPerformAverage=(m_vHistory.size()==ip_ui64MatrixCount);
		}
		else if(ip_ui64AveragingMethod==OVP_TypeId_EpochAverageMethod_CumulativeAverage.toUInteger())
		{
			CMatrix* l_pSwapMatrix=new CMatrix();

			OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pSwapMatrix, *l_pInputMatrix);
			OpenViBEToolkit::Tools::Matrix::copyContent(*l_pSwapMatrix, *l_pInputMatrix);

			m_vHistory.push_back(l_pSwapMatrix);
			l_bShouldPerformAverage=(m_vHistory.size()!=0);
		}
		else
		{
			l_bShouldPerformAverage=false;
		}
	}

	if(l_bShouldPerformAverage)
	{
		OpenViBEToolkit::Tools::Matrix::clearContent(*l_pOutputMatrix);

		if(m_vHistory.size()!=0)
		{
			uint32 l_ui32Count=l_pOutputMatrix->getBufferElementCount();
			float64 l_f64Scale=1./m_vHistory.size();

			std::deque < CMatrix* >::iterator it;
			for(it=m_vHistory.begin(); it!=m_vHistory.end(); it++)
			{
				float64* l_pOutputMatrixBuffer=l_pOutputMatrix->getBuffer();
				float64* l_pInputMatrixBuffer=(*it)->getBuffer();
				for(uint32 i=0; i<l_ui32Count; i++)
				{
					*l_pOutputMatrixBuffer+=*l_pInputMatrixBuffer*l_f64Scale;
					l_pOutputMatrixBuffer++;
					l_pInputMatrixBuffer++;
				}
			}
		}

		this->activateOutputTrigger(OVP_Algorithm_MatrixAverage_OutputTriggerId_AveragePerformed, true);
	}

	return true;
}
