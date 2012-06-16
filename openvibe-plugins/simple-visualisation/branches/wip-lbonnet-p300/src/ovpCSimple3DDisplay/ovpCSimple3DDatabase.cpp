#include "ovpCSimple3DDatabase.h"

#include <algorithm>

#include <cmath>
#include <cstring>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

using namespace OpenViBEToolkit;

using namespace std;

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		static const int s_nbColors = 13;
		static float32 s_palette[s_nbColors*4];

		CSimple3DDatabase::CSimple3DDatabase(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>& oPlugin) :
			CBufferDatabase(oPlugin),
			m_o3DWidgetIdentifier(OV_UndefinedIdentifier),
			m_bCreateObject(false),
			m_eStandard3DObject(Standard3DObject_Invalid),
			m_bDeleteObject(false),
			m_bSetObjectPosition(false),
			m_bSetObjectOrientation(false),
			m_bSetObjectScale(false),
			m_bSetObjectMaterialColor(false),
			m_bSetObjectVertexColor(false),
			m_bRepositionCamera(false)
		{
			memset(m_pPosition, 0, 3*sizeof(float32));
			memset(m_pOrientation, 0, 4*sizeof(float32));
			m_f32ScaleX = m_f32ScaleY = m_f32ScaleZ = 1.f;
			memset(m_pColor, 0, 4*sizeof(float32));

#define COLOR_SCALE 1.f

			s_palette[0] = 255*COLOR_SCALE/255; s_palette[1] = 255*COLOR_SCALE/255; s_palette[2] = 0*COLOR_SCALE/255;
			s_palette[4] = 200*COLOR_SCALE/255; s_palette[5] = 255*COLOR_SCALE/255; s_palette[6] = 0*COLOR_SCALE/255;
			s_palette[8] = 150*COLOR_SCALE/255; s_palette[9] = 255*COLOR_SCALE/255; s_palette[10] = 0*COLOR_SCALE/255;
			s_palette[12] = 0*COLOR_SCALE/255;	s_palette[13] = 225*COLOR_SCALE/255;s_palette[14] = 25*COLOR_SCALE/255;
			s_palette[16] = 0*COLOR_SCALE/255;	s_palette[17] = 164*COLOR_SCALE/255;s_palette[18] = 100*COLOR_SCALE/255;
			s_palette[20] = 0*COLOR_SCALE/255;	s_palette[21] = 97*COLOR_SCALE/255; s_palette[22] = 121*COLOR_SCALE/255;
			s_palette[24] = 0*COLOR_SCALE/255;	s_palette[25] = 0*COLOR_SCALE/255;	s_palette[26] = 152*COLOR_SCALE/255;
			s_palette[28] = 77*COLOR_SCALE/255;	s_palette[29] = 0*COLOR_SCALE/255;	s_palette[30] = 178*COLOR_SCALE/255;
			s_palette[32] = 115*COLOR_SCALE/255;s_palette[33] = 1*COLOR_SCALE/255;	s_palette[34] = 177*COLOR_SCALE/255;
			s_palette[36] = 153*COLOR_SCALE/255;s_palette[37] = 0*COLOR_SCALE/255;	s_palette[38] = 178*COLOR_SCALE/255;
			s_palette[40] = 205*COLOR_SCALE/255;s_palette[41] = 0*COLOR_SCALE/255;	s_palette[42] = 101*COLOR_SCALE/255;
			s_palette[44] = 234*COLOR_SCALE/255;s_palette[45] = 1*COLOR_SCALE/255;	s_palette[46] = 0*COLOR_SCALE/255;
			s_palette[48] = 255*COLOR_SCALE/255;s_palette[49] = 0*COLOR_SCALE/255;	s_palette[50] = 0*COLOR_SCALE/255;

			for(int i=3; i<s_nbColors*4; i+=4)
			{
				s_palette[i] = 0.2f;
			}
		}

		CSimple3DDatabase::~CSimple3DDatabase()
		{
		}

		boolean CSimple3DDatabase::set3DWidgetIdentifier(CIdentifier o3DWidgetIdentifier)
		{
			m_o3DWidgetIdentifier = o3DWidgetIdentifier;
			return true;
		}

		boolean CSimple3DDatabase::createVisualObject(CString sObjectName, EStandard3DObject eStandard3DObject)
		{
			m_bCreateObject = true;
			m_sCreateObjectName = sObjectName;
			m_eStandard3DObject = eStandard3DObject;
			return true;
		}

		boolean CSimple3DDatabase::createVisualObject(CString sObjectName, CString sObjectFileName)
		{
			m_bCreateObject = true;
			m_sCreateObjectName = sObjectName;
			m_sCreateObjectFileName = sObjectFileName;
			return true;
		}

		boolean CSimple3DDatabase::deleteVisualObject(CString sObjectName)
		{
			m_bDeleteObject = true;
			m_sDeleteObjectName = sObjectName;
			return true;
		}

		boolean CSimple3DDatabase::setVisualObjectPosition(CString sObjectName, float32 pPosition[3])
		{
			m_bSetObjectPosition = true;
			m_sPositionObjectName = sObjectName;
			memcpy(m_pPosition, pPosition, 3*sizeof(float32));
			return true;
		}
		/*
		boolean CSimple3DDatabase::setVisualObjectOrientation(CString sObjectName, ???)
		{
			m_bSetObjectOrientation = true;
			m_sOrientationObjectName = sObjectName;
			///???
			return true;
		}*/

		boolean CSimple3DDatabase::setVisualObjectScale(CString sObjectName, float32 f32ScaleX, float32 f32ScaleY, float32 f32ScaleZ)
		{
			m_bSetObjectScale = true;
			m_sScaleObjectName = sObjectName;
			m_f32ScaleX = f32ScaleX;
			m_f32ScaleY = f32ScaleY;
			m_f32ScaleZ = f32ScaleZ;
			return true;
		}

		boolean CSimple3DDatabase::setVisualObjectMaterialColor(CString sObjectName, float32 pColor[4])
		{
			m_bSetObjectMaterialColor = true;
			m_sColorObjectName = sObjectName;
			memcpy(m_pColor, pColor, 4*sizeof(float32));
			return true;
		}

		boolean CSimple3DDatabase::setVisualObjectVertexColor(CString sObjectName, float32 pColor[4])
		{
			m_bSetObjectVertexColor = true;
			m_sColorObjectName = sObjectName;
			memcpy(m_pColor, pColor, 4*sizeof(float32));
			return true;
		}

		boolean CSimple3DDatabase::repositionCamera()
		{
			m_bRepositionCamera = true;
			return true;
		}

		boolean CSimple3DDatabase::process3D()
		{
			if(m_bCreateObject == true)
			{
				CIdentifier l_oIdentifier;
				CNameValuePairList l_oParams;
				//clone objects so that changes to one object don't interfere with other similar objects
				l_oParams.setValue("CloneMeshes", true);
				l_oParams.setValue("CloneMaterials", true);
				//user might want to update vertex colors using the toolbar
				l_oParams.setValue("VertexBufferUsage", "Dynamic");

				if(m_eStandard3DObject != Standard3DObject_Invalid)
				{
					l_oIdentifier = m_oParentPlugin.getVisualisationContext().createObject(m_eStandard3DObject, &l_oParams);
				}
				else
				{
					l_oIdentifier = m_oParentPlugin.getVisualisationContext().createObject(m_sCreateObjectFileName, &l_oParams);
				}

				if(l_oIdentifier != OV_UndefinedIdentifier)
				{
					m_mSimulatedObjects[string(m_sCreateObjectName)] = l_oIdentifier;
				}
				m_eStandard3DObject = Standard3DObject_Invalid;
				m_sCreateObjectFileName = "";
				m_bCreateObject = false;
			}

			if(m_bDeleteObject == true)
			{
				if(m_mSimulatedObjects.find(string(m_sDeleteObjectName)) != m_mSimulatedObjects.end())
				{
					m_oParentPlugin.getVisualisationContext().removeObject(
						m_mSimulatedObjects[string(m_sDeleteObjectName)]);
					m_mSimulatedObjects.erase(string(m_sDeleteObjectName));
				}
				m_bDeleteObject = false;
			}

			if(m_bSetObjectPosition == true)
			{
				if(m_mSimulatedObjects.find(string(m_sPositionObjectName)) != m_mSimulatedObjects.end())
				{
					m_oParentPlugin.getVisualisationContext().setObjectPosition(
						m_mSimulatedObjects[string(m_sPositionObjectName)],
						m_pPosition[0],
						m_pPosition[1],
						m_pPosition[2]);
				}
				m_bSetObjectPosition = false;
			}

			if(m_bSetObjectOrientation == true)
			{
				if(m_mSimulatedObjects.find(string(m_sOrientationObjectName)) != m_mSimulatedObjects.end())
				{
					m_oParentPlugin.getVisualisationContext().setObjectOrientation(
						m_mSimulatedObjects[string(m_sOrientationObjectName)],
						m_pOrientation[0],
						m_pOrientation[1],
						m_pOrientation[2],
						m_pOrientation[3]);
				}
				m_bSetObjectOrientation = false;
			}

			if(m_bSetObjectScale == true)
			{
				if(m_mSimulatedObjects.find(string(m_sScaleObjectName)) != m_mSimulatedObjects.end())
				{
					m_oParentPlugin.getVisualisationContext().setObjectScale(
						m_mSimulatedObjects[string(m_sScaleObjectName)],
						m_f32ScaleX,
						m_f32ScaleY,
						m_f32ScaleZ);
				}
				m_bSetObjectScale = false;
			}

			//setting material color
			if(m_bSetObjectMaterialColor == true)
			{
				if(m_mSimulatedObjects.find(string(m_sColorObjectName)) != m_mSimulatedObjects.end())
				{
					//set new object color
					m_oParentPlugin.getVisualisationContext().setObjectColor(
						m_mSimulatedObjects[string(m_sColorObjectName)],
						m_pColor[0],
						m_pColor[1],
						m_pColor[2]);

					m_oParentPlugin.getVisualisationContext().setObjectTransparency(
						m_mSimulatedObjects[string(m_sColorObjectName)],
						m_pColor[3]);
				}
				m_bSetObjectMaterialColor = false;
			}
			else if(m_bSetObjectVertexColor == true) //setting vertex color
			{
				//retrieve number of vertices
				uint32 l_ui32NbVertices = 0;
				m_oParentPlugin.getVisualisationContext().getObjectVertexCount(
					m_mSimulatedObjects[string(m_sColorObjectName)],
					l_ui32NbVertices);

				//ensure number of vertices could be retrieved
				if(l_ui32NbVertices > 0)
				{
					//FIXME
					float32* l_pColorArray(new float32[l_ui32NbVertices*4]);

					//copy vertex color for each vertex
					for(uint32 i=0; i<l_ui32NbVertices; i++)
					{
						l_pColorArray[i*4] = m_pColor[0];
						l_pColorArray[i*4+1] = m_pColor[1];
						l_pColorArray[i*4+2] = m_pColor[2];
						l_pColorArray[i*4+3] = m_pColor[3];
					}

					m_oParentPlugin.getVisualisationContext().setObjectVertexColorArray(
						m_mSimulatedObjects[string(m_sColorObjectName)],
						l_ui32NbVertices,
						l_pColorArray);

					delete[] l_pColorArray;
				}
				m_bSetObjectVertexColor = false;
			}

			if(m_bRepositionCamera == true)
			{
				m_oParentPlugin.getVisualisationContext().setCameraToEncompassObjects(m_o3DWidgetIdentifier);
				m_bRepositionCamera = false;
			}

			return true;
		}
	};
};
