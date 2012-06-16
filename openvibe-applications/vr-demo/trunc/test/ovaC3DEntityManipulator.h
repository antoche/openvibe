#ifndef __OpenViBEApplication_C3DEntityManipulator_H__
#define __OpenViBEApplication_C3DEntityManipulator_H__

#include "ova_defines.h"

class C3DEntityManipulator : public OMK::SimulatedObject
{
public:

	DECLARE_OBJECT_FACTORY(C3DEntityManipulator);

	virtual void init(void);
	virtual void compute(void);
	virtual bool processEvent(OMK::Event* pEvent);

	bool m_bButton[3];
	std::list<double> m_vCacheAnalog;
	std::list<double> m_vAnalog;
	Wm4::Vector3f m_oBallPosition;
	Wm4::Vector3f m_oBallSpeed;
};

#endif // __OpenViBEApplication_C3DEntityManipulator_H__
