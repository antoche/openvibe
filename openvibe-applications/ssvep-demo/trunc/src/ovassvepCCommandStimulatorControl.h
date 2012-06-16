#ifndef __OpenViBEApplication_CCommandStimulatorControl_H__
#define __OpenViBEApplication_CCommandStimulatorControl_H__

#include "ovassvepICommandVRPNButton.h"

namespace OpenViBESSVEP
{
	class CCommandStimulatorControl : public ICommandVRPNButton
	{
		public:
			CCommandStimulatorControl(CApplication* poApplication);
			~CCommandStimulatorControl() {};

			void execute(int iButton, int iState);
	};
}


#endif // __OpenViBEApplication_CCommandStimulatorControl_H__
