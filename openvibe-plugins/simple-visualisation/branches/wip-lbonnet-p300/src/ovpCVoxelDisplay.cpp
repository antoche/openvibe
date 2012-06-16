#include "ovpCVoxelDisplay.h"
#include "ovpCVoxelDisplay/ovpCVoxelView.h"
#include <cstdlib>
#include <cmath>
#include <memory.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

using namespace OpenViBEToolkit;

using namespace std;

CVoxel::CVoxel()
	:m_oCubeIdentifier(OV_UndefinedIdentifier)
	,m_oSphereIdentifier(OV_UndefinedIdentifier)
	,m_bVisible(false)
	,m_f32X(0)
	,m_f32Y(0)
	,m_f32Z(0)
{
}

boolean CVoxel::setObjectIdentifiers(CIdentifier oCubeIdentifier, CIdentifier oSphereIdentifier)
{
	m_oCubeIdentifier = oCubeIdentifier;
	m_oSphereIdentifier = oSphereIdentifier;
	return true;
}

boolean CVoxel::setPosition(float32 f32X, float32 f32Y, float32 f32Z)
{
	m_f32X = f32X;
	m_f32Y = f32Y;
	m_f32Z = f32Z;
	return true;
}

CVoxelDisplay::CVoxelDisplay(void) :
	m_bVoxelsMatrixLoaded(false),
	m_pStreamedMatrixDatabase(NULL),
	m_pVoxelView(NULL),
	m_o3DWidgetIdentifier(OV_UndefinedIdentifier),
	m_oResourceGroupIdentifier(OV_UndefinedIdentifier),
	m_bCameraPositioned(false),
	m_bAutoCameraMovementEnabled(false),
	m_f64AutoCameraMovementStartTime(0),
	m_f32ThetaOffset(0),
	m_f32PhiOffset(0),
	m_f64MinPotential(FLT_MAX),
	m_f64MaxPotential(FLT_MIN),
	m_ui32NbColors(0),
	m_pColorScale(NULL),
	m_oScalpId(OV_UndefinedIdentifier),
	m_oFaceId(OV_UndefinedIdentifier),
	m_bSetVoxelObject(false),
	m_eVoxelObject(Standard3DObject_Cube),
	m_bToggleColorModification(false),
	m_bColorModificationToggled(false),
	m_bToggleTransparencyModification(false),
	m_bTransparencyModificationToggled(false),
	m_bToggleSizeModification(false),
	m_bSizeModificationToggled(false),
	m_f64MinScaleFactor(1),
	m_f64MaxScaleFactor(2),
	m_bInclusiveDisplayThresholdBoundary(false),
	m_f64MinDisplayThreshold(0),
	m_f64MaxDisplayThreshold(1),
	m_bSetSkullOpacity(false),
	m_f64SkullOpacity(0),
	m_bRepositionCamera(false)
{
	//TODO : read color scale from some database of flow header
	m_ui32NbColors = 13;
	m_pColorScale = new float32[m_ui32NbColors*3];
	m_pColorScale[0] = 255/255.f; m_pColorScale[1] = 255/255.f; m_pColorScale[2] = 0/255.f;
	m_pColorScale[3] = 200/255.f; m_pColorScale[4] = 255/255.f; m_pColorScale[5] = 0/255.f;
	m_pColorScale[6] = 150/255.f; m_pColorScale[7] = 255/255.f; m_pColorScale[8] = 0/255.f;
	m_pColorScale[9] = 0/255.f; m_pColorScale[10] = 225/255.f; m_pColorScale[11] = 25/255.f;
	m_pColorScale[12] = 0/255.f; m_pColorScale[13] = 164/255.f; m_pColorScale[14] = 100/255.f;
	m_pColorScale[15] = 0/255.f; m_pColorScale[16] = 97/255.f; m_pColorScale[17] = 121/255.f;
	m_pColorScale[18] = 0/255.f; m_pColorScale[19] = 0/255.f; m_pColorScale[20] = 152/255.f;
	m_pColorScale[21] = 77/255.f; m_pColorScale[22] = 0/255.f; m_pColorScale[23] = 178/255.f;
	m_pColorScale[24] = 115/255.f; m_pColorScale[25] = 1/255.f; m_pColorScale[26] = 177/255.f;
	m_pColorScale[27] = 153/255.f; m_pColorScale[28] = 0/255.f; m_pColorScale[29] = 178/255.f;
	m_pColorScale[30] = 205/255.f; m_pColorScale[31] = 0/255.f; m_pColorScale[32] = 101/255.f;
	m_pColorScale[33] = 234/255.f; m_pColorScale[34] = 1/255.f; m_pColorScale[35] = 0/255.f;
	m_pColorScale[36] = 255/255.f; m_pColorScale[37] = 0/255.f; m_pColorScale[38] = 0/255.f;
}

