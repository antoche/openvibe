#include "ovkMaterialAction.h"

using namespace Ogre;
using namespace OpenViBE::Kernel;

//===========================================================================
// MaterialPassAction
//===========================================================================
void IMaterialPassAction::subEntityAction( SubEntityMaterial* subEntity ) const
{
  if( !subEntity->getMaterial().isNull() )
  {
    unsigned short i = 0 ;
    for( Material::TechniqueIterator techniqueIt = subEntity->getMaterial()->getTechniqueIterator() ;
         techniqueIt.hasMoreElements() ;
         techniqueIt.moveNext(), ++i )
    {
      Technique* techniqueScr = subEntity->getMaterial()->getTechnique( i ) ;
      unsigned short j = 0;
      for( Technique::PassIterator passIt = techniqueIt.peekNext()->getPassIterator ();
           passIt.hasMoreElements() ;
           passIt.moveNext(), ++j )
      {
        passAction( subEntity, passIt.peekNext(), techniqueScr->getPass( j ) ) ;
      }
    }
  }
}
//===========================================================================

//===========================================================================
// MaterialSetMaterialName
//===========================================================================
void MaterialSetMaterialName::subEntityAction( SubEntityMaterial* subEntity ) const
{
  subEntity->activateMaterial( _materialName ) ;
}
//===========================================================================

//===========================================================================
// MaterialSetSceneBlending
//===========================================================================
void MaterialSetSceneBlending::passAction( SubEntityMaterial* subEntity,
                                           Pass* passDest, Pass* passSrc ) const
{
  passDest->setSceneBlending( _blendType );
}
//===========================================================================

//===========================================================================
// MaterialSetTransparency
//===========================================================================
MaterialSetTransparency::MaterialSetTransparency( const IKernelContext& rKernelContext, Ogre::Real transparency )
: IMaterialPassAction(rKernelContext),
_transparency( transparency < 0.0f ? 0.0 : ( 1.0f < transparency ? 1.0f : transparency ) )
{
}
//---------------------------------------------------------------------------
bool MaterialSetTransparency::entityAction( EntityMaterial* entity ) const
{
  entity->getOgreEntity()->setVisible( 1.0f != _transparency ) ;
  return true ;
}
//---------------------------------------------------------------------------
void MaterialSetTransparency::passAction( SubEntityMaterial* subEntity,
                                          Pass* passDest, Pass* passSrc ) const
{
  ColourValue ac = passSrc->getAmbient () ;
  ColourValue dc = passSrc->getDiffuse () ;
  ColourValue sc = passSrc->getSpecular() ;

  ac.a = _transparency ;
  dc.a = _transparency ;
  sc.a = _transparency ;

  passDest->setDiffuse ( dc );
  passDest->setAmbient ( ac );
  passDest->setSpecular( sc );
}
//===========================================================================

//===========================================================================
// MaterialSetDiffuse
//===========================================================================
void MaterialSetDiffuse::passAction( SubEntityMaterial* subEntity,
                                          Pass* passDest, Pass* passSrc ) const
{
	if(_bUseShader == false)
	{
		passDest->setDiffuse ( _diffuse );
	}
}

void MaterialSetDiffuse::subEntityAction( SubEntityMaterial* subEntity ) const
{
	if(_bUseShader == true)
	{
#define COLOR_MODULATE_PARAM_INDEX 1
		subEntity->getOgreSubEntity()->setCustomParameter(COLOR_MODULATE_PARAM_INDEX, Ogre::Vector4(_diffuse.r, _diffuse.g, _diffuse.b, _diffuse.a));
	}
	else
	{
		//call passAction for each pass
		IMaterialPassAction::subEntityAction(subEntity);
	}
}
//===========================================================================

//===========================================================================
// MaterialSetAwareness
//===========================================================================
MaterialVertexBuffer::MaterialVertexBuffer(const IKernelContext& rKernelContext) :
	IMaterialAction(rKernelContext),
/*	m_ui32VertexPositionCount(0),
	m_pVertexPositionArray(NULL),*/
	m_ui32VertexColorCount(0),
	m_pVertexColorArray(NULL)
{
}

MaterialVertexBuffer::MaterialVertexBuffer(const OpenViBE::Kernel::IKernelContext& rKernelContext,
	/*size_t ui32VertexPositionCount, const float* pVertexPositionArray, */
	size_t ui32VertexColorCount, const float* pVertexColorArray) :
	IMaterialAction(rKernelContext),
/*m_ui32VertexPositionCount(ui32VertexPositionCount),
	m_pVertexPositionArray(pVertexPositionArray),*/
	m_ui32VertexColorCount(ui32VertexColorCount),
	m_pVertexColorArray(pVertexColorArray)
{
}

MaterialVertexBuffer::~MaterialVertexBuffer()
{
}
/*
void MaterialVertexBuffer::setVertexPositionArray(size_t ui32VertexPositionCount, float* pVertexPositionArray)
{
	m_ui32VertexPositionCount = ui32VertexPositionCount;
	m_pVertexPositionArray = pVertexPositionArray;
}*/

void MaterialVertexBuffer::setVertexColorArray(size_t ui32VertexColorCount, const float* pVertexColorArray)
{
	m_ui32VertexColorCount = ui32VertexColorCount;
	m_pVertexColorArray = pVertexColorArray;
}

bool MaterialVertexBuffer::entityAction(EntityMaterial* entity) const
{
	setSubMeshColors(entity, 0, m_ui32VertexColorCount, m_pVertexColorArray);
	return true;
}

