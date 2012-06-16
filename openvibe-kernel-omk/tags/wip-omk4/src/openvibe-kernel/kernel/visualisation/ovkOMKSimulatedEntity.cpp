#include "ovkOMKSimulatedEntity.h"

using namespace std;
using namespace OMK;
using namespace OMK::Type;

#include <OMKSystemEventIdentifier.h>
#include "OMKAttribute.inl"
#include <string>

#include "ovkOMKDeclareVisualObjectExt.h"
#include "ovkOMKAddVisualObjectExt.h"
#include "OMKAddVisualObjectPrm.h"
#include "OMKVisBase.h"
#include "OMKValuedEventListenerCallBack.h"

REGISTER_OBJECT_FACTORY( OMKSimulatedEntity, "OMKSimulatedEntity" ) ;

const OMKEventIdentifier OMKSimulatedEntity::TRANSLATE_EVENT_ID("TRANSLATE_EVENT_ID");
const OMKEventIdentifier OMKSimulatedEntity::ROTATE_EVENT_ID("ROTATE_EVENT_ID");
const OMKEventIdentifier OMKSimulatedEntity::SCALE_EVENT_ID("SCALE_EVENT_ID");
const OMKEventIdentifier OMKSimulatedEntity::COLOR_EVENT_ID("COLOR_EVENT_ID");
const OMKEventIdentifier OMKSimulatedEntity::TRANSPARENCY_EVENT_ID("TRANSPARENCY_EVENT_ID");
const OMKEventIdentifier OMKSimulatedEntity::VERTEXDATA_EVENT_ID("VERTEXDATA_EVENT_ID");

OMKSimulatedEntity::OMKSimulatedEntity(OMKController& ctrl, const OMKObjectDescriptor& objectDescriptor)
	:OMK::ExtensibleSimulatedObject( ctrl, objectDescriptor )
	,m_pTranslateEventListener(NULL)
	,m_pRotateEventListener(NULL)
	,m_pScaleEventListener(NULL)
	,m_pColorEventListener(NULL)
	,m_pTransparencyEventListener(NULL)
	,m_pVertexDataEventListener(NULL)
	,_translate( "translateObject", OMK::Type::Transform())
	,_rotate( "rotateObject", OMK::Type::Transform())
	,_scale( "scaleObject", OMK::Type::Transform())
	,_color( "colorObject", OMK::Type::Color())
	,_vertexData( "vertexData", OMK::Type::VertexData())
	,m_bColorModified(false)
	,m_oColor(1, 1, 1, 1)
	,m_bTranslationModified(false)
	,m_bRotationModified(false)
	,m_bScaleModified(false)
	,m_bVertexDataModified(false)
{
	OMTRACEID ("OMKSimulatedEntity","On Construction");

	m_pTranslateEventListener = new TranslateEventListener(
		*this,
		(TranslateEventListener::CallBackFct)&OMKSimulatedEntity::onTranslateEvent,
		TRANSLATE_EVENT_ID);

	m_pRotateEventListener = new RotateEventListener(
		*this,
		(RotateEventListener::CallBackFct)&OMKSimulatedEntity::onRotateEvent,
		ROTATE_EVENT_ID);

	m_pScaleEventListener = new ScaleEventListener(
		*this,
		(ScaleEventListener::CallBackFct)&OMKSimulatedEntity::onScaleEvent,
		SCALE_EVENT_ID);

	m_pColorEventListener = new ColorEventListener(
		*this,
		(ColorEventListener::CallBackFct)&OMKSimulatedEntity::onColorEvent,
		COLOR_EVENT_ID);

	m_pTransparencyEventListener = new TransparencyEventListener(
		*this,
		(TransparencyEventListener::CallBackFct)&OMKSimulatedEntity::onTransparencyEvent,
		TRANSPARENCY_EVENT_ID);

	m_pVertexDataEventListener = new VertexDataEventListener(
		*this,
		(VertexDataEventListener::CallBackFct)&OMKSimulatedEntity::onVertexDataEvent,
		VERTEXDATA_EVENT_ID);

	//addAttribute(_translate,true); // Flow Creation
	addAttribute(_translate);
	addAttribute(_rotate);
	addAttribute(_scale);
	addAttribute(_color);
	addAttribute(_vertexData);

	new OMKDeclareVisualObjectExt(this, "OMKDeclareVisualObjectExt");
	new OMKAddVisualObjectExt(this, "OMKAddVisualObjectExt");
}

