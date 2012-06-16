#include "ovkOMKVertexBufferAnimator.h"

#include "OMKOgreVis.h"
#include "OMKOgreObject.h"
#include "OgreMovableObject.h"
#include "OMKParametersAccessor.inl"
#include "OMKTracer.h"

using namespace OpenViBE;
using namespace OMK ;
using namespace OMK::Vis ;
using namespace OMK::Type ;
using std::string;
using Ogre::SceneNode;
using Ogre::MovableObject;
using Ogre::Entity;

REGISTER_ANIMATOR_FACTORY( OMKVertexBufferAnimator, "OMKVertexBufferAnimator" ) ;

//-------------------------------------------------------------------------
// constructor
//-------------------------------------------------------------------------
OMKVertexBufferAnimator::OMKVertexBufferAnimator(  VisualObject& visualObject,
                                      const OMKName& id,
                                      const OMKConfigurationParameterDescriptor& node )
  :OgreAnimatorT< VertexDataType >( visualObject, id, node )
#if defined UPDATED_OMK
  ,_nmi ( NULL)
#endif
  /*,_color (1.f,1.f,1.f,1.f)//Opaque white*/
{
  init( node );
}

//-------------------------------------------------------------------------
// destructor
//-------------------------------------------------------------------------
OMKVertexBufferAnimator::~OMKVertexBufferAnimator()
{
  OMTRACEID ("OMK::OMKVertexBufferAnimator" ,"On Destructeur d'Animator");

#if defined UPDATED_OMK
  delete _nmi;
#endif
}

//-------------------------------------------------------------------------
// init
//-------------------------------------------------------------------------
void OMKVertexBufferAnimator::init( const OMKConfigurationParameterDescriptor& node)
{
	//create node material instance
	_anim = dynamic_cast< Ogre::SceneNode *> (&getNode());
  if(_anim == NULL)
	{
		//log << LogLevel_Warning < "";
		return;
	}
#if defined UPDATED_OMK
  _nmi = new NodeMaterialInstance ( _anim );
#endif
}

#if defined UPDATED_OMK
//---------------------------------------------------------------------------
///\brief Action to access Vertex Buffer.
class OVMaterialVertexBuffer : public IEntityMaterialAction
{
public:
  ///\brief The contructor stores the arguments.
	OVMaterialVertexBuffer( )
		: m_ui32VertexPositionCount(0), m_pVertexPositionArray(NULL),
		m_ui32VertexColorCount(0), m_pVertexColorArray(NULL)
  {}

	OVMaterialVertexBuffer( uint32 ui32VertexPositionCount, const float32* pVertexPositionArray, uint32 ui32VertexColorCount, const float32* pVertexColorArray)
		: m_ui32VertexPositionCount(ui32VertexPositionCount), m_pVertexPositionArray(pVertexPositionArray),
		m_ui32VertexColorCount(ui32VertexColorCount), m_pVertexColorArray(pVertexColorArray)
  {}

	void setVertexPositionArray(uint32 ui32VertexPositionCount, float32* pVertexPositionArray)
	{
		m_ui32VertexPositionCount = ui32VertexPositionCount;
		m_pVertexPositionArray = pVertexPositionArray;
	}

	void setVertexColorArray(uint32 ui32VertexColorCount, float32* pVertexColorArray)
	{
		m_ui32VertexColorCount = ui32VertexColorCount;
		m_pVertexColorArray = pVertexColorArray;
	}

	///\brief Destructor.
	virtual ~OVMaterialVertexBuffer() {}

private:
  /// @name Arguments
  //@{
	uint32 m_ui32VertexPositionCount;
	const float32* m_pVertexPositionArray;
	uint32 m_ui32VertexColorCount;
	const float32* m_pVertexColorArray;
  //@}

  virtual void entityAction( EntityMaterialInstance* entity )
  {
		setSubMeshColors(entity, 0, m_ui32VertexColorCount, m_pVertexColorArray);
  }

	//virtual void subEntityAction( SubEntityMaterialInstance* subEntity ) {}

	bool setSubMeshColors( EntityMaterialInstance* entity, size_t mesh_index, uint32 ui32VertexColorCount, const float32* pVertexColorArray )
	{
		if(entity->getEntity() == NULL)
		{
			//log << "Can't set colors : no Entity found!";
			return false;
		}

		Ogre::Mesh* mesh = entity->getEntity()->getMesh().get();

		if(mesh == NULL || mesh_index >= mesh->getNumSubMeshes())
		{
			//log << "Can't set colors : no Mesh found!";
			return false;
		}

		Ogre::SubMesh* submesh = mesh->getSubMesh( mesh_index );

		if(submesh->useSharedVertices)
		{
			//log << "Can't set colors : vertices are shared and thus not accessible from SubMesh!";
			return false;
		}
		else
		{
			if(ui32VertexColorCount != submesh->vertexData->vertexCount)
				return false;

			//get pointer to submesh vertex data
			Ogre::VertexData* vertex_data = submesh->vertexData;

			//get pointer to DIFFUSE element
			const Ogre::VertexElement* difElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_DIFFUSE);

			if(difElem == NULL)
			{
				//log << "Can't set colors : no colours_diffuse element found in vertex buffer!";
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
} ;
#endif

//-------------------------------------------------------------------------
// selfProcessVis
//-------------------------------------------------------------------------
void OMKVertexBufferAnimator::selfProcessVis(const VertexDataType& rVertexDataPrm)
{
	const VertexData& l_rVertexData( rVertexDataPrm.getValue() );
	//OMTRACEID("OMK::OMKVertexBufferAnimator"," got " << value );

	uint32 posCount, colCount;
	const float32* posArray;
	const float32* colArray;
	l_rVertexData.getVertexPositionArray(posCount, posArray);
	l_rVertexData.getVertexColorArray(colCount, colArray);

#if defined UPDATED_OMK
	_nmi->doAction(OVMaterialVertexBuffer(posCount, posArray, colCount, colArray));
#endif
}