bool MaterialVertexBuffer::setSubMeshColors(
	EntityMaterial* entity,
	unsigned short mesh_index,
	size_t ui32VertexColorCount,
	const float* pVertexColorArray) const
{
	if(entity->getOgreEntity() == NULL)
	{
		m_rKernelContext.getLogManager() << LogLevel_Error << "Can't set colors : no Entity found!\n";
		return false;
	}

	Ogre::Mesh* mesh = entity->getOgreEntity()->getMesh().get();

	if(mesh == NULL || mesh_index >= mesh->getNumSubMeshes())
	{
		m_rKernelContext.getLogManager() << LogLevel_Error << "Can't set colors : no Mesh found!\n";
		return false;
	}

	Ogre::SubMesh* submesh = mesh->getSubMesh( mesh_index );

	if(submesh->useSharedVertices)
	{
		m_rKernelContext.getLogManager() << LogLevel_Error << "Can't set colors : vertices are shared and thus not accessible from SubMesh!\n";
		return false;
	}
	else
	{
		if(ui32VertexColorCount != submesh->vertexData->vertexCount)
		{
			m_rKernelContext.getLogManager() << LogLevel_Error << "Can't set colors : vertex count mismatch!\n";
			return false;
		}

		//get pointer to submesh vertex data
		Ogre::VertexData* vertex_data = submesh->vertexData;

		//get pointer to DIFFUSE element
		const Ogre::VertexElement* difElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_DIFFUSE);

		if(difElem == NULL)
		{
			m_rKernelContext.getLogManager() << LogLevel_Error << "Can't set colors : no colours_diffuse element found in vertex buffer!\n";
			return false;
		}

		//convert color to current RenderSystem's format
		Ogre::VertexElementType type = difElem->getType();

		//retrieve VB for DIFFUSE element
		Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(difElem->getSource());

		//lock VB for reading
		unsigned char* color = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_NORMAL));

		// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
		//  as second argument. So make it float, to avoid trouble when Ogre::Real will
		//  be comiled/typedefed as double:
		//      Ogre::Real* pReal;
		Ogre::RGBA* pRGBA;
		Ogre::ColourValue colour;

		for( size_t j = 0; j < vertex_data->vertexCount; ++j, color += vbuf->getVertexSize())
		{
			//get pointer to RGBA DIFFUSE data
			difElem->baseVertexPointerToElement(color, &pRGBA);

			colour.r = pVertexColorArray[4*j];
			colour.g = pVertexColorArray[4*j+1];
			colour.b = pVertexColorArray[4*j+2];
			colour.a = pVertexColorArray[4*j+3];

			//convert color from RGBA floats to a single ARGB uint32
			if(type == Ogre::VET_COLOUR_ARGB) //D3D format
			{
				*pRGBA = colour.getAsARGB();
			}
			else /*if type == OGRE::VET_COLOUR_ABGR)*/ //GL format
			{
				*pRGBA = colour.getAsABGR();
			}
		}

		//unlock VB
		vbuf->unlock();
	}

	return true;
}

//===========================================================================

//===========================================================================
// MaterialSetAwareness
//===========================================================================
ColourValue MaterialSetAwareness::computeBlend( const ColourValue& currentColor ) const
{
  return ColourValue( computeBlend( currentColor.r, _awarenessColor.x ),
                      computeBlend( currentColor.g, _awarenessColor.y ),
                      computeBlend( currentColor.b, _awarenessColor.z ),
                      computeBlend( currentColor.a, 1.0f              ) ) ;
}
//---------------------------------------------------------------------------
void MaterialSetAwareness::passAction( SubEntityMaterial* subEntity,
                                       Pass* passDest, Pass* passSrc ) const
{
  passDest->setDiffuse ( computeBlend( passSrc->getAmbient () ) ) ;
  passDest->setAmbient ( computeBlend( passSrc->getDiffuse () ) ) ;
  passDest->setSpecular( computeBlend( passSrc->getSpecular() ) ) ;
  //force reload
  passDest->_load() ;
}
//===========================================================================

//===========================================================================
// IsProgrammable
//===========================================================================
//TODO handle multi pass
void IsProgrammable::subEntityAction( SubEntityMaterial* subEntity ) const
{
  if( !_isProgrammable )
  {
    _isProgrammable = getFirstPassOfFirstTechnique( subEntity )->isProgrammable() ;
  }
}
//===========================================================================

//===========================================================================
// MaterialSetShader
//===========================================================================
void MaterialSetShader::subEntityAction( SubEntityMaterial* subEntity ) const
{
  Pass * pass = getFirstPassOfFirstTechnique( subEntity ) ;

  pass->setLightingEnabled( _light );

  if ( "null" != _texture )
  {
     pass->createTextureUnitState( _texture );
     //TextureUnitState *texState = pass->createTextureUnitState( texture ) ;
     //texState->setTextureScroll( 0.f, 0.f ) ;
     //texState->setColourOperation( Ogre::LBO_MODULATE ) ;

     //texState->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
  }
  if ( "null" != _fragment)
  { //fragment Program
    pass->setFragmentProgram( _fragment );
    //copy params passed
    Ogre::GpuProgramParametersSharedPtr fragmentParams = pass->getFragmentProgramParameters() ;
    fragmentParams->copyConstantsFrom( *_fragmentPrm ) ;
  }
  if ( "null" != _vertex)
  { //vertex Program
    pass->setVertexProgram( _vertex );
    //copy params passed
    Ogre::GpuProgramParametersSharedPtr vertexParams = pass->getVertexProgramParameters() ;
    vertexParams->copyConstantsFrom( *_vertexPrm ) ;
  }
  pass->_load() ;
}

//===========================================================================
