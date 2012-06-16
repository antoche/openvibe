#include "ovaCNeurofeedbackManipulator.h"

#include <OMKMultipleConfigurationParameter.h>
#include <OMKUniqueConfigurationParameter.h>

REGISTER_OBJECT_FACTORY(CNeurofeedbackManipulator, "ovaCNeurofeedbackManipulator");

CNeurofeedbackManipulator::CNeurofeedbackManipulator(OMK::Controller& rController, const OMK::ObjectDescriptor& rObjectDescriptor)
	:OMK::SimulatedObject(rController, rObjectDescriptor)
{
}

CNeurofeedbackManipulator::~CNeurofeedbackManipulator(void)
{
}

void CNeurofeedbackManipulator::init(void)
{
	static OMK::Name g_sVrpnAnalogStateUpdate("vrpn_analog state update");

	_AnalogRotation_=0.25;
	_AnalogScale_=0.004;
	_AnalogSoften_=1;
	_AnalogFall_=.99;

	m_dLowestAnalogEver=1E10;
	m_dHighestAnalogEver=-1E10;
	m_iLastPrint=-1;

	registerForSignal(g_sVrpnAnalogStateUpdate);

	OMK::ParametersAccessor::get < double > (
		getConfigurationParameters(),
		"_AnalogRotation_",
		_AnalogRotation_);
	OMK::ParametersAccessor::get < double > (
		getConfigurationParameters(),
		"_AnalogScale_",
		_AnalogScale_);
	OMK::ParametersAccessor::get < double > (
		getConfigurationParameters(),
		"_AnalogSoften_",
		_AnalogSoften_);
	OMK::ParametersAccessor::get < double > (
		getConfigurationParameters(),
		"_AnalogFall_",
		_AnalogFall_);

	m_bFirstTime=true;

	m_oPosition[0]=4;
	m_oPosition[1]=0;
	m_oPosition[2]=-3;

	m_oOrientation[0]=0;
	m_oOrientation[1]=0;
	m_oOrientation[2]=0;

	m_dTotal=0;
	m_dCount=0;
}

void CNeurofeedbackManipulator::compute(void)
{
	VrpnAnalogState::iterator itAnalog;
	std::list < OMK::Name > :: iterator itAnalogName;
	for(itAnalog=m_vAnalog.begin(), itAnalogName=m_vAnalogName.begin(); itAnalog!=m_vAnalog.end(); ++itAnalog, ++itAnalogName)
	{
		double l_dAnalog=(*itAnalog);
		m_dTotal+=l_dAnalog;
		m_dCount+=1.;

		if(l_dAnalog>m_dHighestAnalogEver) m_dHighestAnalogEver=l_dAnalog;
		if(l_dAnalog<m_dLowestAnalogEver)  m_dLowestAnalogEver=l_dAnalog;
		if(m_iLastPrint!=getSimulatedDate()/1000)
		{
			m_iLastPrint=getSimulatedDate()/1000;

#if _DEBUG_
			std::cout << "Current session : mean/min/max = "
				<< (m_dTotal/m_dCount) << "/"
				<< m_dLowestAnalogEver << "/"
				<< m_dHighestAnalogEver << "/"
				<< "\n";

			std::cout << "\n";
#endif

			m_dTotal=0;
			m_dCount=0;
			m_dLowestAnalogEver=1E10;
			m_dHighestAnalogEver=-1E10;
		}

		if(l_dAnalog<=0)
		{
			m_oOrientation[0]*=_AnalogFall_;
			m_oOrientation[1]*=_AnalogFall_;
			m_oOrientation[2]*=_AnalogFall_;

			m_oPosition[1]*=_AnalogFall_;
		}
		else
		{
			m_oOrientation[0]+=_AnalogRotation_*((rand()&1)==0?-1:1);
			m_oOrientation[1]+=_AnalogRotation_*((rand()&1)==0?-1:1);
			m_oOrientation[2]+=_AnalogRotation_*((rand()&1)==0?-1:1);

			m_oPosition[1]+=l_dAnalog*_AnalogScale_;

			if(m_oPosition[1]>6)
			{
				m_oPosition[1]=6;
			}
		}

		if(m_oOrientation[0]>5) m_oOrientation[0]=5;
		if(m_oOrientation[1]>5) m_oOrientation[1]=5;
		if(m_oOrientation[2]>5) m_oOrientation[2]=5;

		if(m_oOrientation[0]<-5) m_oOrientation[0]=-5;
		if(m_oOrientation[1]<-5) m_oOrientation[1]=-5;
		if(m_oOrientation[2]<-5) m_oOrientation[2]=-5;

		sendValuedEvent(*itAnalogName, g_sManipulate3DEntity_SetOrientation, OrientationType(m_oOrientation));
		sendValuedEvent(*itAnalogName, g_sManipulate3DEntity_SetPosition, PositionType(m_oPosition));
	}
}

bool CNeurofeedbackManipulator::processEvent(OMK::Event* pEvent)
{
	if(pEvent->eventId == g_sVrpnAnalogStateUpdate)
	{
		VrpnAnalogStateEvent* l_pEvent=dynamic_cast < VrpnAnalogStateEvent* >(pEvent);
		if(l_pEvent)
		{
			VrpnAnalogState l_vAnalog=l_pEvent->value;
			VrpnAnalogState::iterator itAnalog1;
			VrpnAnalogState::iterator itAnalog2;

			if(m_bFirstTime)
			{
				// create objects
				size_t i=0;
				for(itAnalog1=l_vAnalog.begin(); itAnalog1!=l_vAnalog.end(); ++itAnalog1)
				{
					std::ostringstream oss;
					oss << "nf object " << i++;
					m_vAnalogName.push_back(oss.str().c_str());

					// OpenMASK won't let me delete those configuration parameters,
					// However, way to go :o)
					OMK::MultipleConfigurationParameter* l_pUserParams=new OMK::MultipleConfigurationParameter();
					l_pUserParams->appendSubDescriptorNamed("geometryFileName", new OMK::UniqueConfigurationParameter("./Data/neurofeedback.wrl"));

					getController().createObject(
						OMK::ObjectDescriptor(
							oss.str().c_str(),
							"ovManipulable3DEntity",
							getController().computeAdequateFrequency(75),
							l_pUserParams),
						getFathersDescriptor().getName());

					m_vAnalog.push_back(0);
				}
				m_bFirstTime=false;
			}

std::cout << "got data\n";
			for(itAnalog1=l_vAnalog.begin(), itAnalog2=m_vAnalog.begin(); itAnalog1!=l_vAnalog.end() && itAnalog2!=m_vAnalog.end(); ++itAnalog1, ++itAnalog2)
			{
				*itAnalog2 = *itAnalog1*_AnalogSoften_ + *itAnalog2*(1-_AnalogSoften_);
			}

			return true;
		}
	}

	return false;
}
