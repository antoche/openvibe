#ifndef __OpenViBEApplication_CNeurofeedbackManipulator_H__
#define __OpenViBEApplication_CNeurofeedbackManipulator_H__

#include "ova_defines.h"

class CNeurofeedbackManipulator : public OMK::SimulatedObject
{
public:

	DECLARE_OBJECT_FACTORY(CNeurofeedbackManipulator);

	virtual void init(void);
	virtual void compute(void);
	virtual bool processEvent(OMK::Event* pEvent);

	Position m_oPosition;
	Orientation m_oOrientation;
	VrpnAnalogState m_vAnalog;
	std::list < OMK::Name > m_vAnalogName;
	bool m_bFirstTime;
	double m_dLowestAnalogEver;
	double m_dHighestAnalogEver;
	double m_dTotal;
	double m_dCount;
	int m_iLastPrint;

	double _AnalogRotation_;
	double _AnalogScale_;
	double _AnalogSoften_;
	double _AnalogFall_;
};

#endif // __OpenViBEApplication_CNeurofeedbackManipulator_H__