OMKSimulatedEntity::~OMKSimulatedEntity ()
{
	delete m_pTranslateEventListener;
	delete m_pRotateEventListener;
	delete m_pScaleEventListener;
	delete m_pColorEventListener;
	delete m_pTransparencyEventListener;
	delete m_pVertexDataEventListener;
}

bool OMKSimulatedEntity::loadParameters( const ConfigurationParameterDescriptor * node )
{
	/*
  ParametersAccessor::get( node, "Max", _max ) ;
  ParametersAccessor::get( node, "Step", _step ) ;
  ParametersAccessor::get( node, "RotStep", _rotStep ) ;*/
  return true;
}

//---------------------------------------------------------------------------
// Init connection
//---------------------------------------------------------------------------
void OMKSimulatedEntity::init (void)
{
	OMK::ExtensibleSimulatedObject::init();

	_translate.initAnimator();
	_rotate.initAnimator();
	_scale.initAnimator();
	_color.initAnimator();
	_vertexData.initAnimator();
}

/*
bool OMKSimulatedEntity::processEvent(Event* pEvent)
{
	const EventIdentifier& eventId(pEvent->eventId);

	if(eventId == SystemEventIdentifier::MaskRestart)
  {

	}
}*/

bool OMKSimulatedEntity::onTranslateEvent(TranslateEvent* pTranslateEvent)
{
	m_oTranslationTransform = pTranslateEvent->value.getValue();
	m_bTranslationModified = true;
	return true;
}

bool OMKSimulatedEntity::onRotateEvent(RotateEvent* pRotateEvent)
{
	m_oRotationTransform = pRotateEvent->value.getValue();
	m_bRotationModified = true;
	return true;
}

bool OMKSimulatedEntity::onScaleEvent(ScaleEvent* pScaleEvent)
{
	m_oScaleTransform = pScaleEvent->value.getValue();
	m_bScaleModified = true;
	return true;
}

bool OMKSimulatedEntity::onColorEvent(ColorEvent* pColorEvent)
{
	OMK::Type::Color l_oColor = pColorEvent->value.getValue();
	l_oColor.setA(m_oColor.getA());
	m_oColor = l_oColor;
	m_bColorModified = true;
	return true;
}

bool OMKSimulatedEntity::onTransparencyEvent(TransparencyEvent* pTransparencyEvent)
{
	OMK::Type::Color l_oColor = pTransparencyEvent->value.getValue();
	m_oColor.setA(l_oColor.getA());
	m_bColorModified = true;
	return true;
}

bool OMKSimulatedEntity::onVertexDataEvent(VertexDataEvent* pVertexDataEvent)
{
	m_oVertexData = pVertexDataEvent->value.getValue();
	m_bVertexDataModified = true;
	return true;
}

void OMKSimulatedEntity::compute()
{
	OMK::ExtensibleSimulatedObject::compute();

	if(m_bTranslationModified == true)
	{
		cout << "Position before modification = " << _translate.get() << endl;
		_translate.set(m_oTranslationTransform);
		cout << "Position after  modification = " << _translate.get() << endl;
		m_bTranslationModified = false;
	}

	if(m_bRotationModified == true)
	{
		cout << "Rotation before modification = " << _rotate.get() << endl;
		_rotate.set(m_oRotationTransform);
		cout << "Rotation after  modification = " << _rotate.get() << endl;
		m_bRotationModified = false;
	}

	if(m_bScaleModified == true)
	{
		_scale.set(m_oScaleTransform);
		m_bScaleModified = false;
	}

	if(m_bColorModified == true)
	{
		_color.set(m_oColor);
		m_bColorModified = false;
	}

	if(m_bVertexDataModified == true)
	{
		_vertexData.set(m_oVertexData);
		m_bVertexDataModified = false;
	}
}
