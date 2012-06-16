#ifndef __OpenViBEApplication_CNeurofeedbackScoreCounter_H__
#define __OpenViBEApplication_CNeurofeedbackScoreCounter_H__

#include "ova_defines.h"

#include <OMKTransform.h>

class CNeurofeedbackScoreCounter : public OMK::SimulatedObject
{
public:

	DECLARE_OBJECT_FACTORY(CNeurofeedbackScoreCounter);

	virtual void init(void);
	virtual void compute(void);
	virtual bool processEvent(OMK::Event* pEvent);

	Scale m_oScale;
	Scale m_oScaleTarget;
	bool m_bButton[1024];
	bool m_bShouldCare;
	bool m_bWasZero;
	unsigned int m_uiScore;
};

#endif // __OpenViBEApplication_CNeurofeedbackScoreCounter_H__
