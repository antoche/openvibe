#ifndef __OpenViBEApplication_CNeurofeedbackSwitch_H__
#define __OpenViBEApplication_CNeurofeedbackSwitch_H__

#include "ova_defines.h"

#include <OMKOutput.h>
#include <OMKTransform.h>

class CNeurofeedbackSwitch : public OMK::SimulatedObject
{
public:

	DECLARE_OBJECT_FACTORY(CNeurofeedbackSwitch);

	virtual void init(void);
	virtual void compute(void);
	virtual bool processEvent(OMK::Event* pEvent);

	bool m_bButton[1024];
	Position _Position_;
	Position _FarAway_;
};

#endif // __OpenViBEApplication_CNeurofeedbackSwitch_H__
