#ifndef __OpenViBEKernel_Kernel_Player_COgreResourceGroup_H__
#define __OpenViBEKernel_Kernel_Player_COgreResourceGroup_H__

#include <openvibe/ov_all.h>

#include <Ogre.h>
/*
#include "ov_types.h"
#include "ovCString.h"
#include "ovCIdentifier.h"
*/
/**
 * \class COgreResourceGroup
 * \author Vincent Delannoy (INRIA/IRISA)
 * \date 2008-06
 * \brief Ogre Resource Group management
 * This class handles a resource group, possibly shared by multiple
 * scenarios. Its reference count keeps track of when it is actually
 * needed. Resources are freed when the count reaches 0.
 */
class COgreResourceGroup
{
public:
	/**
	 * \brief Constructor
	 * Create a resource group names 'name'
	 * \param[in] rKernelContext OpenViBE kernel context
	 * \param[in] rName name of this resource group
	 */
	COgreResourceGroup(
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		const std::string& rName);

	/**
	 * \brief Destructor
	 */
	~COgreResourceGroup();

	/**
	 * \brief Get resource group name
	 */
	const std::string& getName();

	/**
	 * \brief Add a resource location
	 * \remarks Resources can't be added once Resource Group has been initialized
	 * \param rPath Resources location path
	 * \param type Resources type (path or zip file)
	 * \param bRecursive Resources are added recursively from location passed in parameter if true
	 * \return True if location could be added, false otherwise
	 */
	bool addResourceLocation(
		const std::string& rPath,
		const std::string& rType,
		bool bRecursive);

	/**
	 * \brief Initialize resource group
	 * \remarks This increments the resource group internal reference counter
	 * \return True if this group was effectively intialised (i.e. upon first call to this method), false otherwise
	 */
	bool initialize();

	/**
	 * \brief Destroy resource group
	 * \remarks This decrements the resource group internal reference counter
	 * \return True if this group was effectively destroyed (i.e. its refcount reached 0), false otherwise
	 */
	bool destroy();
	//@}

private:
	/// OpenViBE kernel context
	const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
	///Resource group name
	std::string m_sName;
	///Resource group reference counter (incremented when initialised and decremented when destroyed)
	unsigned int m_ui32RefCount;
	///Intialisation flag -only when this flag is false does a call to initialize() actually initialises the resource group
	bool m_bInitialized;
	//std::map<CString> m_oResourceLocations;
};

#endif // __OpenViBEKernel_Kernel_Player_COgreResourceGroup_H__
