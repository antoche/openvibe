#ifndef __OpenViBEApplication_CManipulable3DEntity_H__
#define __OpenViBEApplication_CManipulable3DEntity_H__

#include "ova_defines.h"

class CManipulable3DEntity : public OMK::SimulatedObject
{
public:

	DECLARE_OBJECT_FACTORY(CManipulable3DEntity);

	virtual void init(void);
	virtual void compute(void);
	virtual bool processEvent(OMK::Event* pEvent);

	Position m_oPosition;
	Orientation m_oOrientation;
	Scale m_oScale;

/*
	OMK::Output < OMK::Type::Transform >& m_rTransformOutput;
*/

	bool m_bIsGoal;
	int m_iGoalDate;
};

#endif // __OpenViBEApplication_CManipulable3DEntity_H__
