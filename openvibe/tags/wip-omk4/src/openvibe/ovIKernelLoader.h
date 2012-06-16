#ifndef __OpenViBE_IKernelLoader_H__
#define __OpenViBE_IKernelLoader_H__

#include "ovIObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IKernelDesc;
	};

	/**
	 * \class IKernelLoader
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2006-09-26
	 * \brief This class allows an §OpenViBE§ application to load a kernel module.
	 *
	 * This class allows an §OpenViBE§ application to load a kernel module.
	 * The application should first load the DLL/so file and initialize it.
	 * Then it is able to get a kernel descriptor and to build its own kernel
	 * to use. The kernel DLL/so file should be freed thanks to the unintialization
	 * and unload methods.
	 *
	 * \sa OpenViBE::Kernel::IKernel
	 * \sa OpenViBE::Kernel::IKernelDesc
	 *
	 */
	class OV_API IKernelLoader : public OpenViBE::IObject
	{
	public:

		/**
		 * \brief Loads a kernel DLL/so file
		 * \param sFileName [in] : the filename to load
		 * \param pError [out] : an optional error string to get when loading fails
		 * \return \e true on success.
		 * \return \e false on error.
		 */
		virtual OpenViBE::boolean load(
			const OpenViBE::CString& sFileName,
			OpenViBE::CString* pError=NULL)=0;
		/**
		 * \brief Unloads a loaded kernel DLL/so file
		 * \return \e true on success.
		 * \return \e false on error.
		 * \warning \c load must have been called successfully before calling \c unload
		 */
		virtual OpenViBE::boolean unload(
			CString* pError=NULL)=0;
		/**
		 * \brief Requests the kernel DLL/so file to self initialize
		 * \return \e true on success.
		 * \return \e false on error.
		 * \warning \c load must have been called successfully before calling \c initialize
		 *
		 * Calling this function results in calling the \c onInitialize global function of
		 * the kernel DLL/so file. See section \ref Doc_CreatingNewKernel to get a full description
		 * of how a kernel is loaded / used.
		 */
		virtual OpenViBE::boolean initialize(void)=0;
		/**
		 * \brief Gets the kernel description of the loaded kernel DLL/so file
		 * \param rpKernelDesc [out] : a pointer to the kernel description
		 * \return \e true on success.
		 * \return \e false on error.
		 * \warning both \c load and \c initialize must have been called successfully before calling \c getKernelDesc
		 */
		virtual OpenViBE::boolean getKernelDesc(
			OpenViBE::Kernel::IKernelDesc*& rpKernelDesc)=0;
		/**
		 * \brief Uninitializes a loaded and initialized kernel DLL/so file
		 * \return \e true on success.
		 * \return \e false on error.
		 * \warning both \c load and \c initialize must have been called successfully before calling \c getKernelDesc
		 */
		virtual OpenViBE::boolean uninitialize(void)=0;
		/**
		 * \brief Releases this kernel loader
		 *
		 * This function notifies this kernel loader it won't ever be used. At this time,
		 * the kernel loader can release any allocated memory or resource.
		 *
		 * \sa OpenViBE::createKernelLoader
		 */
		virtual void release(void)=0;

		_IsDerivedFromClass_(OpenViBE::IObject, OV_ClassId_KernelLoader)
	};

	/**
	 * \brief Creates a new kernel loader
	 * \return a new kernel loader
	 * \sa OpenViBE::IKernelLoader
	 */
	extern OV_API OpenViBE::IKernelLoader* createKernelLoader(void);
};

#endif // __OpenViBE_IKernelLoader_H__
