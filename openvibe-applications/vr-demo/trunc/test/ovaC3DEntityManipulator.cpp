#include "ovaC3DEntityManipulator.h"

REGISTER_OBJECT_FACTORY(C3DEntityManipulator, "ovaC3DEntityManipulator");

C3DEntityManipulator::C3DEntityManipulator(OMK::Controller& rController, const OMK::ObjectDescriptor& rObjectDescriptor)
	:OMK::SimulatedObject(rController, rObjectDescriptor)
{
}

C3DEntityManipulator::~C3DEntityManipulator(void)
{
}

void C3DEntityManipulator::init(void)
{
	registerForSignal(g_sVrpnButtonStateUpdate);
	registerForSignal(g_sVrpnAnalogStateUpdate);

	m_oBallPosition[0]=1000000;
	m_oBallPosition[1]=1000000;
	m_oBallPosition[2]=1000000;

	m_bButton[0]=false;
	m_bButton[1]=false;
	m_bButton[2]=false;
}

void C3DEntityManipulator::compute(void)
{
	if(m_bButton[0] && m_vAnalog.size())
	{
		while(m_vAnalog.size())
		{
			double l_dValue=m_vAnalog.front()*0.002;

			m_oBallSpeed[0]=0;
			m_oBallSpeed[1]=0;
			m_oBallSpeed[2]=l_dValue;

			m_oBallPosition[0]+=m_oBallSpeed[0];
			m_oBallPosition[1]+=m_oBallSpeed[1];
			m_oBallPosition[2]+=m_oBallSpeed[2];

			m_vAnalog.pop_front();
		}
	}
	else
	{
		m_oBallPosition[0]+=m_oBallSpeed[0];
		m_oBallPosition[1]+=m_oBallSpeed[1];
		m_oBallPosition[2]+=m_oBallSpeed[2];
	}

	const double l_fViscosity=0.010;
	m_oBallSpeed[0]*=(1-l_fViscosity);
	m_oBallSpeed[1]*=(1-l_fViscosity);
	m_oBallSpeed[2]*=(1-l_fViscosity);

	Position l_oFar;
	l_oFar[0]=1000000;
	l_oFar[1]=1000000;
	l_oFar[2]=1000000;
	sendValuedEvent((m_bButton[1] || m_bButton[2])?"OpenViBE_ball":"OpenViBE_passive_ball", g_sManipulate3DEntity_SetPosition, PositionType(m_oBallPosition));
	sendValuedEvent((m_bButton[1] || m_bButton[2])?"OpenViBE_passive_ball":"OpenViBE_ball", g_sManipulate3DEntity_SetPosition, PositionType(l_oFar));

	Orientation l_oOrientation;

	l_oOrientation[0]=0;
	l_oOrientation[1]=(m_oBallPosition[2]*180.0/M_PI)/0.5;
	l_oOrientation[2]=0;

	sendValuedEvent("OpenViBE_passive_ball", g_sManipulate3DEntity_SetOrientation, OrientationType(l_oOrientation));
	sendValuedEvent("OpenViBE_ball",         g_sManipulate3DEntity_SetOrientation, OrientationType(l_oOrientation));

	if(m_oBallPosition[2]<-7 || m_oBallPosition[2]>7)
	{
		sendEvent(getName(), g_s3DEntityManipulator_Reset);
		sendEvent("OpenViBE_goal_display", g_sManipulate3DEntity_Goal);
	}
}

bool C3DEntityManipulator::processEvent(OMK::Event* pEvent)
{
	if(pEvent->eventId == g_s3DEntityManipulator_Reset)
	{
		m_oBallPosition[0]=0;
		m_oBallPosition[1]=0.5;
		m_oBallPosition[2]=0;

		m_oBallSpeed[0]=0;
		m_oBallSpeed[1]=0;
		m_oBallSpeed[2]=0;

		return true;
	}

	if(pEvent->eventId == g_sVrpnButtonStateUpdate)
	{
		VrpnButtonStateEvent* l_pEvent=dynamic_cast < VrpnButtonStateEvent* >(pEvent);
		if(l_pEvent)
		{
			const VrpnButtonState& l_rVrpnButtonState=l_pEvent->value;
			if(l_rVrpnButtonState.first<(int)(sizeof(m_bButton)/sizeof(m_bButton[0])))
			{
				m_bButton[l_rVrpnButtonState.first]=(l_rVrpnButtonState.second?true:false);
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
			m_vCacheAnalog.push_back(l_rVrpnAnalogState.front());
			if(m_vCacheAnalog.size()>10)
			{
				m_vCacheAnalog.pop_front();
			}

			if(m_bButton[0])
			{
				double l_dAnalog=0;
				std::list<double>::iterator itAnalog;
				for(itAnalog=m_vCacheAnalog.begin(); itAnalog!=m_vCacheAnalog.end(); itAnalog++)
				{
					l_dAnalog+=*itAnalog;
				}
				m_vAnalog.push_back(l_dAnalog/m_vCacheAnalog.size());
			}
		}
		return true;
	}

	return false;
}
