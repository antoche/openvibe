#ifndef __OpenViBEApplication_CCommandStartStop_H__
#define __OpenViBEApplication_CCommandStartStop_H__


#include "ovassvepICommandOIS.h"

namespace OpenViBESSVEP
{
	class CVRPNServer;

	class CCommandStartStop : public ICommandOIS
	{
		public:
			CCommandStartStop(CApplication* poApplication);
			~CCommandStartStop();

			void processFrame();

			void receiveKeyPressedEvent( const OIS::KeyCode oKey );
			void receiveKeyReleasedEvent( const OIS::KeyCode oKey );

		private:
			CVRPNServer* m_poVRPNServer;



	};
}


#endif // __OpenViBEApplication_CCommandStartStop_H__
