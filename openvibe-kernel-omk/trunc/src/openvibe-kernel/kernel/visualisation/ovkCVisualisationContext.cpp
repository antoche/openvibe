#include "../player/ovkCSimulatedBox.h"
#include "ovkCVisualisationContext.h"

#include "../../tools/ovk_bridge_bind_function.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CVisualisationManagerBridge : public TKernelObject<IVisualisationManager>
		{
		public:

			CVisualisationManagerBridge(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox) : TKernelObject<IVisualisationManager>(rKernelContext), m_pSimulatedBox(pSimulatedBox) { }

			virtual __BridgeBindFunc0__(getKernelContext().getVisualisationManager(), boolean, initialize3DContext, )

			virtual __BridgeBindFunc2__(getKernelContext().getVisualisationManager(), boolean, createResourceGroup, , CIdentifier&, rResourceGroupIdentifier, const CString&, rResourceGroupName)
			virtual __BridgeBindFunc4__(getKernelContext().getVisualisationManager(), boolean, addResourceLocation, , const CIdentifier&, rResourceGroupIdentifier, const CString&, rResourceName, OpenViBE::Kernel::EResourceType, eResourceType, boolean, bRecursive)
			virtual __BridgeBindFunc1__(getKernelContext().getVisualisationManager(), boolean, initializeResourceGroup, , const CIdentifier&, rResourceGroupIdentifier)
			virtual __BridgeBindFunc1__(getKernelContext().getVisualisationManager(), boolean, destroyResourceGroup, , const CIdentifier&, rResourceGroupIdentifier)

			virtual __BridgeBindFunc1__(getKernelContext().getVisualisationManager(), boolean, createVisualisationTree, , CIdentifier&, rVisualisationTreeIdentifier)
			virtual __BridgeBindFunc1__(getKernelContext().getVisualisationManager(), boolean, releaseVisualisationTree, , const CIdentifier&, rVisualisationTreeIdentifier)
			virtual __BridgeBindFunc1__(getKernelContext().getVisualisationManager(), IVisualisationTree&, getVisualisationTree, , const CIdentifier&, rVisualisationTreeIdentifier)
			virtual __BridgeBindFunc1__(getKernelContext().getVisualisationManager(), boolean, enumerateVisualisationTrees, const, IVisualisationManager::IVisualisationTreeEnum&, rCallBack)

			virtual __BridgeBindFunc3__(getKernelContext().getVisualisationManager(), boolean, setToolbar, , const CIdentifier&, rVisualisationTreeIdentifier, const CIdentifier&, rBoxIdentifier,  GtkWidget*, pToolbarWidget)
			virtual __BridgeBindFunc3__(getKernelContext().getVisualisationManager(), boolean, setWidget, , const CIdentifier&, rVisualisationTreeIdentifier, const CIdentifier&, rBoxIdentifier, GtkWidget*, pWidget)

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

boolean CVisualisationContext::setToolbar(::GtkWidget* pToolbarWidget)
{
	CIdentifier l_oBoxIdentifier;
	m_pSimulatedBox->getBoxIdentifier(l_oBoxIdentifier);

	return m_pVisualisationManagerBridge->setToolbar(
		m_pSimulatedBox->getScenario().getVisualisationTreeIdentifier(),
		l_oBoxIdentifier,
		pToolbarWidget);
}

boolean CVisualisationContext::setWidget(::GtkWidget* pTopmostWidget)
{
	CIdentifier l_oBoxIdentifier;
	m_pSimulatedBox->getBoxIdentifier(l_oBoxIdentifier);

	return m_pVisualisationManagerBridge->setWidget(
		m_pSimulatedBox->getScenario().getVisualisationTreeIdentifier(),
		l_oBoxIdentifier,
		pTopmostWidget);
}

boolean CVisualisationContext::createResourceGroup(CIdentifier& rResourceGroupIdentifier, const CString& rResourceGroupName)
{
	return m_pVisualisationManagerBridge->createResourceGroup(rResourceGroupIdentifier, rResourceGroupName);
}

boolean CVisualisationContext::addResourceLocation(const CIdentifier& rResourceGroupIdentifier, const CString& rPath, EResourceType type,	bool bRecursive)
{
	return m_pVisualisationManagerBridge->addResourceLocation(rResourceGroupIdentifier, rPath, type, bRecursive);
}

boolean CVisualisationContext::addResourceLocationFromFile(const CIdentifier& rResourceGroupIdentifier,	const CString& rResourcesFileName)
{
	return false;//m_pVisualisationManagerBridge->addResourceLocationFromFile(rResourceGroupIdentifier, rResourcesFileName);
}

boolean CVisualisationContext::initializeResourceGroup(const CIdentifier& rResourceGroupIdentifier)
{
	return m_pVisualisationManagerBridge->initializeResourceGroup(rResourceGroupIdentifier);
}

boolean CVisualisationContext::destroyResourceGroup(const CIdentifier& rResourceGroupIdentifier)
{
	return m_pVisualisationManagerBridge->destroyResourceGroup(rResourceGroupIdentifier);
}

CIdentifier CVisualisationContext::create3DWidget(::GtkWidget*& p3DWidget)
{
	return m_pSimulatedBox->create3DWidget(p3DWidget);
}

boolean CVisualisationContext::is3DWidgetRealized(
	const CIdentifier& rWidgetIdentifier) const
{
	return m_pSimulatedBox->is3DWidgetRealized(rWidgetIdentifier);
}

boolean CVisualisationContext::update3DWidget(const CIdentifier& rWidgetIdentifier)
{
	return m_pSimulatedBox->update3DWidget(rWidgetIdentifier);
}

boolean CVisualisationContext::setBackgroundColor(const OpenViBE::CIdentifier& rWidgetIdentifier, float32 f32ColorRed, float32 f32ColorGreen, float32 f32ColorBlue)
{
	return m_pSimulatedBox->setBackgroundColor(rWidgetIdentifier, f32ColorRed, f32ColorGreen, f32ColorBlue);
}

boolean CVisualisationContext::setCameraToEncompassObjects(const CIdentifier& rWidgetIdentifier)
{
	return m_pSimulatedBox->setCameraToEncompassObjects(rWidgetIdentifier);
}

boolean CVisualisationContext::setCameraSphericalCoordinates(const CIdentifier& rWidgetIdentifier, float32 rTheta, float32 rPhi, float32 rRadius)
{
	return m_pSimulatedBox->setCameraSphericalCoordinates(rWidgetIdentifier, rTheta, rPhi, rRadius);
}

CIdentifier CVisualisationContext::createObject(const CString& rObjectFileName, const CNameValuePairList* pObjectParams)
{
	return m_pSimulatedBox->createObject(rObjectFileName, pObjectParams);
}

CIdentifier CVisualisationContext::createObject(const Kernel::EStandard3DObject eStandard3DObject, const CNameValuePairList* pObjectParams)
{
	return m_pSimulatedBox->createObject(eStandard3DObject, pObjectParams);
}

boolean CVisualisationContext::removeObject(const CIdentifier& rIdentifier)
{
	return m_pSimulatedBox->removeObject(rIdentifier);
}

boolean CVisualisationContext::setObjectVisible(const CIdentifier& rIdentifier, boolean bVisible)
{
	return m_pSimulatedBox->setObjectVisible(rIdentifier, bVisible);
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

boolean CVisualisationContext::setObjectVertexCount(const CIdentifier& rIdentifier, uint32 ui32VertexCount)
{
	getLogManager() << LogLevel_Warning << "setObjectVertexCount is not implemented yet!\n";
	return false;
}

boolean CVisualisationContext::setObjectVertexPositionArray(const CIdentifier& rIdentifier, uint32 ui32VertexPositionCount, const float32* pVertexPositionArray)
{
	getLogManager() << LogLevel_Warning << "setObjectVertexPositionArray is not implemented yet!\n";
	return false;
}

boolean CVisualisationContext::setObjectVertexColorArray(const CIdentifier& rIdentifier, uint32 ui32VertexColorCount, const float32* pVertexColorArray)
{
	return m_pSimulatedBox->setObjectVertexColorArray(rIdentifier, ui32VertexColorCount, pVertexColorArray);
}

boolean CVisualisationContext::setObjectTriangleCount(const CIdentifier& rIdentifier, const uint32 ui32TriangleCount)
{
	getLogManager() << LogLevel_Warning << "setObjectTriangleCount is not implemented yet!\n";
	return false;
}

boolean CVisualisationContext::setObjectTriangleIndexArray(const CIdentifier& rIdentifier, uint32 ui32TriangleCount, const uint32* pTriangleIndexArray)
{
	getLogManager() << LogLevel_Warning << "setObjectTriangleIndexArray is not implemented yet!\n";
	return false;
}

boolean CVisualisationContext::getCameraSphericalCoordinates(const CIdentifier& rWidgetIdentifier, float32& rTheta, float32& rPhi, float32& rRadius)
{
	return m_pSimulatedBox->getCameraSphericalCoordinates(rWidgetIdentifier, rTheta, rPhi, rRadius);
}

boolean CVisualisationContext::getObjectAxisAlignedBoundingBox(const CIdentifier& rIdentifier, float32* pMinimum, float32* pMaximum)
{
	return m_pSimulatedBox->getObjectAxisAlignedBoundingBox(rIdentifier, pMinimum, pMaximum);
}

boolean CVisualisationContext::getObjectVertexCount(const CIdentifier& rIdentifier, uint32& ui32VertexCount) const
{
	return m_pSimulatedBox->getObjectVertexCount(rIdentifier, ui32VertexCount);
}

boolean CVisualisationContext::getObjectVertexPositionArray(const CIdentifier& rIdentifier, uint32 ui32VertexCount, float32* pVertexPositionArray) const
{
	return m_pSimulatedBox->getObjectVertexPositionArray(rIdentifier, ui32VertexCount, pVertexPositionArray);
}

boolean CVisualisationContext::getObjectVertexColorArray(const CIdentifier& rIdentifier, uint32 ui32VertexColorCount, float32* pVertexColorArray) const
{
	getLogManager() << LogLevel_Warning << "getObjectVertexColorArray is not implemented yet!\n";
	return false;
}

boolean CVisualisationContext::getObjectTriangleCount(const CIdentifier& rIdentifier, uint32& ui32TriangleCount) const
{
	return m_pSimulatedBox->getObjectTriangleCount(rIdentifier, ui32TriangleCount);
}

boolean CVisualisationContext::getObjectTriangleIndexArray(const CIdentifier& rIdentifier, uint32 ui32TriangleCount, uint32* pTriangleIndexArray) const
{
	return m_pSimulatedBox->getObjectTriangleIndexArray(rIdentifier, ui32TriangleCount, pTriangleIndexArray);
}
