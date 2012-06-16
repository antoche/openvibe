#ifndef __OpenViBE_Kernel_Log_ILogManager_h__
#define __OpenViBE_Kernel_Log_ILogManager_h__

#include "ovILogListener.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class ILogManager
		 * \brief Log manager
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-06-03
		 * \ingroup Group_Log
		 * \ingroup Group_Kernel
		 *
		 * The log manager is responsible for keeping a trace of all the messages
		 * the application could send as debug output. Such information is not
		 * usefull most of the cases but could become crucial in some cases.
		 * Thus there are different levels of activation for the log manager
		 * to work. The log manager forwards each log request to its registered
		 * log listeners that effectively do the log the way they want
		 * (be it a status window, a console, a file, whatever). See ILogListener
		 * for more details.
		 */
		class OV_API ILogManager : public OpenViBE::Kernel::ILogListener
		{
		public:

			/**
			 * \brief Registers a new log listener
			 * \param pListener [in] : the new listener to register
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean addListener(OpenViBE::Kernel::ILogListener* pListener)=0;
			/**
			 * \brief Removes a registered listener
			 * \param pListener [in] : the listener to unregister
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean removeListener(OpenViBE::Kernel::ILogListener* pListener)=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Log_LogManager);
		};

		/**
		 * \brief Stream output operator
		 * \param rLogManager [in] : the log manager that takes the object
		 * \param rObject [in] : the object to log
		 * \return The log manager itself
		 * \sa OpenViBE::Kernel::ILogManager
		 *
		 * This function helps in logging different objects thanks to the
		 * stream operator. The log manager can almost be used as any std
		 * ostream object.
		 */
		template <class T> OpenViBE::Kernel::ILogManager& operator << (OpenViBE::Kernel::ILogManager& rLogManager, const T& rObject)
		{
			rLogManager.log(rObject);
			return rLogManager;
		}
	};
};

#endif // __OpenViBE_Kernel_Log_ILogManager_h__
