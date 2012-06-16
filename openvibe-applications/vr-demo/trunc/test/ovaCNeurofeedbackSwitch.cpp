#include "ovaCNeurofeedbackSwitch.h"

REGISTER_OBJECT_FACTORY(CNeurofeedbackSwitch, "ovaCNeurofeedbackSwitch");

CNeurofeedbackSwitch::CNeurofeedbackSwitch(OMK::Controller& rController, const OMK::ObjectDescriptor& rObjectDescriptor)
	:OMK::SimulatedObject(rController, rObjectDescriptor)
{
}

CNeurofeedbackSwitch::~CNeurofeedbackSwitch(void)
{
}

void CNeurofeedbackSwitch::init(void)
{
	_FarAway_[0]=1000;
	_FarAway_[1]=1000;
	_FarAway_[2]=1000;

	_Position_[0]=0;
	_Position_[1]=0;
	_Position_[2]=0;

	sendValuedEvent("OpenViBE_ball",         g_sManipulate3DEntity_SetPosition, PositionType(_FarAway_));
	sendValuedEvent("OpenViBE_passive_ball", g_sManipulate3DEntity_SetPosition, PositionType(_FarAway_));

	registerForSignal(g_sVrpnButtonStateUpdate);
}

void CNeurofeedbackSwitch::compute(void)
{
}

bool CNeurofeedbackSwitch::processEvent(OMK::Event* pEvent)
{
	if(pEvent->eventId == g_sVrpnButtonStateUpdate)
	{
		VrpnButtonStateEvent* l_pEvent=dynamic_cast < VrpnButtonStateEvent* >(pEvent);
		if(l_pEvent)
		{
			const VrpnButtonState& l_rVrpnButtonState=l_pEvent->value;
			if(l_rVrpnButtonState.first<(int)(sizeof(m_bButton)/sizeof(m_bButton[0])))
			{
				m_bButton[l_rVrpnButtonState.first]=(l_rVrpnButtonState.second?true:false);

#if _DEBUG_
std::cout << "got event " << *l_pEvent << std::endl;
#endif
				sendValuedEvent("OpenViBE_ball",         g_sManipulate3DEntity_SetPosition, PositionType(m_bButton[2]?_Position_:_FarAway_));
				sendValuedEvent("OpenViBE_passive_ball", g_sManipulate3DEntity_SetPosition, PositionType(m_bButton[3]?_Position_:_FarAway_));
			}
		}
		return true;
	}
	return false;
}

