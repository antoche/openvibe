#include "ovkCVisualisationContext.h"
#include "../player/ovkPsSimulatedBox.h"

#include "../../tools/ovk_bridge_bind_function.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CVisualisationManagerBridge : virtual public TKernelObject<IVisualisationManager>
		{
		public:

			CVisualisationManagerBridge(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox) : TKernelObject<IVisualisationManager>(rKernelContext), m_pSimulatedBox(pSimulatedBox) { }

			virtual __BridgeBindFunc1__(getKernelContext().getVisualisationManager(), boolean, createVisualisationTree, , CIdentifier&, rVisualisationTreeIdentifier)
			virtual __BridgeBindFunc1__(getKernelContext().getVisualisationManager(), boolean, releaseVisualisationTree, , const CIdentifier&, rVisualisationTreeIdentifier)
			virtual __BridgeBindFunc1__(getKernelContext().getVisualisationManager(), IVisualisationTree&, getVisualisationTree, , const CIdentifier&, rVisualisationTreeIdentifier)
			virtual __BridgeBindFunc1__(getKernelContext().getVisualisationManager(), boolean, enumerateVisualisationTrees, const, IVisualisationManager::IVisualisationTreeEnum&, rCallBack)
			virtual __BridgeBindFunc4__(getKernelContext().getVisualisationManager(), boolean, setWidgets, , const CIdentifier&, rVisualisationTreeIdentifier, const CString&, rVisualisationBoxName,  GtkWidget*, pWidget, GtkWidget*, pToolbarWidget)

			_IsDerivedFromClass_Final_(TKernelObject<IVisualisationManager>, OV_UndefinedIdentifier);

		protected:

			CSimulatedBox* m_pSimulatedBox;
		};
	};
};

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
#define boolean OpenViBE::boolean

CVisualisationContext::CVisualisationContext(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox)
	:TKernelObject<IVisualisationContext>(rKernelContext)
	,m_pSimulatedBox(pSimulatedBox)
	,m_pVisualisationManagerBridge(NULL)
{
	m_pVisualisationManagerBridge=new CVisualisationManagerBridge(rKernelContext, pSimulatedBox);
}

CVisualisationContext::~CVisualisationContext(void)
{
	delete m_pVisualisationManagerBridge;
}

boolean CVisualisationContext::setWidgets(GtkWidget* pWidget, GtkWidget* pToolbarWidget)
{
	//create 3D widgets dynamically
	if(pWidget == NULL) //FIXME : use a 1x1 GtkTable or better yet, a custom widget
	{
		pWidget = m_pSimulatedBox->create3DWidget(m_pSimulatedBox->getOVName());
	}

	return m_pVisualisationManagerBridge->setWidgets(
		m_pSimulatedBox->getScenario().getVisualisationTreeIdentifier(),
		m_pSimulatedBox->getOVName(),
		pWidget,
		pToolbarWidget);
}

boolean CVisualisationContext::setBackgroundColor(float32 f32ColorRed, float32 f32ColorGreen, float32 f32ColorBlue)
{
	return m_pSimulatedBox->setBackgroundColor(f32ColorRed, f32ColorGreen, f32ColorBlue);
}

boolean CVisualisationContext::setCameraToEncompassObjects()
{
	return m_pSimulatedBox->setCameraToEncompassObjects();
}

CIdentifier CVisualisationContext::createObject(const CString& rObjectFileName)
{
	return m_pSimulatedBox->createObject(rObjectFileName);
}

CIdentifier CVisualisationContext::createObject(const Kernel::EStandard3DObject eStandard3DObject)
{
	return m_pSimulatedBox->createObject(eStandard3DObject);
}

boolean CVisualisationContext::removeObject(const CIdentifier& rIdentifier)
{
	return m_pSimulatedBox->removeObject(rIdentifier);
}

boolean CVisualisationContext::setObjectScale(const CIdentifier& rIdentifier, float32 f32ScaleX, float32 f32ScaleY, float32 f32ScaleZ)
{
	return m_pSimulatedBox->setObjectScale(rIdentifier, f32ScaleX, f32ScaleY, f32ScaleZ);
}

boolean CVisualisationContext::setObjectScale(const CIdentifier& rIdentifier, float32 f32Scale)
{
	return m_pSimulatedBox->setObjectScale(rIdentifier, f32Scale);
}

boolean CVisualisationContext::setObjectPosition(const CIdentifier& rIdentifier, float32 f32PositionX, float32 f32PositionY, float32 f32PositionZ)
{
	return m_pSimulatedBox->setObjectPosition(rIdentifier, f32PositionX, f32PositionY, f32PositionZ);
}

boolean CVisualisationContext::setObjectOrientation(const CIdentifier& rIdentifier, float32 f32OrientationX, float32 f32OrientationY,
	float32 f32OrientationZ, float32 f32OrientationW)
{
	return m_pSimulatedBox->setObjectOrientation(rIdentifier, f32OrientationX, f32OrientationY, f32OrientationZ, f32OrientationW);
}

boolean CVisualisationContext::setObjectColor(const CIdentifier& rIdentifier, float32 f32ColorRed, float32 f32ColorGreen, float32 f32ColorBlue)
{
	return m_pSimulatedBox->setObjectColor(rIdentifier, f32ColorRed, f32ColorGreen, f32ColorBlue);
}

boolean CVisualisationContext::setObjectTransparency(const CIdentifier& rIdentifier, float32 f32Transparency)
{
	return m_pSimulatedBox->setObjectTransparency(rIdentifier, f32Transparency);
}

boolean CVisualisationContext::setObjectVertexCount(const CIdentifier& rIdentifier, const uint32 ui32VertexCount)
{
	return false;
}

boolean CVisualisationContext::setObjectVertexPositionArray(const CIdentifier& rIdentifier, const float32* pVertexPositionArray)
{
	return false;
}

boolean CVisualisationContext::setObjectVertexColorArray(const CIdentifier& rIdentifier, const uint32 ui32VertexColorCount, const float32* pVertexColorArray)
{
	return m_pSimulatedBox->setObjectVertexColorArray(rIdentifier, ui32VertexColorCount, pVertexColorArray);
}

boolean CVisualisationContext::setObjectTriangleCount(const CIdentifier& rIdentifier, const uint32 ui32TriangleCount)
{
	return false;
}

boolean CVisualisationContext::setObjectTriangleIndexArray(const CIdentifier& rIdentifier,	const uint32* pTriangleIndexArray)
{
	return false;
}

boolean CVisualisationContext::getObjectVertexCount(const CIdentifier& rIdentifier, uint32& ui32VertexCount) const
{
	return m_pSimulatedBox->getObjectVertexCount(rIdentifier, ui32VertexCount);
}

boolean CVisualisationContext::getObjectVertexPositionArray(const CIdentifier& rIdentifier, float32* pVertexPositionArray) const
{
	return m_pSimulatedBox->getObjectVertexPositionArray(rIdentifier, pVertexPositionArray);
}

boolean CVisualisationContext::getObjectVertexColorArray(const CIdentifier& rIdentifier,	float32* pVertexColorArray) const
{
	return false;
}

boolean CVisualisationContext::getObjectTriangleCount(const CIdentifier& rIdentifier, uint32& ui32TriangleCount) const
{
	return false;
}

boolean CVisualisationContext::getObjectTriangleIndexArray(const CIdentifier& rIdentifier, uint32* pTriangleIndexArray) const
{
	return false;
}
