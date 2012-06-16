#include "ovaCManipulable3DEntity.h"

REGISTER_OBJECT_FACTORY(CManipulable3DEntity, "ovaCManipulable3DEntity");

CManipulable3DEntity::CManipulable3DEntity(OMK::Controller& rController, const OMK::ObjectDescriptor& rObjectDescriptor)
	:OMK::SimulatedObject(rController, rObjectDescriptor)
{
}

CManipulable3DEntity::~CManipulable3DEntity(void)
{
}

/*
	,m_rPositionOutput(addOutput < OMK::Translation > ("position", new OMK::Polator < OMK::Translation > ()))
	,m_rOrientationOutput(addOutput < OMK::HPRRotation > ("orientation", new OMK::Polator < OMK::HPRRotation > ()))
	,m_rScaleOutput(addOutput < OMK::Scale > ("scale", new OMK::Polator < OMK::Scale > ()))
	,m_bIsGoal(false)
{
	visualiseOutput < OMK::vTranslationInputHandler >(m_rPositionOutput, "DCS_position") ;
	visualiseOutput < OMK::vHPRRotationInputHandler >(m_rOrientationOutput, "DCS_orientation") ;
	visualiseOutput < OMK::vScaleInputHandler >(m_rScaleOutput, "DCS_scale") ;
}
*/

void CManipulable3DEntity::init(void)
{
/*
	m_rTransformOutput.set(m_oTransform);
*/
}

#define duration 1250
#define fadein 500
#define fadeout 500

void CManipulable3DEntity::compute(void)
{
	if(m_bIsGoal)
	{
		int l_iDelta=getSimulatedDate()-m_iGoalDate;
		if(l_iDelta<duration)
		{
			if(l_iDelta<fadein)
			{
				double l_dCoef=(fadein-l_iDelta)/(double)fadein; // 1-0
				double l_dRotation=180*pow(l_dCoef, 3);
				m_oOrientation[0]=0;
				m_oOrientation[1]=3*l_dRotation;
				m_oOrientation[2]=l_dRotation*sin(M_PI*pow(l_dCoef, 3)*1);

				m_oPosition[0]=0;
				m_oPosition[1]=5+7*sin(l_dCoef*M_PI/2);
				m_oPosition[2]=0;
			}

			if(l_iDelta>duration-fadeout)
			{
				double l_dCoef=(l_iDelta-duration+fadeout)/(double)(fadeout); // 0-1
				double l_dRotation=3*180*pow(l_dCoef, 4);
				m_oOrientation[0]=l_dRotation;
				m_oOrientation[1]=3*l_dRotation*sin(M_PI*pow(l_dCoef, 3)*2);
				m_oOrientation[2]=0;

				m_oPosition[0]=0;
				m_oPosition[1]=5+7*l_dCoef*sin(l_dCoef*M_PI/2);
				m_oPosition[2]=0;
			}
			if(l_iDelta>=fadein && l_iDelta<=duration-fadeout)
			{
				m_oOrientation[0]=0;
				m_oOrientation[1]=0;
				m_oOrientation[2]=0;

				m_oPosition[0]=0;
				m_oPosition[1]=5;
				m_oPosition[2]=0;
			}
		}
	}
/*
	m_rTransformOutput.set(m_oTransform);
*/
}

bool CManipulable3DEntity::processEvent(OMK::Event* pEvent)
{
	if(pEvent->eventId == g_sManipulate3DEntity_Goal)
	{
		m_bIsGoal=true;
		m_iGoalDate=getSimulatedDate();
	}

	if(pEvent->eventId == g_sManipulate3DEntity_Reset)
	{
		m_oPosition[0]=0;
		m_oPosition[1]=0.2;
		m_oPosition[2]=0;

		m_oOrientation[0]=0;
		m_oOrientation[1]=0;
		m_oOrientation[2]=0;

#if defined _DEBUG_
		std::cout << "### reseted\n";
#endif
		return true;
	}

	if(pEvent->eventId == g_sManipulate3DEntity_Translate)
	{
		PositionEvent* l_pEvent=dynamic_cast < PositionEvent* >(pEvent);
		if(l_pEvent)
		{
			const Position& l_rValue=l_pEvent->value;
			m_oPosition[0]+=l_rValue[0];
			m_oPosition[1]+=l_rValue[1];
			m_oPosition[2]+=l_rValue[2];

#if defined _DEBUG_
			std::cout << "### translated\n";
#endif
			return true;
		}
	}

	if(pEvent->eventId == g_sManipulate3DEntity_SetPosition)
	{
		PositionEvent* l_pEvent=dynamic_cast < PositionEvent* >(pEvent);
		if(l_pEvent)
		{
			const Position& l_rValue=l_pEvent->value;
			m_oPosition[0]=l_rValue[0];
			m_oPosition[1]=l_rValue[1];
			m_oPosition[2]=l_rValue[2];

#if defined _DEBUG_
			std::cout << "### moved\n";
#endif
			return true;
		}
	}

	if(pEvent->eventId == g_sManipulate3DEntity_Scale)
	{
		ScaleEvent* l_pEvent=dynamic_cast < ScaleEvent* >(pEvent);
		if(l_pEvent)
		{
			const Scale& l_rValue=l_pEvent->value;
			m_oScale[0]*=l_rValue[0];
			m_oScale[1]*=l_rValue[1];
			m_oScale[2]*=l_rValue[2];

#if defined _DEBUG_
			std::cout << "### scaled\n";
#endif
			return true;
		}
	}

	if(pEvent->eventId == g_sManipulate3DEntity_SetScale)
	{
		ScaleEvent* l_pEvent=dynamic_cast < ScaleEvent* >(pEvent);
		if(l_pEvent)
		{
			const Scale& l_rValue=l_pEvent->value;
			m_oScale[0]=l_rValue[0];
			m_oScale[1]=l_rValue[1];
			m_oScale[2]=l_rValue[2];

#if defined _DEBUG_
			std::cout << "### scaled fixed\n";
#endif
			return true;
		}
	}

	if(pEvent->eventId == g_sManipulate3DEntity_Rotate)
	{
		OrientationEvent* l_pEvent=dynamic_cast < OrientationEvent* >(pEvent);
		if(l_pEvent)
		{
			const Orientation& l_rValue=l_pEvent->value;
			m_oOrientation[0]+=l_rValue[0];
			m_oOrientation[1]+=l_rValue[1];
			m_oOrientation[2]+=l_rValue[2];

#if defined _DEBUG_
			std::cout << "### rotated\n";
#endif
			return true;
		}
	}

	if(pEvent->eventId == g_sManipulate3DEntity_SetOrientation)
	{
		OrientationEvent* l_pEvent=dynamic_cast < OrientationEvent* >(pEvent);
		if(l_pEvent)
		{
			const Orientation& l_rValue=l_pEvent->value;
			m_oOrientation[0]=l_rValue[0];
			m_oOrientation[1]=l_rValue[1];
			m_oOrientation[2]=l_rValue[2];

#if defined _DEBUG_
			std::cout << "### orirentation set\n";
#endif
			return true;
		}
	}

	return false;
}