uint64 CVoxelDisplay::getClockFrequency(void)
{
	return ((uint64)1LL)<<37;
}

boolean CVoxelDisplay::initialize(void)
{
	// Create OVMatrix file reader
	//FIXME
	m_pOVMatrixFileReader =
		&getAlgorithmManager().getAlgorithm(
		getAlgorithmManager().createAlgorithm(
		OpenViBE::CIdentifier(0x10661A33, 0x0B0F44A7)
		/*OVP_ClassId_Algorithm_OVMatrixFileReader*/));
	m_pOVMatrixFileReader->initialize();

	// OVMatrix file reader parameters
	ip_sFilename.initialize(
		m_pOVMatrixFileReader->getInputParameter(
		OpenViBE::CIdentifier(0x28F87B29, 0x0B09737E)
		/*OVP_Algorithm_OVMatrixFileReader_InputParameterId_Filename*/));

	op_pVoxelsMatrix.initialize(
		m_pOVMatrixFileReader->getOutputParameter(
		OpenViBE::CIdentifier(0x2F9521E0, 0x027D789F)
		/*OVP_Algorithm_OVMatrixFileReader_OutputParameterId_Matrix*/));

	m_bVoxelsMatrixLoaded = false;

	//create spectrum database
	m_pStreamedMatrixDatabase = new CStreamedMatrixDatabase(*this);
	m_pStreamedMatrixDatabase->initialize();
	m_pStreamedMatrixDatabase->setMaxBufferCount(2);

	//retrieve box settings
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	//CString l_sFilename;
	l_rStaticBoxContext.getSettingValue(0, *ip_sFilename/*l_sFilename*/);
	//*ip_sFilename = l_sFilename;

	//create voxel view (handling GUI interaction)
	m_pVoxelView = new CVoxelView(*this);

	//have database notify view when data is received
	m_pStreamedMatrixDatabase->setDrawable(m_pVoxelView);

	//send widget pointers to visualisation context for parenting
	::GtkWidget* l_pWidget=NULL;
	m_o3DWidgetIdentifier = getBoxAlgorithmContext()->getVisualisationContext()->create3DWidget(l_pWidget);
	if(l_pWidget != NULL)
	{
		getBoxAlgorithmContext()->getVisualisationContext()->setWidget(l_pWidget);
	}

	::GtkWidget* l_pToolbarWidget=NULL;
	m_pVoxelView->getToolbar(l_pToolbarWidget);
	if(l_pToolbarWidget != NULL)
	{
		getBoxAlgorithmContext()->getVisualisationContext()->setToolbar(l_pToolbarWidget);
	}

	//resource group
	getVisualisationContext().createResourceGroup(m_oResourceGroupIdentifier, "VoxelDisplayResources");
	getVisualisationContext().addResourceLocation(m_oResourceGroupIdentifier, "../share/openvibe-plugins/simple-visualisation/voxeldisplay", ResourceType_Directory, false);
	getVisualisationContext().initializeResourceGroup(m_oResourceGroupIdentifier);

	return true;
}

