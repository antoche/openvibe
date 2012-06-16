#ifndef __OpenViBEKernel_Kernel_Player_MaterialAction_H__
#define __OpenViBEKernel_Kernel_Player_MaterialAction_H__

#include "ovkCMaterial.h"

//===========================================================================
///\brief A class which calls the action for each pass of each technique.
class IMaterialPassAction : public IMaterialAction
{
public:
  ///\brief The contructor.
	IMaterialPassAction(
		const OpenViBE::Kernel::IKernelContext& rKernelContext) :
		IMaterialAction(rKernelContext)
	{}
  ///\brief Destructor.
  virtual ~IMaterialPassAction()
	{}
protected:
  ///\brief The action calls each pass of each technique.
  virtual void subEntityAction(
		SubEntityMaterial* subEntity) const;
  ///\brief The action for each pass of each technique.
  virtual void passAction(
		SubEntityMaterial* subEntity,
		Ogre::Pass* passDest,
		Ogre::Pass* passSrc) const=0;
};

//===========================================================================
///\brief .
class MaterialSetMaterialName : public IMaterialAction
{
public:
  ///\brief The contructor stores the argument.
  MaterialSetMaterialName(
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		const Ogre::String& materialName) :
		IMaterialAction(rKernelContext),
		_materialName(materialName)
	{}
  ///\brief Destructor.
  virtual ~MaterialSetMaterialName()
	{}
protected:
  ///\brief The argument.
  /// It is given to the function.
  Ogre::String _materialName;
private:
  ///\brief The action calls each pass of each technique.
  virtual void subEntityAction(
		SubEntityMaterial* subEntity) const;
};

//===========================================================================
///\brief Action to set the scene blending.
/// Sets the blending method to use to adjust transparency.
/// @param blendType The SceneBlendType desired.
class MaterialSetSceneBlending : public IMaterialPassAction
{
public:
  ///\brief The contructor stores the argument.
  MaterialSetSceneBlending(
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		Ogre::SceneBlendType blendType) :
		IMaterialPassAction(rKernelContext),
		_blendType(blendType)
	{}
  ///\brief Destructor.
  virtual ~MaterialSetSceneBlending()
	{}
protected:
  ///\brief The argument.
  /// It is given to the function.
  Ogre::SceneBlendType _blendType;
private:
  ///\brief The action for each pass of each technique.
  virtual void passAction(
		SubEntityMaterial* subEntity,
		Ogre::Pass* passDest,
		Ogre::Pass* passSrc) const;
};

//===========================================================================
///\brief Action to set the diffuse color.
/// Sets the color
/// @param diffuse The diffuse color desired.
class MaterialSetDiffuse : public IMaterialPassAction
{
public:
	MaterialSetDiffuse(
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		Ogre::ColourValue diffuse,
		bool bUseShader) :
		IMaterialPassAction(rKernelContext),
		_bUseShader(bUseShader),
		_diffuse(diffuse)
	{}
  virtual ~MaterialSetDiffuse()
	{}
	void setUseShader(bool b)
	{
		_bUseShader = b;
	}
	void setDiffuseColor(const Ogre::ColourValue& d)
	{
		_diffuse = d;
	}
	void setDiffuseColor(Ogre::Real r, Ogre::Real g, Ogre::Real b)
	{
		_diffuse.r = r;
		_diffuse.g = g;
		_diffuse.b = b;
	}
	void setDiffuseAlpha(Ogre::Real a)
	{
		_diffuse.a = a;
	}

protected:
	bool _bUseShader;
	Ogre::ColourValue _diffuse;
private:
  ///\brief The action for each pass of each technique.
  virtual void passAction(
		SubEntityMaterial* subEntity,
		Ogre::Pass* passDest,
		Ogre::Pass* passSrc) const;
	virtual void subEntityAction(
		SubEntityMaterial* subEntity) const;
};

//---------------------------------------------------------------------------
///\brief Action to modify a Vertex Buffer.
class MaterialVertexBuffer : public IMaterialAction
{
public:
  ///\brief Constructor
	MaterialVertexBuffer(
		const OpenViBE::Kernel::IKernelContext& rKernelContext);

	///\brief Constructor
	MaterialVertexBuffer(
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		/*
		OpenViBE::uint32 ui32VertexPositionCount,
		const OpenViBE::float32* pVertexPositionArray, */
		size_t ui32VertexColorCount,
		const float* pVertexColorArray);

	///\brief Destructor.
	virtual ~MaterialVertexBuffer();
/*
	void setVertexPositionArray(
		OpenViBE::uint32 ui32VertexPositionCount,
		OpenViBE::float32* pVertexPositionArray);
	*/
	void setVertexColorArray(
		size_t ui32VertexColorCount,
		const float* pVertexColorArray);

private:
	virtual bool entityAction(
		EntityMaterial* entity) const;

