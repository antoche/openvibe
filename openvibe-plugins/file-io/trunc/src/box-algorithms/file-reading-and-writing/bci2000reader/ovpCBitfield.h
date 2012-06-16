#ifndef BITFIELD_H
#define BITFIELD_H

#include <openvibe/ov_all.h>

#include <string>
#include <iostream>
#include <map>
#include <vector>

namespace BCI2000
{
	class CBitfield
	{
	protected:
		class CField
		{
		public:
			OpenViBE::int32   m_i32BytePosition; // starting byte
			OpenViBE::int32   m_i32BitPosition;  // starting bit within starting byte
			OpenViBE::int32   m_i32Length;       // field length (in bits)
			OpenViBE::CString m_sName;
			OpenViBE::int32   m_i32InitialValue; // initial value. Not updated when reading files
			CField(OpenViBE::int32 i32BytePosition, OpenViBE::int32 i32BitPosition, OpenViBE::int32 i32Length, OpenViBE::CString sName, OpenViBE::int32 i32InitialValue) :
				m_i32BytePosition(i32BytePosition),
				m_i32BitPosition(i32BitPosition),
				m_i32Length(i32Length),
				m_sName(sName),
				m_i32InitialValue(i32InitialValue)
			{}
		};

		std::vector<CField> m_vFields;
	public:
		// extract values from compressed data; returned array of size this->size() to
		// be deleted[] by the user.
		OpenViBE::uint32 * getFields(unsigned char * pData);

		// extract values from compressed data
		// extracted_fields must be allocated and of size this->size()
		void getFields(unsigned char * pData,OpenViBE::uint32 * pExtractedFields);

		// add a new field. Returns false when invalid paramaters entered, true otherwise.
		bool addField(OpenViBE::int32 i32BytePosition, OpenViBE::int32 i32BitPosition, OpenViBE::int32 i32Length, OpenViBE::CString sName, OpenViBE::int32 i32InitialValue);

		// returns the number of m_vFields in the structure
		// (not to be confused with the actual memory footprint)
		OpenViBE::uint32 size(void) const
		{
			return m_vFields.size();
		}
		const OpenViBE::CString & getFieldName(OpenViBE::uint32 ui32Index) const
		{
			return m_vFields[ui32Index].m_sName;
		}
		OpenViBE::int32 getInitialValue(OpenViBE::uint32 ui32Index) const
		{
			return m_vFields[ui32Index].m_i32InitialValue;
		}
	};
}

#endif
