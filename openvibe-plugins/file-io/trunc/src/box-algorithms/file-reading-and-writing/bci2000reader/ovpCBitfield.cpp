#include "ovpCBitfield.h"

using namespace OpenViBE;
using namespace BCI2000;

uint32 * CBitfield::getFields(unsigned char * pData)
{
	uint32 l_ui32NbFields=m_vFields.size();
	uint32 * pField=new uint32[l_ui32NbFields];
	getFields(pData,pField);
	return pField;
}

void CBitfield::getFields(unsigned char * pData, uint32 * pExtractedField)
{
	uint32 l_ui32NbFields=m_vFields.size();
	for (uint32 i=0; i<l_ui32NbFields; i++)
	{
		int32 l_i32Space=(m_vFields[i].m_i32Length-1+m_vFields[i].m_i32BitPosition)/8+1;
		unsigned char * l_pPointer=pData+m_vFields[i].m_i32BytePosition;
		uint32 l_ui32Field=0;

		l_ui32Field=(*l_pPointer)>>m_vFields[i].m_i32BitPosition;

		for (uint32 j=1; j<l_i32Space; j++)
		{
			l_pPointer++;
			l_ui32Field+=((uint32)(*l_pPointer)) << (j*8-m_vFields[i].m_i32BitPosition);
		}

		l_ui32Field&=0xFFFFFFFF>>(32-m_vFields[i].m_i32Length);
		pExtractedField[i]=l_ui32Field;
	}
}

bool CBitfield::addField(int32 i32BytePosition, int32 i32BitPosition, int32 i32Length, CString sName, int32 i32InitialValue)
{
	if (i32Length>32) return false; // doc says len is 32 bits max
	if (i32BitPosition>7) return false;
	m_vFields.push_back(CField(i32BytePosition,i32BitPosition,i32Length,sName,i32InitialValue));
	return true;
}
