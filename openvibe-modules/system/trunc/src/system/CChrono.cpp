#include "CChrono.h"
#include "Time.h"

using namespace System;

CChrono::CChrono(void)
	:m_pStepInTime(NULL)
	,m_pStepOutTime(NULL)
	,m_ui32StepCount(0)
	,m_ui32StepIndex(0)
	,m_bIsInStep(false)
	,m_bHasNewEstimation(false)
	,m_ui64TotalStepInTime(0)
	,m_ui64TotalStepOutTime(0)
{
}

CChrono::~CChrono(void)
{
	delete [] m_pStepInTime;
	delete [] m_pStepOutTime;
}

boolean CChrono::reset(uint32 ui32StepCount)
{
	if(!ui32StepCount)
	{
		return false;
	}

	uint64* l_pStepInTime=new uint64[ui32StepCount+1];
	uint64* l_pStepOutTime=new uint64[ui32StepCount+1];
	if(!l_pStepInTime || !l_pStepOutTime)
	{
		delete [] l_pStepInTime;
		delete [] l_pStepOutTime;
		return false;
	}

	for(uint32 i=0; i<=ui32StepCount; i++)
	{
		l_pStepInTime[i]=0;
		l_pStepOutTime[i]=0;
	}

	delete [] m_pStepInTime;
	delete [] m_pStepOutTime;
	m_pStepInTime=l_pStepInTime;
	m_pStepOutTime=l_pStepOutTime;

	m_ui32StepCount=ui32StepCount;
	m_ui32StepIndex=0;
	m_bIsInStep=false;
	m_bHasNewEstimation=false;

	m_ui64TotalStepInTime=0;
	m_ui64TotalStepOutTime=0;

	return true;
}

boolean CChrono::stepIn(void)
{
	if(m_bIsInStep || !m_ui32StepCount)
	{
		return false;
	}

	m_bIsInStep=!m_bIsInStep;

	m_pStepInTime[m_ui32StepIndex]=Time::zgetTime();
	if(m_ui32StepIndex==m_ui32StepCount)
	{
		m_ui64TotalStepInTime=0;
		m_ui64TotalStepOutTime=0;
		for(uint32 i=0; i<m_ui32StepCount; i++)
		{
			m_ui64TotalStepInTime+=m_pStepOutTime[i]-m_pStepInTime[i];
			m_ui64TotalStepOutTime+=m_pStepInTime[i+1]-m_pStepOutTime[i];
		}
		m_pStepInTime[0]=m_pStepInTime[m_ui32StepCount];
		m_ui32StepIndex=0;
		m_bHasNewEstimation=true;
	}
	else
	{
		m_bHasNewEstimation=false;
	}

	return true;
}

boolean CChrono::stepOut(void)
{
	if(!m_bIsInStep || !m_ui32StepCount)
	{
		return false;
	}

	m_bIsInStep=!m_bIsInStep;

	m_pStepOutTime[m_ui32StepIndex]=Time::zgetTime();
	m_ui32StepIndex++;

	return true;
}

uint64 CChrono::getTotalStepInDuration(void) const
{
	return m_ui64TotalStepInTime;
}

uint64 CChrono::getTotalStepOutDuration(void) const
{
	return m_ui64TotalStepOutTime;
}

uint64 CChrono::getAverageStepInDuration(void) const
{
	return m_ui32StepCount?this->getTotalStepInDuration()/m_ui32StepCount:0;
}

uint64 CChrono::getAverageStepOutDuration(void) const
{
	return m_ui32StepCount?this->getTotalStepOutDuration()/m_ui32StepCount:0;
}

float64 CChrono::getStepInPercentage(void) const
{
	uint64 l_ui64TotalStepDuration=(this->getTotalStepInDuration()+this->getTotalStepOutDuration());
	return l_ui64TotalStepDuration?(this->getTotalStepInDuration()*100.0)/l_ui64TotalStepDuration:0;
}

float64 CChrono::getStepOutPercentage(void) const
{
	uint64 l_ui64TotalStepDuration=(this->getTotalStepOutDuration()+this->getTotalStepInDuration());
	return l_ui64TotalStepDuration?(this->getTotalStepOutDuration()*100.0)/l_ui64TotalStepDuration:0;
}

boolean CChrono::hasNewEstimation(void)
{
	return m_bHasNewEstimation;
}
