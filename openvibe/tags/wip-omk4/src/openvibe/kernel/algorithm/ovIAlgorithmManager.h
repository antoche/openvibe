#ifndef __OpenViBE_Kernel_Algorithm_IAlgorithmManager_H__
#define __OpenViBE_Kernel_Algorithm_IAlgorithmManager_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IAlgorithmProxy;

		/**
		 * \class IAlgorithmManager
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-11-06
		 * \brief Manager for all kind of plugin algorithms
		 * \ingroup Group_Algorithm
		 * \ingroup Group_Kernel
		 * \sa OpenViBE::Plugins::IAlgorithm
		 */
		class OV_API IAlgorithmManager : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
			 * \brief Creates a new algorithm
			 * \param rAlgorithmClassIdentifier [out] : the class identifier of
			 *        the newly created algorithm
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::CIdentifier createAlgorithm(
				const OpenViBE::CIdentifier& rAlgorithmClassIdentifier)=0;
			/**
			 * \brief Releases an existing algorithm
			 * \param rAlgorithmIdentifier [in] : the existing algorithm identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean releaseAlgorithm(
				const OpenViBE::CIdentifier& rAlgorithmIdentifier)=0;
			/**
			 * \brief Releases an existing algorithm
			 * \param rAlgorithm [in] : the existing algorithm
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean releaseAlgorithm(
				OpenViBE::Kernel::IAlgorithmProxy& rAlgorithm)=0;
			/**
			 * \brief Gets details on a specific algorithm
			 * \param rAlgorithmIdentifier [in] : the algorithm identifier which details should be returned
			 * \return the corresponding algorithm reference.
			 * \warning Calling this function with a bad identifier causes a crash
			 */
			virtual OpenViBE::Kernel::IAlgorithmProxy& getAlgorithm(
				const OpenViBE::CIdentifier& rAlgorithmIdentifier)=0;
			/**
			 * \brief Gets next algorithm identifier
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding algorithm
			 * \return The identifier of the next algorithm in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first algorithm
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextAlgorithmIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Algorithm_AlgorithmManager);
		};
	};
};

#endif // __OpenViBE_Kernel_Algorithm_IAlgorithmManager_H__
