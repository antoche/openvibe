#include "ovaCNeurofeedbackScoreCounter.h"

REGISTER_OBJECT_FACTORY(CNeurofeedbackScoreCounter, "ovaCNeurofeedbackScoreCounter");

CNeurofeedbackScoreCounter::CNeurofeedbackScoreCounter(OMK::Controller& rController, const OMK::ObjectDescriptor& rObjectDescriptor)
	:OMK::SimulatedObject(rController, rObjectDescriptor)
{
}

CNeurofeedbackScoreCounter::~CNeurofeedbackScoreCounter(void)
{
}

void CNeurofeedbackScoreCounter::init(void)
{
	registerForSignal(g_sVrpnButtonStateUpdate);
	registerForSignal(g_sVrpnAnalogStateUpdate);

	m_bShouldCare=false;
	m_bWasZero=false;
	m_uiScore=0;

	m_oScale[0]=1;
	m_oScale[1]=1;
	m_oScale[2]=1;

	m_oScaleTarget[0]=1;
	m_oScaleTarget[1]=1;
	m_oScaleTarget[2]=1;

	sendValuedEvent("OpenViBE_score_counter", g_sManipulate3DEntity_SetScale, ScaleType(m_oScale));
}

void CNeurofeedbackScoreCounter::compute(void)
{
	double l_dSpeed=.01;
	m_oScale=(float)l_dSpeed*m_oScaleTarget+(float)(1-l_dSpeed)*m_oScale;

	sendValuedEvent("OpenViBE_score_counter", g_sManipulate3DEntity_SetScale, ScaleType(m_oScale));
}

bool CNeurofeedbackScoreCounter::processEvent(OMK::Event* pEvent)
{
	if(pEvent->eventId == g_sVrpnButtonStateUpdate)
	{
		VrpnButtonStateEvent* l_pEvent=dynamic_cast < VrpnButtonStateEvent * >(pEvent);
		if(l_pEvent)
		{
			const VrpnButtonState& l_rVrpnButtonState=l_pEvent->value;
			if(l_rVrpnButtonState.first<(int)(sizeof(m_bButton)/sizeof(m_bButton[0])))
			{
				m_bButton[l_rVrpnButtonState.first]=(l_rVrpnButtonState.second?true:false);
				if(l_rVrpnButtonState.first==3)
				{
					if(m_bButton[l_rVrpnButtonState.first])
					{
						m_bShouldCare=true;
#ifdef _DEBUG_
						std::cout << "Should care\n";
#endif
					}
					else
					{
						m_bShouldCare=false;
#ifdef _DEBUG_
						std::cout << "Should not care\n";
#endif
					}
				}
			}
		}
		return true;
	}

	if(pEvent->eventId == g_sVrpnAnalogStateUpdate)
	{
		VrpnAnalogStateEvent* l_pEvent=dynamic_cast < VrpnAnalogStateEvent* >(pEvent);
		if(l_pEvent)
		{
			const VrpnAnalogState& l_rVrpnAnalogState=l_pEvent->value;
			VrpnAnalogState::const_iterator itAnalog=l_rVrpnAnalogState.begin();
			if(m_bWasZero)
			{
				if(*itAnalog>=1E-3)
				{
					if(m_bShouldCare)
					{
						m_oScaleTarget[1]+=.2;
						m_uiScore+=1;
						m_bShouldCare=false;
						std::cout << "NeW SCoRe : " << m_uiScore << "\n";
					}

					m_bWasZero=false;

				}
			}
			else
			{
				if(*itAnalog<1E-3)
				{
					m_bWasZero=true;
				}
			}
		}
	}

	return false;
}
