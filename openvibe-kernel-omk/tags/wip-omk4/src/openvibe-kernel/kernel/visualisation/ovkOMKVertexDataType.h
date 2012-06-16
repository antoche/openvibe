#ifndef OMKVERTEXDATATYPE_H
#define OMKVERTEXDATATYPE_H

#include "OMKSimpleTypeT.inl"
#include "ovkOMKVertexData.h"
#include "OMKAttribute.h"

namespace OMK 
{
  namespace Type 
  {

//-----------------------------------------------------------------------------
// Type def for easy use
typedef SimpleTypeT< VertexData > VertexDataType ;

//-----------------------------------------------------------------------------
// Specific implementation
//template<> inline PolatorNT* VertexDataType::createPolator() { return new Polator< VertexDataType >() ; }

//-----------------------------------------------------------------------------
// Declaration for instantiation
template class /*OMK_API*/ SimpleTypeT< VertexData > ;

  } // namespace Type
} // namespace OMK

#endif // defined OMKVERTEXDATATYPE_H