	bool setSubMeshColors(
		EntityMaterial* entity,
		unsigned short mesh_index,
		size_t ui32VertexColorCount,
		const float* pVertexColorArray) const;

private:
  /// @name Arguments
  //@{
	/*
	size_t m_ui32VertexPositionCount;
	float* m_pVertexPositionArray;*/
	size_t m_ui32VertexColorCount;
	const float* m_pVertexColorArray;
  //@}
};

//===========================================================================
///\brief Action to change this instance transparency.
/// @param transparency The new transparency. Values will be clamped to [0.0f..1.0f].
/// @note This changes transparency. A value of 0 means full opacity, while 1 means full
///       transparency (invisible)
/// @note If transparency equals 0, it will use the original material instead of the copy
///       (the copy is mandatory transparent, and thus might be slower than the original).
class MaterialSetTransparency : public IMaterialPassAction
{
public:
  ///\brief The contructor stores the argument.
  MaterialSetTransparency(
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		Ogre::Real transparency);
  ///\brief Destructor.
  virtual ~MaterialSetTransparency()
	{}
protected:
  ///\brief The argument.
  /// It is given to the function.
  Ogre::Real _transparency ;
private:
  ///\brief The action update the entity.
  virtual bool entityAction(EntityMaterial* entity) const;
  ///\brief The action for each pass of each technique.
  virtual void passAction(SubEntityMaterial* subEntity, Ogre::Pass* passDest, Ogre::Pass* passSrc) const;
};

//===========================================================================
///\brief Action to set the awareness color.
/// Carefull MaterialSetAwareness is exclusive, you have to choose beetween,
/// MaterialSetMaterialName and MaterialSetAwareness
class MaterialSetAwareness : public IMaterialPassAction
{
public:
  ///\brief The contructor stores the argument.
  MaterialSetAwareness(
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		const Ogre::Vector4& awarenessColor) :
		IMaterialPassAction(rKernelContext),
		_awarenessColor(awarenessColor)
	{}
  ///\brief Destructor.
  virtual ~MaterialSetAwareness()
	{}
protected:
  ///\brief The awareness color.
  /// The alpha channel is the blend value.
  Ogre::Vector4 _awarenessColor;
private:
  ///\brief The action for each pass of each technique.
  virtual void passAction(
		SubEntityMaterial* subEntity,
		Ogre::Pass* passDest,
		Ogre::Pass* passSrc) const;
  ///\brief Compute the blend for one parameter of a color.
  /// The alpha channel is the blend value.
  inline float computeBlend(
		float currentPrm,
		float awarenessPrm) const
  {
    return ( 1.0f - _awarenessColor.w ) * currentPrm + _awarenessColor.w * awarenessPrm;
  }
  ///\brief Compute the blend for a color.
  /// The alpha channel is the blend value.
  Ogre::ColourValue computeBlend(const Ogre::ColourValue& currentColor) const;
};

//===========================================================================
///\brief Action to know if is programmable.
/// Main pass is programmable ? ie includes either a vertex or fragment
/// program.
/// \code
/// IsProgrammable isProgrammable ;
/// _nmi->doAction( isProgrammable ) ;
/// if( isProgrammable )
/// ...
/// \endcode
class IsProgrammable : public IMaterialAction
{
public:
  ///\brief The contructor. The result is \e false by default.
  IsProgrammable(
		const OpenViBE::Kernel::IKernelContext& rKernelContext) :
		IMaterialAction(rKernelContext),
		_isProgrammable(false)
	{}
  ///\brief Destructor.
  virtual ~IsProgrammable()
	{}
  ///\brief Operator to retrieve the result.
  operator bool()
	{
		return _isProgrammable;
	}
private:
  ///\brief The result. It is update by the action process.
  mutable bool _isProgrammable;
  ///\brief The action calls the appropriate function to update the result.
  virtual void subEntityAction(SubEntityMaterial* subEntity) const;
};

//===========================================================================
///\brief Action to set shader.
///Allow user to add fragment and shader on a new Pass
class MaterialSetShader : public IMaterialAction
{
public:
  ///\brief The contructor stores the arguments.
  MaterialSetShader(
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		const std::string & fragment,
		const std::string & vertex,
    Ogre::GpuProgramParameters *fragmentPrm,
    Ogre::GpuProgramParameters *vertexPrm,
    bool light,
    const std::string & texture = "null") :
		IMaterialAction(rKernelContext),
		_fragment(fragment),
    _vertex(vertex),
    _fragmentPrm(fragmentPrm),
    _vertexPrm(vertexPrm),
    _light(light),
    _texture(texture)
  {}
  ///\brief Destructor.
  virtual ~MaterialSetShader()
	{}
private:
  /// @name Arguments
  //@{
  std::string _fragment;
  std::string _vertex;
  Ogre::GpuProgramParameters *_fragmentPrm;
  Ogre::GpuProgramParameters *_vertexPrm;
  bool _light;
  std::string _texture;
  //@}
  ///\brief The action calls the appropriate function with the arguments.
  virtual void subEntityAction(
		SubEntityMaterial* subEntity) const;
};

#endif // __OpenViBEKernel_Kernel_Player_MaterialAction_H__
