#if !defined _OVKOMKSimulatedEntity_H_
#define _OVKOMKSimulatedEntity_H_

#include <OMKExtensibleSimulatedObject.h>
#include <OMKValuedEventListenerCallBack.h>
#include <OMKEventListenerCallBack.h>
#include <OMKSystemEventIdentifier.h>
/*
#include "ovkEventId.h"
#include "ovkPsTypeChunk.h"
*/

#include <OMKExtensibleSimulatedObject.h>
#include <OMKController.h>
#include <OMKAttributeAnimator.h>
#include <OMKTransformType.h>
#include <OMKColorType.h>
#include "ovkOMKVertexDataType.h"
#include "ovkOMKColorShaderAnimator.h"

class OMKSimulatedEntity : public OMK::ExtensibleSimulatedObject
{
public:

	DECLARE_OBJECT_FACTORY(OMKSimulatedEntity);

	/// Reads in the configuration node the values to set the attributs.
	virtual bool loadParameters( const OMK::ConfigurationParameterDescriptor * node ) ;

	/// init connection
	virtual void init();

	/// compute Position
	virtual void compute();

	DECLARE_TYPE_EVENT_LISTENER(OMK::Type::Transform, Translate, OMKSimulatedEntity);
	DECLARE_TYPE_EVENT_LISTENER(OMK::Type::Transform, Rotate, OMKSimulatedEntity);
	DECLARE_TYPE_EVENT_LISTENER(OMK::Type::Transform, Scale, OMKSimulatedEntity);
	DECLARE_TYPE_EVENT_LISTENER(OMK::Type::Color, Color, OMKSimulatedEntity);
	DECLARE_TYPE_EVENT_LISTENER(OMK::Type::Color, Transparency, OMKSimulatedEntity);
	DECLARE_TYPE_EVENT_LISTENER(OMK::Type::VertexData, VertexData, OMKSimulatedEntity);

	//\brief CallBack Anim
	virtual bool onTranslateEvent(TranslateEvent* pTranslateEvent);
	virtual bool onRotateEvent(RotateEvent* pRotateEvent);
	virtual bool onScaleEvent(ScaleEvent* pScaleEvent);
	virtual bool onColorEvent(ColorEvent* pColorEvent);
	virtual bool onTransparencyEvent(TransparencyEvent* pTransparencyEvent);
	virtual bool onVertexDataEvent(VertexDataEvent* pVertexDataEvent);

public:

	static const OMKEventIdentifier TRANSLATE_EVENT_ID;
	static const OMKEventIdentifier ROTATE_EVENT_ID;
	static const OMKEventIdentifier SCALE_EVENT_ID;
	static const OMKEventIdentifier COLOR_EVENT_ID;
	static const OMKEventIdentifier TRANSPARENCY_EVENT_ID;
	static const OMKEventIdentifier VERTEXDATA_EVENT_ID;

protected:

	TranslateEventListener* m_pTranslateEventListener;
	RotateEventListener* m_pRotateEventListener;
	ScaleEventListener* m_pScaleEventListener;
	ColorEventListener* m_pColorEventListener;
	TransparencyEventListener* m_pTransparencyEventListener;
	VertexDataEventListener* m_pVertexDataEventListener;

	OMK::AttributeAnimatorT< OMK::Type::Transform > _translate ;
	OMK::AttributeAnimatorT< OMK::Type::Transform > _rotate ;
	OMK::AttributeAnimatorT< OMK::Type::Transform > _scale ;
	OMK::AttributeAnimatorT< OMK::Type::Color > _color ;
	OMK::AttributeAnimatorT< OMK::Type::VertexData > _vertexData ;

	OpenViBE::boolean m_bColorModified;
	OMK::Type::Color m_oColor;

	OpenViBE::boolean m_bTranslationModified;
	OMK::Type::Transform m_oTranslationTransform;

	OpenViBE::boolean m_bRotationModified;
	OMK::Type::Transform m_oRotationTransform;

	OpenViBE::boolean m_bScaleModified;
	OMK::Type::Transform m_oScaleTransform;

	OpenViBE::boolean m_bVertexDataModified;
	OMK::Type::VertexData m_oVertexData;
};

#endif // _OVKOMKSimulatedEntity_H_
