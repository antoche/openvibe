#include "ovtkStimulationSet.h"

using namespace OpenViBE;

boolean OpenViBEToolkit::Tools::StimulationSet::shift(IStimulationSet& rStimulationSet, const uint64 ui64TimeShift)
{
	uint64 l_ui64StimulationCount=rStimulationSet.getStimulationCount();
	for(uint64 i=0; i<l_ui64StimulationCount; i++)
	{
		rStimulationSet.setStimulationDate(i, rStimulationSet.getStimulationDate(i)+ui64TimeShift);
	}
	return true;
}

boolean OpenViBEToolkit::Tools::StimulationSet::copy(IStimulationSet& rDestinationStimulationSet, const IStimulationSet& rSourceStimulationSet, const uint64 ui64TimeShift)
{
	rDestinationStimulationSet.clear();
	return append(rDestinationStimulationSet, rSourceStimulationSet, ui64TimeShift);
}

boolean OpenViBEToolkit::Tools::StimulationSet::append(IStimulationSet& rDestinationStimulationSet, const IStimulationSet& rSourceStimulationSet, const uint64 ui64TimeShift)
{
	uint64 l_ui64StimulationCount=rSourceStimulationSet.getStimulationCount();
	for(uint64 i=0; i<l_ui64StimulationCount; i++)
	{
		rDestinationStimulationSet.appendStimulation(
			rSourceStimulationSet.getStimulationIdentifier(i),
			rSourceStimulationSet.getStimulationDate(i)+ui64TimeShift,
			rSourceStimulationSet.getStimulationDuration(i));
	}
	return true;
}

boolean OpenViBEToolkit::Tools::StimulationSet::appendRange(IStimulationSet& rDestinationStimulationSet, const IStimulationSet& rSourceStimulationSet, const uint64 ui64SourceStartTime, const uint64 ui64SourceEndTime, const uint64 ui64TimeShift)
{
	uint64 l_ui64StimulationCount=rSourceStimulationSet.getStimulationCount();
	for(uint64 i=0; i<l_ui64StimulationCount; i++)
	{
		uint64 l_ui64StimulationDate=rSourceStimulationSet.getStimulationDate(i);
		if(ui64SourceStartTime <= l_ui64StimulationDate && l_ui64StimulationDate < ui64SourceEndTime)
		{
			rDestinationStimulationSet.appendStimulation(
				rSourceStimulationSet.getStimulationIdentifier(i),
				rSourceStimulationSet.getStimulationDate(i)+ui64TimeShift,
				rSourceStimulationSet.getStimulationDuration(i));
		}
	}
	return true;
}

boolean OpenViBEToolkit::Tools::StimulationSet::removeRange(IStimulationSet& rStimulationSet, const uint64 ui64StartTime, const uint64 ui64EndTime)
{
	for(uint64 i=0; i<rStimulationSet.getStimulationCount(); i++)
	{
		uint64 l_ui64StimulationDate=rStimulationSet.getStimulationDate(i);
		if(ui64StartTime <= l_ui64StimulationDate && l_ui64StimulationDate < ui64EndTime)
		{
			rStimulationSet.removeStimulation(i--);
		}
	}
	return true;
}