boolean CVoxelDisplay::uninitialize(void)
{
	delete m_pVoxelView;
	m_pVoxelView = NULL;
	delete m_pStreamedMatrixDatabase;
	m_pStreamedMatrixDatabase = NULL;

	m_pOVMatrixFileReader->uninitialize();
	getAlgorithmManager().releaseAlgorithm(*m_pOVMatrixFileReader);

	delete[] m_pColorScale;

	//destroy resource group
	getVisualisationContext().destroyResourceGroup(m_oResourceGroupIdentifier);

	return true;
}

boolean CVoxelDisplay::processInput(uint32 ui32InputIndex)
{
	if(!getBoxAlgorithmContext()->getVisualisationContext()->is3DWidgetRealized(m_o3DWidgetIdentifier))
	{
		return true;
	}
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CVoxelDisplay::processClock(IMessageClock& rMessageClock)
{
	if(!getBoxAlgorithmContext()->getVisualisationContext()->is3DWidgetRealized(m_o3DWidgetIdentifier))
	{
		return true;
	}
	updateCameraPosition();
	getBoxAlgorithmContext()->getVisualisationContext()->update3DWidget(m_o3DWidgetIdentifier);
	//getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CVoxelDisplay::process(void)
{
	IDynamicBoxContext* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();
	uint32 i;

	for(i=0; i<l_pDynamicBoxContext->getInputChunkCount(0); i++)
	{
		m_pStreamedMatrixDatabase->decodeMemoryBuffer(
			l_pDynamicBoxContext->getInputChunk(0, i),
			l_pDynamicBoxContext->getInputChunkStartTime(0, i),
			l_pDynamicBoxContext->getInputChunkEndTime(0, i));

		l_pDynamicBoxContext->markInputAsDeprecated(0, i);
	}

	boolean l_bProcess3D = process3D();

	if(l_bProcess3D == true)
	{
		getBoxAlgorithmContext()->getVisualisationContext()->update3DWidget(m_o3DWidgetIdentifier);
		return true;
	}
	else
	{
		//disable plugin upon errors
		return false;
	}
}

boolean CVoxelDisplay::enableAutoCameraMovement(boolean bEnable)
{
	m_bAutoCameraMovementEnabled = bEnable;
	if(bEnable == true)
	{
		//reset time base used to compute theta/phi offsets
		m_f64AutoCameraMovementStartTime = 0;
		//reset offsets
		m_f32ThetaOffset = 0;
		m_f32PhiOffset = 0;
	}
	return true;
}

boolean CVoxelDisplay::setVoxelObject(EStandard3DObject eVoxelObject)
{
	m_bSetVoxelObject = true;
	m_eVoxelObject = eVoxelObject;

	//set mod flags to true so that voxel state is up to date
	m_bToggleColorModification = true;
	m_bToggleTransparencyModification = true;
	m_bToggleSizeModification = true;

	return true;
}

boolean CVoxelDisplay::toggleColorModification(boolean bModifyColor)
{
	m_bToggleColorModification = true;
	m_bColorModificationToggled = bModifyColor;
	return true;
}

boolean CVoxelDisplay::toggleTransparencyModification(boolean bModifyTransparency)
{
	m_bToggleTransparencyModification = true;
	m_bTransparencyModificationToggled = bModifyTransparency;
	return true;
}

boolean CVoxelDisplay::toggleSizeModification(boolean bModifySize)
{
	m_bToggleSizeModification = true;
	m_bSizeModificationToggled = bModifySize;
	return true;
}

boolean CVoxelDisplay::setMinScaleFactor(float64 f64MinScaleFactor)
{
	m_f64MinScaleFactor = f64MinScaleFactor;
	return true;
}

boolean CVoxelDisplay::setMaxScaleFactor(float64 f64MaxScaleFactor)
{
	m_f64MaxScaleFactor = f64MaxScaleFactor;
	return true;
}

boolean CVoxelDisplay::setMinDisplayThreshold(float64 f64MinDisplayThreshold)
{
	m_f64MinDisplayThreshold = f64MinDisplayThreshold;
	return true;
}

boolean CVoxelDisplay::setMaxDisplayThreshold(float64 f64MaxDisplayThreshold)
{
	m_f64MaxDisplayThreshold = f64MaxDisplayThreshold;
	return true;
}

boolean CVoxelDisplay::setDisplayThresholdBoundaryType(boolean bInclusiveBoundary)
{
	m_bInclusiveDisplayThresholdBoundary = bInclusiveBoundary;
	return true;
}

boolean CVoxelDisplay::setSkullOpacity(float64 f64Opacity)
{
	m_bSetSkullOpacity = true;
	m_f64SkullOpacity = f64Opacity;
	return true;
}

boolean CVoxelDisplay::repositionCamera()
{
	m_bRepositionCamera = true;
	return true;
}

static float32 s_f32VoxelOffsetX = 0.f;
static float32 s_f32VoxelOffsetY = 1.2f;
static float32 s_f32VoxelOffsetZ = -0.05f;
static float32 s_f32ScaleFromOffset = 0.015f;
static float32 s_f32VoxelScale = 0.05f;
static const uint32 s_ui32VoxelStep = 1;

boolean CVoxelDisplay::process3D()
{
	//load voxel coords
	if(m_bVoxelsMatrixLoaded == false)
	{
		return loadVoxels();
	}
	//create voxels and load head mesh
	else if(m_oVoxels.size() == 0)
	{
		return createVoxels();
	}
	//objects have been created : auto position camera
	else if(m_bCameraPositioned == false)
	{
		getVisualisationContext().setCameraToEncompassObjects(m_o3DWidgetIdentifier);
		m_bCameraPositioned = true;
		return true;
	}
	//proceed with color/shape modification
	else
	{
		return updateVoxels();
	}
}

boolean CVoxelDisplay::loadVoxels()
{
	m_bVoxelsMatrixLoaded = true;

	m_pOVMatrixFileReader->process();

	//ensure matrix is 2 dimensional and that dimension sizes are correct
	if(op_pVoxelsMatrix->getDimensionCount() != 2 || op_pVoxelsMatrix->getDimensionSize(1) != 3)
	{
		getLogManager() << LogLevel_Warning << "Wrong format for voxels coordinates matrix loaded from file " << *ip_sFilename << " !\n";
		return false;
	}

	return true;
}

boolean CVoxelDisplay::createVoxels()
{
	//set background color
	getVisualisationContext().setBackgroundColor(m_o3DWidgetIdentifier, 0, 0, 0);

	//load skull meshes
	m_oFaceId = getVisualisationContext().createObject("ov_voxeldisplay_face");
	m_oScalpId = getVisualisationContext().createObject("ov_voxeldisplay_scalp");
	//initialize skull opacity
	getVisualisationContext().setObjectVisible(m_oFaceId, m_f64SkullOpacity > 0);
	getVisualisationContext().setObjectVisible(m_oScalpId, m_f64SkullOpacity > 0);
	getVisualisationContext().setObjectTransparency(m_oFaceId, 1.f-(float32)m_f64SkullOpacity);
	getVisualisationContext().setObjectTransparency(m_oScalpId, 1.f-(float32)m_f64SkullOpacity);

	//create voxels
	if(op_pVoxelsMatrix->getDimensionCount() != 2)
	{
		return false;
	}
	m_oVoxels.resize(op_pVoxelsMatrix->getDimensionSize(0));
	CNameValuePairList l_oParamsList;
	//clone voxel materials so that color changes don't interfere with each other
	l_oParamsList.setValue("CloneMaterials", true);

	for(uint32 i=0; i<op_pVoxelsMatrix->getDimensionSize(0); i+=s_ui32VoxelStep)
	{
		//object is visible by default
		m_oVoxels[i].m_bVisible = true;
		//store voxel position
		m_oVoxels[i].setPosition(
			float32((*(op_pVoxelsMatrix->getBuffer() + 3*i) - s_f32VoxelOffsetX) * s_f32ScaleFromOffset + s_f32VoxelOffsetX),
			float32((*(op_pVoxelsMatrix->getBuffer() + 3*i+1) - s_f32VoxelOffsetY) * s_f32ScaleFromOffset + s_f32VoxelOffsetY),
			float32((*(op_pVoxelsMatrix->getBuffer() + 3*i+2) - s_f32VoxelOffsetZ) * s_f32ScaleFromOffset + s_f32VoxelOffsetZ));

		//load shapes
		m_oVoxels[i].setObjectIdentifiers(
			getVisualisationContext().createObject("ov_unitcube", &l_oParamsList),
			OV_UndefinedIdentifier/*getVisualisationContext().createObject("ov_unitsphere_80faces", &l_oParamsList)*/);

		//show active shape and hide the other one
		getVisualisationContext().setObjectVisible(m_oVoxels[i].m_oCubeIdentifier, m_eVoxelObject == Standard3DObject_Cube);
		//getVisualisationContext().setObjectVisible(m_oVoxels[i].m_oSphereIdentifier, m_eVoxelObject == Standard3DObject_Sphere);
		//position 3D objects
		getVisualisationContext().setObjectPosition(m_oVoxels[i].m_oCubeIdentifier, m_oVoxels[i].m_f32X, m_oVoxels[i].m_f32Y, m_oVoxels[i].m_f32Z);
		//getVisualisationContext().setObjectPosition(m_oVoxels[i].m_oSphereIdentifier, m_oVoxels[i].m_f32X, m_oVoxels[i].m_f32Y, m_oVoxels[i].m_f32Z);
		//scale 3D objects
		getVisualisationContext().setObjectScale(m_oVoxels[i].m_oCubeIdentifier, s_f32VoxelScale, s_f32VoxelScale, s_f32VoxelScale);
		//getVisualisationContext().setObjectScale(m_oVoxels[i].m_oSphereIdentifier, s_f32VoxelScale, s_f32VoxelScale, s_f32VoxelScale);
	}

	m_oPotentialMatrix.setDimensionCount(1);
	m_oPotentialMatrix.setDimensionSize(0, m_oVoxels.size());

	return true;
}

boolean CVoxelDisplay::updateVoxels()
{
	if(computeActivationLevels() == false)
	{
		return false;
	}

	if(processActivationLevels() == false)
	{
		return false;
	}

	//shape change
	if(m_bSetVoxelObject == true)
	{
		for(uint32 i=0; i<m_oVoxels.size(); i++)
		{
			getVisualisationContext().setObjectVisible(m_oVoxels[i].m_oCubeIdentifier, m_eVoxelObject == Standard3DObject_Cube);
			//getVisualisationContext().setObjectVisible(m_oVoxels[i].m_oSphereIdentifier, m_eVoxelObject == Standard3DObject_Sphere);
			//object is visible now
			m_oVoxels[i].m_bVisible = true;
		}

		m_bSetVoxelObject = false;
	}

	//color toggled on/off
	if(m_bToggleColorModification == true)
	{
		if(m_bColorModificationToggled == false)
		{
			//reset color
			for(uint32 i=0; i<m_oVoxels.size(); i+=s_ui32VoxelStep)
			{
				getVisualisationContext().setObjectColor(getActiveShapeIdentifier(m_oVoxels[i]), 1, 1, 1);
			}
		}

		m_bToggleColorModification = false;
	}

	//transparency toggled on/off
	if(m_bToggleTransparencyModification == true)
	{
		//unset transparency
		if(m_bTransparencyModificationToggled == false)
		{
			for(uint32 i=0; i<m_oVoxels.size(); i+=s_ui32VoxelStep)
			{
				getVisualisationContext().setObjectTransparency(getActiveShapeIdentifier(m_oVoxels[i]), 0);
			}
		}

		m_bToggleTransparencyModification = false;
	}

	//size modification toggled on/off
	if(m_bToggleSizeModification == true)
	{
		if(m_bSizeModificationToggled == false)
		{
			//reset size
			for(uint32 i=0; i<m_oVoxels.size(); i+=s_ui32VoxelStep)
			{
				getVisualisationContext().setObjectScale(getActiveShapeIdentifier(m_oVoxels[i]), s_f32VoxelScale);
			}
		}

		m_bToggleSizeModification = false;
	}

	//skull opacity changed
	if(m_bSetSkullOpacity == true)
	{
		getVisualisationContext().setObjectVisible(m_oFaceId, m_f64SkullOpacity > 0);
		getVisualisationContext().setObjectVisible(m_oScalpId, m_f64SkullOpacity > 0);
		getVisualisationContext().setObjectTransparency(m_oFaceId, 1.f-(float32)m_f64SkullOpacity);
		getVisualisationContext().setObjectTransparency(m_oScalpId, 1.f-(float32)m_f64SkullOpacity);
		m_bSetSkullOpacity = false;
	}

	//camera reset requested
	if(m_bRepositionCamera == true)
	{
		getVisualisationContext().setCameraToEncompassObjects(m_o3DWidgetIdentifier);
		m_bRepositionCamera = false;
		//reset offsets so that camera animation oscillates around new position
		m_f64AutoCameraMovementStartTime = 0;
		m_f32PhiOffset = 0;
		m_f32ThetaOffset = 0;
	}

	return true;
}

//Quake III code
inline float32 SquareRootFloat(const float32 & number)
{
	int32 i;
	float32 x, y;
	const float32 f = 1.5F;

	x = number * 0.5F;
	y  = number;
#if 0
	i  = * ( int32 * ) &y;
#else
	memcpy(&i, &y, sizeof(i));
#endif
	i  = 0x5f3759df - ( i >> 1 );
#if 0
	y  = * ( float32 * ) &i;
#else
	memcpy(&y, &i, sizeof(y));
#endif
	y  = y * ( f - ( x * y * y ) );
	y  = y * ( f - ( x * y * y ) );
	return number * y;
}

boolean CVoxelDisplay::computeActivationLevels()
{
	//retrieve sources matrix
	const float64* l_pBuffer = m_pStreamedMatrixDatabase->getBuffer(m_pStreamedMatrixDatabase->getCurrentBufferCount()-1);

	if(l_pBuffer == NULL)
	{
		//don't disable plugin but wait for sources matrix to be received
		return true;
	}

	//ensure matrix has the right size
	if(m_pStreamedMatrixDatabase->getBufferElementCount() != 3 * m_oVoxels.size())
	{
		getLogManager() << LogLevel_Warning << "Sources matrix triplets count (3*" << m_pStreamedMatrixDatabase->getBufferElementCount()/3
			<< ") differs from voxels count (" << (uint32)m_oVoxels.size()
			<<")! Please ensure the spatial filter used to reconstruct sources corresponds to the voxel grid used by this plugin.\n";
		return false;
	}

	m_f64MinPotential = FLT_MAX;
	m_f64MaxPotential = FLT_MIN;

	//compute vector norm
	for(uint32 i=0; i<m_oVoxels.size(); i+=s_ui32VoxelStep)
	{
		//FIXME take into account number of samples per buffer and use last buffer!
		const float64* l_pVoxelBuffer = l_pBuffer + 3 * (i /** nbEchantillons*/);

#if 0
		float64 f64Potential = sqrt(*l_pVoxelBuffer * *l_pVoxelBuffer +
			*(l_pVoxelBuffer+1) * *(l_pVoxelBuffer+1) + *(l_pVoxelBuffer+2) * *(l_pVoxelBuffer+2));
#else
		//$$$ this will probably need to be fixed on 64 bits
		float64 f64Potential = SquareRootFloat((float32)(*l_pVoxelBuffer * *l_pVoxelBuffer +
			*(l_pVoxelBuffer+1) * *(l_pVoxelBuffer+1) + *(l_pVoxelBuffer+2) * *(l_pVoxelBuffer+2)));
#endif

		*(m_oPotentialMatrix.getBuffer() + i) = f64Potential;

		if(m_f64MinPotential > f64Potential)
		{
			m_f64MinPotential = f64Potential;
		}
		else if(m_f64MaxPotential < f64Potential)
		{
			m_f64MaxPotential = f64Potential;
		}
	}

	return true;
}

boolean CVoxelDisplay::processActivationLevels()
{
	float64 l_f64InvPotentialStep = m_ui32NbColors / (m_f64MaxPotential - m_f64MinPotential);

	float64 l_f64InvPotentialInterval = 1. / (m_f64MaxPotential - m_f64MinPotential);

	//update each voxel
	for(uint32 i=0; i<m_oVoxels.size(); i+=s_ui32VoxelStep)
	{
		float64 l_f64Potential = *(m_oPotentialMatrix.getBuffer() + i);

		//determine whether this voxel should be displayed
		float64 l_f64ActivationFactor = (l_f64Potential - m_f64MinPotential) * l_f64InvPotentialInterval; //0<x<1

		boolean l_bDisplayVoxel = false;
		if(m_bInclusiveDisplayThresholdBoundary == true)
		{
			l_bDisplayVoxel = (l_f64ActivationFactor >= m_f64MinDisplayThreshold && l_f64ActivationFactor <= m_f64MaxDisplayThreshold);
		}
		else
		{
			l_bDisplayVoxel = (l_f64ActivationFactor < m_f64MinDisplayThreshold || l_f64ActivationFactor > m_f64MaxDisplayThreshold);
		}

		if(l_bDisplayVoxel != m_oVoxels[i].m_bVisible)
		{
			m_oVoxels[i].m_bVisible = l_bDisplayVoxel;
			getVisualisationContext().setObjectVisible(getActiveShapeIdentifier(m_oVoxels[i]), l_bDisplayVoxel);
		}

		if(l_bDisplayVoxel == false)
		{
			continue;
		}

		//retrieve corresponding color index
		uint32 l_ui32ColorIndex = (uint32)((l_f64Potential - m_f64MinPotential) * l_f64InvPotentialStep);
		if(l_ui32ColorIndex >= m_ui32NbColors)
		{
			l_ui32ColorIndex = m_ui32NbColors-1;
		}

		if(m_bColorModificationToggled == true)
		{
			getVisualisationContext().setObjectColor(
				getActiveShapeIdentifier(m_oVoxels[i]),
				m_pColorScale[3*l_ui32ColorIndex],
				m_pColorScale[3*l_ui32ColorIndex+1],
				m_pColorScale[3*l_ui32ColorIndex+2]);
		}

		if(m_bTransparencyModificationToggled == true)
		{
			float32 l_f32Opacity = float32(l_f64ActivationFactor);

			getVisualisationContext().setObjectTransparency(getActiveShapeIdentifier(m_oVoxels[i]),	1 - l_f32Opacity);
		}

		if(m_bSizeModificationToggled == true)
		{
			getVisualisationContext().setObjectScale(
				getActiveShapeIdentifier(m_oVoxels[i]), s_f32VoxelScale * (
					(float32)(m_f64MinScaleFactor + l_f64ActivationFactor * (m_f64MaxScaleFactor - m_f64MinScaleFactor)))
				);
		}
	}

	return true;
}

boolean CVoxelDisplay::updateCameraPosition()
{
	const float32 l_f32PhiAmplitude = 2; //in degrees
	const float32 l_f32ThetaAmplitude = 1;
	const float32 l_f32PhiAmplitudeTime = 2; //in seconds
	const float32 l_f32ThetaAmplitudeTime = l_f32PhiAmplitudeTime;
	const float32 l_f32ThetaAmplitudePhase = 0;
	const float32 l_f32Pi = 3.14159265359f;

	if(m_bAutoCameraMovementEnabled == true)
	{
		//get current time
		uint64 l_ui64Time = getBoxAlgorithmContext()->getPlayerContext()->getCurrentTime();
		float64 l_f64CurTime = (float64)l_ui64Time / (float64)(1LL<<32);

		//ensure base time is initialized
		if(m_f64AutoCameraMovementStartTime == 0)
		{
			m_f64AutoCameraMovementStartTime = l_f64CurTime;
		}

		float32 l_f32CurTheta=0;
		float32 l_f32CurPhi=0;
		float32 l_f32CurDist=0;

		//retrieve cam coords
		getBoxAlgorithmContext()->getVisualisationContext()->getCameraSphericalCoordinates(
			m_o3DWidgetIdentifier, l_f32CurTheta, l_f32CurPhi, l_f32CurDist);

		//retrieve camera 'base' position (without offsets)
		l_f32CurTheta -= m_f32ThetaOffset;
		l_f32CurPhi -= m_f32PhiOffset;

		//compute new theta/phi offsets
		float32 l_f32ThetaAmplitudeCoef = float32(l_f64CurTime - m_f64AutoCameraMovementStartTime) / l_f32ThetaAmplitudeTime;
		l_f32ThetaAmplitudeCoef -= (float32)((int32)l_f32ThetaAmplitudeCoef); //normalize coef

		if(l_f32ThetaAmplitudeCoef < 0.25)
		{
			m_f32ThetaOffset = sinf(l_f32Pi/2 * 4 * l_f32ThetaAmplitudeCoef + l_f32ThetaAmplitudePhase) * l_f32ThetaAmplitude/2;
		}
		else if(l_f32ThetaAmplitudeCoef > 0.75)
		{
			m_f32ThetaOffset = sinf(l_f32Pi/2 * 4 * (l_f32ThetaAmplitudeCoef-1.f) + l_f32ThetaAmplitudePhase) * l_f32ThetaAmplitude/2;
		}
		else
		{
			m_f32ThetaOffset = l_f32ThetaAmplitude/2 - sinf(l_f32Pi/2 * 2 * (l_f32ThetaAmplitudeCoef - 0.25f) + l_f32ThetaAmplitudePhase) * l_f32ThetaAmplitude;
		}

		//Phi coef
		float32 l_f32PhiAmplitudeCoef = float32(l_f64CurTime - m_f64AutoCameraMovementStartTime) / l_f32PhiAmplitudeTime;
		l_f32PhiAmplitudeCoef -= (float32)((int32)l_f32PhiAmplitudeCoef); //normalize coef

		if(l_f32PhiAmplitudeCoef < 0.25)
		{
			m_f32PhiOffset = sinf(l_f32Pi/2 * 4 * l_f32PhiAmplitudeCoef) * l_f32PhiAmplitude/2;
		}
		else if(l_f32PhiAmplitudeCoef > 0.75)
		{
			m_f32PhiOffset = sinf(l_f32Pi/2 * 4 * (l_f32PhiAmplitudeCoef-1.f)) * l_f32PhiAmplitude/2;
		}
		else
		{
			m_f32PhiOffset = l_f32PhiAmplitude/2 - sinf(l_f32Pi/2 * 2 * (l_f32PhiAmplitudeCoef - 0.25f)) * l_f32PhiAmplitude;
		}

		//update camera position
		getBoxAlgorithmContext()->getVisualisationContext()->setCameraSphericalCoordinates(
			m_o3DWidgetIdentifier,
			l_f32CurTheta + m_f32ThetaOffset,
			l_f32CurPhi + m_f32PhiOffset,
			l_f32CurDist);
	}

	return true;
}

CIdentifier CVoxelDisplay::getActiveShapeIdentifier(CVoxel& rVoxel)
{
	return m_eVoxelObject==Standard3DObject_Cube?rVoxel.m_oCubeIdentifier:rVoxel.m_oSphereIdentifier;
}
