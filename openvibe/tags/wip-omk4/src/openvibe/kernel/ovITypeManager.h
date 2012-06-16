#ifndef __OpenViBE_Kernel_ITypeManager_H__
#define __OpenViBE_Kernel_ITypeManager_H__

#include "ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class ITypeManager
		 * \brief Type manager, provides information on platform's handled types, parameters, streams etc...
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-03-14
		 *
		 * This manager concentrates all information about types in the §OpenViBE§
		 * platform. Any non-standard type should be declared in the type identifier
		 * by the kernel or the plugins in order to be used. Registered types range
		 * from box settings to streams, eventually including inter-type auto casts.
		 *
		 * See the different functions for more details.
		 */
		class OV_API ITypeManager : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/** \name Type registration */
			//@{

			/**
			 * \brief Gets next type identifier
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding type
			 * \return The identifier of the next type in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first type
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextTypeIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const=0;
			/**
			 * \brief Registers a new simple type of data
			 * \param rTypeIdentifier [in] : the identifier for this type
			 * \param sTypeName [in] : the name for this type
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean registerType(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sTypeName)=0;
			/**
			 * \brief Registers a new communication stream type
			 * \param rTypeIdentifier [in] : the identifier for this type
			 * \param sTypeName [in] : the name for this type
			 * \param rParentTypeIdentifier [in] : the parent stream type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Stream types are organized as a hierarchy, meaning that any
			 * derived stream should at least contain the parent stream content,
			 * plus other informations. This will be used by applications in order
			 * to determine what stream is compatible with what other stream and
			 * to allow box connections or not.
			 */
			virtual OpenViBE::boolean registerStreamType(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sTypeName,
				const OpenViBE::CIdentifier& rParentTypeIdentifier)=0;
			/**
			 * \brief Registers a new enumeration type
			 * \param rTypeIdentifier [in] : the type identifier for this type
			 * \param sTypeName [in] : the name for this type
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \sa registerEnumerationEntry
			 *
			 * An enumeration should have several possible values.
			 * This values have to be created thanks to \c registerEnumerationEntry
			 */
			virtual OpenViBE::boolean registerEnumerationType(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sTypeName)=0;
			/**
			 * \brief Registers a new enumeration value for a given enumeration type
			 * \param rTypeIdentifier [in] : the type identifier of the enumeration which new entry has to be registered
			 * \param sEntryName [in] : the name of the entry to register
			 * \param ui64EntryValue [in] : the value of the entry to register
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note the enumeration has to be registered
			 * \sa registerEnumerationType
			 */
			virtual OpenViBE::boolean registerEnumerationEntry(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sEntryName,
				const OpenViBE::uint64 ui64EntryValue)=0;
			/**
			 * \brief Registers a new bitmask type
			 * \param rTypeIdentifier [in] : the type identifier for this type
			 * \param sTypeName [in] : the name for this type
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \sa registerBitMaskEntry
			 *
			 * A bitmask should have several possible values.
			 * This values have to be created thanks to \c registerBitMaskEntry
			 */
			virtual OpenViBE::boolean registerBitMaskType(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sTypeName)=0;
			/**
			 * \brief Registers a new bitmask value for a given bitmask type
			 * \param rTypeIdentifier [in] : the type identifier of the bitmask which new entry has to be registered
			 * \param sEntryName [in] : the name of the entry to register
			 * \param ui64EntryValue [in] : the value of the entry to register
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note the bitmask has to be registered
			 * \sa registerBitMaskType
			 */
			virtual OpenViBE::boolean registerBitMaskEntry(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sEntryName,
				const OpenViBE::uint64 ui64EntryValue)=0;

			//@}
			/** \name Registration verification */
			//@{

			/**
			 * \brief Tests if a specific type has been registered
			 * \param rTypeIdentifier [in] : the type identifier which registration has to be tested
			 * \return \e true if the specified type has been registered.
			 * \return \e false if the specified type has not been registered.
			 */
			virtual OpenViBE::boolean isRegistered(
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			/**
			 * \brief Tests if a specific type has been registered and is a stream
			 * \param rTypeIdentifier [in] : the type identifier which registration has to be tested
			 * \return \e true if the specified type has been registered is a stream.
			 * \return \e false if the specified type has not been registered or is not a stream.
			 */
			virtual OpenViBE::boolean isStream(
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			/**
			 * \brief Tests if a specific type has been registered and is an enumeration
			 * \param rTypeIdentifier [in] : the type identifier which registration has to be tested
			 * \return \e true if the specified type has been registered and is an enumeration.
			 * \return \e false if the specified type has not been registered or is not an enumeration.
			 */
			virtual OpenViBE::boolean isEnumeration(
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			/**
			 * \brief Tests if a specific type has been registered and is a bitmask
			 * \param rTypeIdentifier [in] : the type identifier which registration has to be tested
			 * \return \e true if the specified type has been registered and is a bitmask.
			 * \return \e false if the specified type has not been registered or is not a bitmask.
			 */
			virtual OpenViBE::boolean isBitMask(
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;

			//@}
			/** \name Type identification */
			//@{

			/**
			 * \brief Gets the name of a specified type
			 * \param rTypeIdentifier [in] : the type identifier which name should be returned
			 * \return the name of the speficied type.
			 */
			virtual OpenViBE::CString getTypeName(
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			/**
			 * \brief Gets the parent stream type for a stream type
			 * \param rTypeIdentifier [in] : the stream type identifier which parent stream type be returned
			 * \return the parent stream type on success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note The specified type identifier has to be a stream type.
			 */
			virtual OpenViBE::CIdentifier getStreamParentType(
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;

			//@}
			/** \name Enumeration entry accessors */
			//@{

			/**
			 * \brief Gets the number of enumeration entry for an enumeration type
			 * \param rTypeIdentifier [in] : the enumeration type identifier
			 * \return the number of entry for this enumeration type.
			 */
			virtual OpenViBE::uint64 getEnumerationEntryCount(
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			/**
			 * \brief Gets details for a specific enumeration type entry
			 * \param rTypeIdentifier [in] : the enumeration type identifier
			 * \param ui64EntryIndex [in] : the index of the entry which details should be returned
			 * \param sEntryName [out] : the name of the specified entry
			 * \param rEntryValue [out] : the value of the speficied entry
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean getEnumerationEntry(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::uint64 ui64EntryIndex,
				OpenViBE::CString& sEntryName,
				OpenViBE::uint64& rEntryValue)=0;
			/**
			 * \brief Converts an enumeration entry value to an enumeration entry name
			 * \param rTypeIdentifier [in] : the enumeration type identifier
			 * \param ui64EntryValue [in] : the enumeration entry value
			 * \return the enumeration entry name corresponding to the specified value.
			 * \warning on error, an empty string is returned.
			 */
			virtual OpenViBE::CString getEnumerationEntryNameFromValue(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::uint64 ui64EntryValue)=0;
			/**
			 * \brief Converts an enumeration entry name to an enumeration entry value
			 * \param rTypeIdentifier [in] : the enumeration type identifier
			 * \param rEntryName [in] : the enumeration entry name
			 * \return the enumeration entry value corresponding to the specified name.
			 * \warning on error, \c 0xffffffffffffffffLL is returned.
			 */
			virtual OpenViBE::uint64 getEnumerationEntryValueFromName(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& rEntryName)=0;

			//@}
			/** \name Bitmask entry accessors */
			//@{

			/**
			 * \brief Gets the number of bitmask entry for a bitmask type
			 * \param rTypeIdentifier [in] : the bitmask type identifier
			 * \return the number of entry for this bitmask type.
			 */
			virtual OpenViBE::uint64 getBitMaskEntryCount(
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			/**
			 * \brief Gets details for a specific bitmask type entry
			 * \param rTypeIdentifier [in] : the bitmask type identifier
			 * \param ui64EntryIndex [in] : the index of the entry which details should be returned
			 * \param sEntryName [out] : the name of the specified entry
			 * \param rEntryValue [out] : the value of the speficied entry
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean getBitMaskEntry(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::uint64 ui64EntryIndex,
				OpenViBE::CString& sEntryName,
				OpenViBE::uint64& rEntryValue)=0;
			/**
			 * \brief Converts a bitmask entry value to a bitmask entry name
			 * \param rTypeIdentifier [in] : the bitmask type identifier
			 * \param ui64EntryValue [in] : the bitmask entry value
			 * \return the bitmask entry name corresponding to the specified value.
			 * \warning on error, an empty string is returned.
			 */
			virtual OpenViBE::CString getBitMaskEntryNameFromValue(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::uint64 ui64EntryValue)=0;
			/**
			 * \brief Converts a bitmask entry name to a bitmask entry value
			 * \param rTypeIdentifier [in] : the bitmask type identifier
			 * \param rEntryName [in] : the bitmask entry name
			 * \return the bitmask entry value corresponding to the specified name.
			 * \warning on error, \c 0xffffffffffffffffLL is returned.
			 */
			virtual OpenViBE::uint64 getBitMaskEntryValueFromName(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& rEntryName)=0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_TypeManager);
		};
	};
};

#endif // __OpenViBE_Kernel_ITypeManager_H__
