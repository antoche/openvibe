#ifndef __OpenViBEApplication_CCommandReceiveTarget_H__
#define __OpenViBEApplication_CCommandReceiveTarget_H__

#include "../ovassvepICommandVRPNButton.h"

namespace OpenViBESSVEP
{
	class CTrainerApplication;

	class CCommandReceiveTarget : public ICommandVRPNButton
	{
		public:
			CCommandReceiveTarget(CApplication* poApplication);
			~CCommandReceiveTarget() {};

			void execute(int iButton, int iState);


	};
}


#endif // __OpenViBEApplication_CCommandReceiveTarget_H__
