#if defined TARGET_HAS_ThirdPartyMatlab

#include "ovpCMatlabHelper.h"


#include <system/Memory.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <mex.h>
//#include <engine.h>
#include <string>

#if defined OVP_OS_Windows
	#include <windows.h>
#endif

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEToolkit;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Matlab;

using namespace std;

#define boolean OpenViBE::boolean

//---------------------------------------------------------------------------------------------------------------

boolean CMatlabHelper::setStreamedMatrixInputHeader(uint32 ui32InputIndex, IMatrix * pMatrix)
{
	char l_sBuffer[32];
	CString l_sLabelList = "";
	CString l_sDimensionSizes = "";
	for(uint32 dim = 0; dim<pMatrix->getDimensionCount(); dim++)
	{
		sprintf(l_sBuffer, "%i",pMatrix->getDimensionSize(dim));
		l_sDimensionSizes = l_sDimensionSizes + CString(l_sBuffer) + CString(" ");
		l_sLabelList = l_sLabelList + CString("{");
		for(uint32 lab = 0; lab<pMatrix->getDimensionSize(dim); lab++)
		{
			l_sLabelList = l_sLabelList + CString("'")+ escapeMatlabString(CString(pMatrix->getDimensionLabel(dim,lab))) + CString("' ");
		}
		l_sLabelList = l_sLabelList + CString("} ");
	}
	//function box_out = OV_setStreamedMatrixInputInputHeader(box_in, input_index, dimension_count, dimension_sizes, dimension_labels)

	
	sprintf(l_sBuffer, "%i",(ui32InputIndex+1));
	CString l_sCommand = "";
	l_sCommand = l_sCommand + m_sBoxInstanceVariableName + CString(" = OV_setStreamedMatrixInputHeader(") + m_sBoxInstanceVariableName + ",";
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(",");
	sprintf(l_sBuffer, "%i",pMatrix->getDimensionCount());
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(",");
	l_sCommand = l_sCommand + CString("[")+ l_sDimensionSizes + CString("],");
	l_sCommand = l_sCommand + CString("{")+ l_sLabelList + CString("});");
	
	return engEvalString(m_pMatlabEngine, (const char *)l_sCommand) == 0;
	
}

boolean CMatlabHelper::setFeatureVectorInputHeader(uint32 ui32InputIndex, IMatrix * pMatrix)
{
	CString l_sLabelList = "";
	CString l_sDimensionSizes = "";
	
	for(uint32 i = 0; i<pMatrix->getDimensionSize(0); i++)
	{
			l_sLabelList = l_sLabelList + CString("'")+ escapeMatlabString(CString(pMatrix->getDimensionLabel(0,i))) + CString("' ");
	}
	//function box_out = OV_setStreamedMatrixInputInputHeader(box_in, input_index, dimension_count, dimension_sizes, dimension_labels)

	char l_sBuffer[32];
	sprintf(l_sBuffer, "%i",(ui32InputIndex+1));
	CString l_sCommand = "";
	//box_out = OV_setFeatureVectorInputHeader(box_in, input_index, nb_features, labels)
	l_sCommand = l_sCommand + m_sBoxInstanceVariableName + CString(" = OV_setFeatureVectorInputHeader(") + m_sBoxInstanceVariableName + ",";
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(",");
	sprintf(l_sBuffer, "%i",pMatrix->getDimensionSize(0));
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(",");
	l_sCommand = l_sCommand + CString("{")+ l_sLabelList + CString("});");

	return engEvalString(m_pMatlabEngine, (const char *)l_sCommand) == 0;
}

boolean CMatlabHelper::setSignalInputHeader(uint32 ui32InputIndex, IMatrix * pMatrix, uint64 ui64SamplingRate)
{
	CString l_sLabelList = "";
	for(uint32 l = 0; l<pMatrix->getDimensionSize(0); l++)
	{
		l_sLabelList = l_sLabelList + CString("'")+ escapeMatlabString(CString(pMatrix->getDimensionLabel(0,l))) + CString("' ");
	}
	//function box_out = ov_set_signal_input_header(box_in,  input_index, nb_channel, nb_samples_per_buffer, channel_names, sampling_rate)
	char l_sBuffer[32];
	sprintf(l_sBuffer, "%i",(ui32InputIndex+1));
	CString l_sCommand = "";
	l_sCommand = l_sCommand + m_sBoxInstanceVariableName + CString(" = OV_setSignalInputHeader(") + m_sBoxInstanceVariableName + ",";
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(",");
	sprintf(l_sBuffer, "%i",pMatrix->getDimensionSize(0));
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(",");
	sprintf(l_sBuffer, "%i",pMatrix->getDimensionSize(1));
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(",");
	l_sCommand = l_sCommand + CString("{")+ l_sLabelList + CString("},");
	sprintf(l_sBuffer, "%i",ui64SamplingRate);
	l_sCommand = l_sCommand + CString(l_sBuffer)+ CString(");");

	return engEvalString(m_pMatlabEngine, (const char *) l_sCommand) == 0;

}

boolean CMatlabHelper::setChannelLocalisationInputHeader(uint32 ui32InputIndex, IMatrix * pMatrix, boolean bDynamic)
{
	CString l_sLabelList = "";
	for(uint32 l = 0; l<pMatrix->getDimensionSize(0); l++)
	{
		l_sLabelList = l_sLabelList + CString("'")+ escapeMatlabString(CString(pMatrix->getDimensionLabel(0,l))) + CString("' ");
	}
	//function  box_out = OV_setChannelLocalisationInputHeader(box_in, input_index, nb_channels, channel_names, dynamic)
	char l_sBuffer[32];
	sprintf(l_sBuffer, "%i",(ui32InputIndex+1));
	CString l_sCommand = "";
	l_sCommand = l_sCommand + m_sBoxInstanceVariableName + CString(" = OV_setChannelLocalisationInputHeader(") + m_sBoxInstanceVariableName + ",";
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(",");
	sprintf(l_sBuffer, "%i",pMatrix->getDimensionSize(0));
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(",");
	l_sCommand = l_sCommand + CString("{")+ l_sLabelList + CString("},");
	sprintf(l_sBuffer, "%s",(bDynamic?"true":"false"));
	l_sCommand = l_sCommand + CString(l_sBuffer)+ CString(");");
	
	return engEvalString(m_pMatlabEngine, (const char *) l_sCommand) == 0;
}

boolean CMatlabHelper::setSpectrumInputHeader(uint32 ui32InputIndex, IMatrix * pMatrix, IMatrix * pFrequencyBands)
{
	CString l_sLabelList = "";
	for(uint32 l = 0; l<pMatrix->getDimensionSize(0); l++)
	{
		l_sLabelList = l_sLabelList + CString("'")+ escapeMatlabString(CString(pMatrix->getDimensionLabel(0,l))) + CString("' ");
	}

	CString l_sBandList = "";
	for(uint32 l = 0; l<pMatrix->getDimensionSize(1); l++)
	{
		l_sBandList = l_sBandList + CString("'")+ escapeMatlabString(CString(pMatrix->getDimensionLabel(1,l))) + CString("' ");
	}

	mxArray * l_pMatlabMatrix=::mxCreateDoubleMatrix(
			pFrequencyBands->getDimensionSize(1),
			pFrequencyBands->getDimensionSize(0),
			mxREAL);
					
	System::Memory::copy(::mxGetPr(l_pMatlabMatrix), pFrequencyBands->getBuffer(), pFrequencyBands->getBufferElementCount()*sizeof(float64));
	::engPutVariable(m_pMatlabEngine, "OV_MATRIX_TMP", l_pMatlabMatrix);


	//box_out = OV_setSpectrumInputHeader(box_in, input_index, nb_channels, channel_names, nb_bands, band_names, bands)
	char l_sBuffer[32];
	sprintf(l_sBuffer, "%i",(ui32InputIndex+1));
	CString l_sCommand = "";
	l_sCommand = l_sCommand + m_sBoxInstanceVariableName + CString(" = OV_setSpectrumInputHeader(") + m_sBoxInstanceVariableName + ",";
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(","); // input_index
	sprintf(l_sBuffer, "%i",pMatrix->getDimensionSize(0));
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(","); // nb_channels
	l_sCommand = l_sCommand + CString("{")+ l_sLabelList + CString("},"); // channel_names
	sprintf(l_sBuffer, "%i",pMatrix->getDimensionSize(1));
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(","); // nb_bands
	l_sCommand = l_sCommand + CString("{")+ l_sBandList + CString("},"); //band names
	l_sCommand = l_sCommand + CString("OV_MATRIX_TMP);"); //bands
	
	return engEvalString(m_pMatlabEngine, (const char *) l_sCommand) == 0;
}

boolean CMatlabHelper::setStimulationsInputHeader(uint32 ui32InputIndex)
{
	
	//box_out = OV_setStimulationInputHeader(box_in, input_index)
	char l_sBuffer[32];
	sprintf(l_sBuffer, "%i",(ui32InputIndex+1));
	CString l_sCommand = "";
	l_sCommand = l_sCommand + m_sBoxInstanceVariableName + CString(" = OV_setStimulationsInputHeader(") + m_sBoxInstanceVariableName + ",";
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(");");
	
	return engEvalString(m_pMatlabEngine, (const char *) l_sCommand) == 0;
}

//boolean setExperimentInformationInputHeader(IMatrix * pMatrix);

//---------------------------------------------------------------------------------------------------------------

boolean CMatlabHelper::addStreamedMatrixInputBuffer(uint32 ui32InputIndex, IMatrix * pMatrix, uint64 ui64OpenvibeStartTime,uint64 ui64OpenvibeEndTime)
{
	mwSize* l_pDims = new mwSize[pMatrix->getDimensionCount()];
	uint32 j = pMatrix->getDimensionCount()-1;
	for(uint32 i =0; i < pMatrix->getDimensionCount();i++)
	{
		l_pDims[i] = pMatrix->getDimensionSize(j);
		j--;
	}
	
	mxArray * l_pMatlabMatrix=::mxCreateNumericArray(
		pMatrix->getDimensionCount(),
		l_pDims,
		mxDOUBLE_CLASS,
		mxREAL);

	//test : channel 1 samples to '10'
	//for(uint32 i = 0; i < 32;i++) pMatrix->getBuffer()[i] = 10;

	System::Memory::copy(::mxGetPr(l_pMatlabMatrix), pMatrix->getBuffer(), pMatrix->getBufferElementCount()*sizeof(float64));
	::engPutVariable(m_pMatlabEngine, "OV_MATRIX_TMP", l_pMatlabMatrix);

	char l_sBuffer[32];
	sprintf(l_sBuffer, "%i",(ui32InputIndex+1));
	CString l_sCommand = "";
	l_sCommand = l_sCommand + m_sBoxInstanceVariableName + CString(" = OV_addInputBuffer(") + m_sBoxInstanceVariableName + ",";
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(",");
	sprintf(l_sBuffer, "%f",(ui64OpenvibeStartTime>>16)/65536.);
	//sprintf(l_sBuffer, "%f",ui64OpenvibeStartTime/4294967296.);
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(",");
	sprintf(l_sBuffer, "%f",(ui64OpenvibeEndTime>>16)/65536.);
	//sprintf(l_sBuffer, "%f",ui64OpenvibeEndTime/4294967296.);
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(",OV_MATRIX_TMP');");
	// please note the transpose operator ' to put the matrix with  1 channel per line

	delete l_pDims;

	return ::engEvalString(m_pMatlabEngine, (const char*)l_sCommand) == 0;
}

boolean CMatlabHelper::addStimulationsInputBuffer(uint32 ui32InputIndex, IStimulationSet * pStimulationSet, uint64 ui64OpenvibeStartTime,uint64 ui64OpenvibeEndTime)
{
	if(pStimulationSet->getStimulationCount() == 0)
	{
		if(::engEvalString(m_pMatlabEngine, "OV_MATRIX_TMP = 0") != 0) return false;
	}
	else
	{
		// we create a 3xN matrix for N stims (access is easier in that order)
		mxArray * l_pMatlabMatrix=::mxCreateDoubleMatrix(
			3,
			(uint32)pStimulationSet->getStimulationCount(),
			mxREAL);

		for(uint32 i = 0; i<pStimulationSet->getStimulationCount(); i++)
		{
			::mxGetPr(l_pMatlabMatrix)[i*3]   =  (double)pStimulationSet->getStimulationIdentifier(i);
			::mxGetPr(l_pMatlabMatrix)[i*3+1] =  (pStimulationSet->getStimulationDate(i)>>16)/65536.;
			::mxGetPr(l_pMatlabMatrix)[i*3+2] =  (pStimulationSet->getStimulationDuration(i)>>16)/65536.;
		}

		::engPutVariable(m_pMatlabEngine, "OV_MATRIX_TMP", l_pMatlabMatrix);
	}

	char l_sBuffer[32];
	sprintf(l_sBuffer, "%i",(ui32InputIndex+1));
	CString l_sCommand = "";
	l_sCommand = l_sCommand + m_sBoxInstanceVariableName + CString(" = OV_addInputBuffer(") + m_sBoxInstanceVariableName + ",";
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(",");
	sprintf(l_sBuffer, "%f",(ui64OpenvibeStartTime>>16)/65536.);
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(",");
	sprintf(l_sBuffer, "%f",(ui64OpenvibeEndTime>>16)/65536.);
	l_sCommand = l_sCommand + CString(l_sBuffer) + CString(",OV_MATRIX_TMP);");

	boolean res = ::engEvalString(m_pMatlabEngine, (const char*)l_sCommand) == 0;

	return res;
}

//---------------------------------------------------------------------------------------------------------------

boolean CMatlabHelper::setStreamedMatrixOutputHeader(uint32 ui32OutputIndex, IMatrix * pMatrix)
{
	char l_sBuffer[32];
	sprintf(l_sBuffer, "%i",(ui32OutputIndex+1));
	CString l_sCommand = "[OV_ERRNO, OV_NB_DIMENSIONS, OV_DIMENSION_SIZES, OV_DIMENSION_LABELS] = OV_getStreamedMatrixOutputHeader(" + m_sBoxInstanceVariableName + "," + CString(l_sBuffer) + ");";
	uint32 l_ui32Result = ::engEvalString(m_pMatlabEngine, (const char*)l_sCommand);
	if(l_ui32Result != 0)
	{
		return false;
	}

	mxArray * l_pErrno           = ::engGetVariable(m_pMatlabEngine,"OV_ERRNO");
	uint32 l_ui32Errno = (uint32) *mxGetPr(l_pErrno);
	if(l_ui32Errno != 0)
	{
		return false;
	}

	
	mxArray * l_pNbDimensions    = ::engGetVariable(m_pMatlabEngine,"OV_NB_DIMENSIONS");
	mxArray * l_pDimensionSizes  = ::engGetVariable(m_pMatlabEngine,"OV_DIMENSION_SIZES");
	mxArray * l_pDimensionLabels = ::engGetVariable(m_pMatlabEngine,"OV_DIMENSION_LABELS");

	
	uint32 l_ui32NbDimensions = (uint32) *mxGetPr(l_pNbDimensions);
	mwSize l_oNbCells = mxGetNumberOfElements(l_pDimensionLabels);

	char ** l_pNameList = new char*[l_oNbCells];
	for(uint32 cell = 0; cell < l_oNbCells; cell++)
	{
		mxArray * l_pCell = mxGetCell(l_pDimensionLabels, cell);
		const mwSize * l_pCellSizes  = mxGetDimensions(l_pCell);
		char * l_sName = new char[l_pCellSizes[1]+1];
		l_sName[l_pCellSizes[1]] = '\0';
		for(uint32 cellsize = 0; cellsize < (uint32)l_pCellSizes[1]; cellsize++)
		{
			l_sName[cellsize] = ((char *)::mxGetData(l_pCell))[cellsize*2];
		}
		l_pNameList[cell] = l_sName;
	}
	
	
	pMatrix->setDimensionCount(l_ui32NbDimensions);
	uint32 l_ui32Index = 0;
	for(uint32 i = 0; i<l_ui32NbDimensions;i++)
	{
		pMatrix->setDimensionSize(i,(uint32) mxGetPr(l_pDimensionSizes)[i]);
		for(uint32 x = 0; x<pMatrix->getDimensionSize(i); x++)
		{
			CString l_sNameTmp = l_pNameList[l_ui32Index];
			pMatrix->setDimensionLabel(i,x,escapeMatlabString(l_sNameTmp));
			l_ui32Index++;
		}
	}

	delete[] l_pNameList;
	mxDestroyArray(l_pNbDimensions);
	mxDestroyArray(l_pDimensionSizes);
	mxDestroyArray(l_pDimensionLabels);

	return true;
}

boolean CMatlabHelper::setFeatureVectorOutputHeader(uint32 ui32OutputIndex, IMatrix * pMatrix)
{
	char l_sBuffer[32];
	sprintf(l_sBuffer, "%i",(ui32OutputIndex+1));
	CString l_sCommand = "[OV_ERRNO, OV_NB_FEATURES, OV_LABELS] = OV_getFeatureVectorOutputHeader(" + m_sBoxInstanceVariableName + "," + CString(l_sBuffer) + ");";
	uint32 l_ui32Result = ::engEvalString(m_pMatlabEngine, (const char*)l_sCommand);
	if(l_ui32Result != 0)
	{
		return false;
	}
	mxArray * l_pErrno           = ::engGetVariable(m_pMatlabEngine,"OV_ERRNO");
	uint32 l_ui32Errno = (uint32) *mxGetPr(l_pErrno);
	if(l_ui32Errno != 0)
	{
		return false;
	}

	mxArray * l_pNbFeatures = ::engGetVariable(m_pMatlabEngine,"OV_NB_FEATURES");
	mxArray * l_pNames      = ::engGetVariable(m_pMatlabEngine,"OV_LABELS");

	uint32 l_ui32NbFeatures = (uint32) *mxGetPr(l_pNbFeatures);
	mwSize l_oNbCells = mxGetNumberOfElements(l_pNames);
	char ** l_pNameList = new char*[l_oNbCells];
	for(uint32 cell = 0; cell < l_oNbCells; cell++)
	{
		mxArray * l_pCell = mxGetCell(l_pNames, cell);
		const mwSize * l_pCellSizes  = mxGetDimensions(l_pCell);
		char * l_sName = new char[l_pCellSizes[1]+1];
		l_sName[l_pCellSizes[1]] = '\0';
		for(uint32 cellsize = 0; cellsize < (uint32)l_pCellSizes[1]; cellsize++)
		{
			l_sName[cellsize] = ((char *)::mxGetData(l_pCell))[cellsize*2];
		}
		l_pNameList[cell] = l_sName;
	}
				
				
	pMatrix->setDimensionCount(1);
	pMatrix->setDimensionSize(0,l_ui32NbFeatures);
	for(uint32 x=0;x<l_ui32NbFeatures;x++)
	{
		CString l_sNameTmp = l_pNameList[x];
		pMatrix->setDimensionLabel(0,x,escapeMatlabString(l_sNameTmp));
	}

	delete[] l_pNameList;
	mxDestroyArray(l_pNbFeatures);
	mxDestroyArray(l_pNames);
	
	return true;
}

boolean CMatlabHelper::setSignalOutputHeader(uint32 ui32OutputIndex, IMatrix * pMatrix, uint64& rSamplingFrequency)
{
	char l_sBuffer[32];
	sprintf(l_sBuffer, "%i",(ui32OutputIndex+1));
	CString l_sCommand = "[OV_ERRNO, OV_NB_CHANNELS, OV_NB_SAMPLES_PER_BUFFER, OV_CHANNEL_NAMES, OV_SAMPLING_RATE] = OV_getSignalOutputHeader(" + m_sBoxInstanceVariableName + "," + CString(l_sBuffer) + ");";
	uint32 l_ui32Result = ::engEvalString(m_pMatlabEngine, (const char*)l_sCommand);
	if(l_ui32Result != 0)
	{
		return false;
	}
	
	mxArray * l_pErrno           = ::engGetVariable(m_pMatlabEngine,"OV_ERRNO");
	uint32 l_ui32Errno = (uint32) *mxGetPr(l_pErrno);
	if(l_ui32Errno != 0)
	{
		return false;
	}

	mxArray * l_pNbChannels = ::engGetVariable(m_pMatlabEngine,"OV_NB_CHANNELS");
	mxArray * l_pNbSamples  = ::engGetVariable(m_pMatlabEngine,"OV_NB_SAMPLES_PER_BUFFER");
	mxArray * l_pNames      = ::engGetVariable(m_pMatlabEngine,"OV_CHANNEL_NAMES");
	mxArray * l_pRate       = ::engGetVariable(m_pMatlabEngine,"OV_SAMPLING_RATE");

	uint32 l_ui32NbChannels = (uint32) *mxGetPr(l_pNbChannels);
	uint32 l_ui32NbSamples  = (uint32) *mxGetPr(l_pNbSamples);
	uint32 l_ui32Rate     = (uint32) *mxGetPr(l_pRate);
	mwSize l_oNbCells = mxGetNumberOfElements(l_pNames);
	if(l_oNbCells != l_ui32NbChannels)
	{
		return false;
	}
	char ** l_pNameList = new char*[l_oNbCells];
	for(uint32 cell = 0; cell < l_oNbCells; cell++)
	{
		mxArray * l_pCell = mxGetCell(l_pNames, cell);
		const mwSize * l_pCellSizes  = mxGetDimensions(l_pCell);
		char * l_sName = new char[l_pCellSizes[1]+1];
		l_sName[l_pCellSizes[1]] = '\0';
		for(uint32 cellsize = 0; cellsize < (uint32)l_pCellSizes[1]; cellsize++)
		{
			l_sName[cellsize] = ((char *)::mxGetData(l_pCell))[cellsize*2];
		}
		l_pNameList[cell] = l_sName;
	}
				
				
	pMatrix->setDimensionCount(2);
	pMatrix->setDimensionSize(0,l_ui32NbChannels);
	pMatrix->setDimensionSize(1,l_ui32NbSamples);
	rSamplingFrequency = l_ui32Rate;
	for(uint32 x=0;x<l_ui32NbChannels;x++)
	{
		CString l_sNameTmp = l_pNameList[x];
		pMatrix->setDimensionLabel(0,x,escapeMatlabString(l_sNameTmp));
	}

	delete[] l_pNameList;
	mxDestroyArray(l_pNbChannels);
	mxDestroyArray(l_pNbSamples);
	mxDestroyArray(l_pNames);
	mxDestroyArray(l_pRate);

	return true;

}

boolean CMatlabHelper::setChannelLocalisationOutputHeader(uint32 ui32OutputIndex, IMatrix * pMatrix, boolean& bDynamic)
{
	char l_sBuffer[32];
	sprintf(l_sBuffer, "%i",(ui32OutputIndex+1));
	CString l_sCommand = "[OV_ERRNO, OV_NB_CHANNELS, OV_CHANNEL_NAMES, OV_DYNAMIC] = OV_getChannelLocalisationOutputHeader(" + m_sBoxInstanceVariableName + "," + CString(l_sBuffer) + ");";
	uint32 l_ui32Result = ::engEvalString(m_pMatlabEngine, (const char*)l_sCommand);
	if(l_ui32Result != 0)
	{
		return false;
	}

	mxArray * l_pErrno           = ::engGetVariable(m_pMatlabEngine,"OV_ERRNO");
	uint32 l_ui32Errno = (uint32) *mxGetPr(l_pErrno);
	if(l_ui32Errno != 0)
	{
		return false;
	}

	mxArray * l_pNbChannels = ::engGetVariable(m_pMatlabEngine,"OV_NB_CHANNELS");
	mxArray * l_pNames      = ::engGetVariable(m_pMatlabEngine,"OV_CHANNEL_NAMES");
	mxArray * l_pDynamic    = ::engGetVariable(m_pMatlabEngine,"OV_DYNAMIC");

	uint32 l_ui32NbChannels = (uint32) *mxGetPr(l_pNbChannels);
	boolean l_ui32Dynamic   = ((uint32) *mxGetPr(l_pDynamic) == 1);
	mwSize l_oNbCells = mxGetNumberOfElements(l_pNames);
	char ** l_pNameList = new char*[l_oNbCells];
	for(uint32 cell = 0; cell < l_oNbCells; cell++)
	{
		mxArray * l_pCell = mxGetCell(l_pNames, cell);
		const mwSize * l_pCellSizes  = mxGetDimensions(l_pCell);
		char * l_sName = new char[l_pCellSizes[1]+1];
		l_sName[l_pCellSizes[1]] = '\0';
		for(uint32 cellsize = 0; cellsize < (uint32)l_pCellSizes[1]; cellsize++)
		{
			l_sName[cellsize] = ((char *)::mxGetData(l_pCell))[cellsize*2];
		}
		l_pNameList[cell] = l_sName;
	}
				
				
	pMatrix->setDimensionCount(2);
	pMatrix->setDimensionSize(0,l_ui32NbChannels);
	pMatrix->setDimensionSize(1,3);

	for(uint32 x=0;x<l_ui32NbChannels;x++)
	{
		CString l_sNameTmp = l_pNameList[x];
		pMatrix->setDimensionLabel(0,x,escapeMatlabString(l_sNameTmp));
	}

	delete[] l_pNameList;

	mxDestroyArray(l_pNbChannels);
	mxDestroyArray(l_pNames);
	mxDestroyArray(l_pDynamic);

	return true;
}

boolean CMatlabHelper::setSpectrumOutputHeader(uint32 ui32OutputIndex, IMatrix * pMatrix, IMatrix * pFrequencyBands)
{
	char l_sBuffer[32];
	sprintf(l_sBuffer, "%i",(ui32OutputIndex+1));
	CString l_sCommand = "[OV_ERRNO, OV_NB_CHANNELS, OV_CHANNEL_NAMES, OV_NB_BANDS, OV_BANDS_NAME, OV_BANDS_LINEAR] = OV_getSpectrumOutputHeader(" + m_sBoxInstanceVariableName + "," + CString(l_sBuffer) + ");";
	uint32 l_ui32Result = ::engEvalString(m_pMatlabEngine, (const char*)l_sCommand);
	if(l_ui32Result != 0)
	{
		return false;
	}

	mxArray * l_pErrno           = ::engGetVariable(m_pMatlabEngine,"OV_ERRNO");
	uint32 l_ui32Errno = (uint32) *mxGetPr(l_pErrno);
	if(l_ui32Errno != 0)
	{
		return false;
	}

	mxArray * l_pNbChannels = ::engGetVariable(m_pMatlabEngine,"OV_NB_CHANNELS");
	mxArray * l_pNames      = ::engGetVariable(m_pMatlabEngine,"OV_CHANNEL_NAMES");
	mxArray * l_pNbBands    = ::engGetVariable(m_pMatlabEngine,"OV_NB_BANDS");
	mxArray * l_pBandNames  = ::engGetVariable(m_pMatlabEngine,"OV_BANDS_NAME");
	mxArray * l_pBands      = ::engGetVariable(m_pMatlabEngine,"OV_BANDS_LINEAR");

	uint32 l_ui32NbChannels = (uint32) *mxGetPr(l_pNbChannels);
	uint32 l_ui32NbBands    = (uint32) *mxGetPr(l_pNbBands);
	
	// getting the channel names
	mwSize l_oNbCells = mxGetNumberOfElements(l_pNames);
	char ** l_pNameList = new char*[l_oNbCells];
	for(uint32 cell = 0; cell < l_oNbCells; cell++)
	{
		mxArray * l_pCell = mxGetCell(l_pNames, cell);
		const mwSize * l_pCellSizes  = mxGetDimensions(l_pCell);
		char * l_sName = new char[l_pCellSizes[1]+1];
		l_sName[l_pCellSizes[1]] = '\0';
		for(uint32 cellsize = 0; cellsize < (uint32)l_pCellSizes[1]; cellsize++)
		{
			l_sName[cellsize] = ((char *)::mxGetData(l_pCell))[cellsize*2];
		}
		l_pNameList[cell] = l_sName;
	}

	// now getting the band names
	l_oNbCells = mxGetNumberOfElements(l_pBandNames);
	char ** l_pBandNameList = new char*[l_oNbCells];
	for(uint32 cell = 0; cell < l_oNbCells; cell++)
	{
		mxArray * l_pCell = mxGetCell(l_pBandNames, cell);
		const mwSize * l_pCellSizes  = mxGetDimensions(l_pCell);
		char * l_sBandName = new char[l_pCellSizes[1]+1];
		l_sBandName[l_pCellSizes[1]] = '\0';
		for(uint32 cellsize = 0; cellsize < (uint32)l_pCellSizes[1]; cellsize++)
		{
			l_sBandName[cellsize] = ((char *)::mxGetData(l_pCell))[cellsize*2];
		}
		l_pBandNameList[cell] = l_sBandName;
	}

	//The Frequency band list has dimensions nb_bands x 2 values
	pFrequencyBands->setDimensionCount(2);
	pFrequencyBands->setDimensionSize(0,2);
	pFrequencyBands->setDimensionSize(1,l_ui32NbBands);
	for(uint32 x=0;x<l_ui32NbChannels;x++)
	{
		CString l_sNameTmp = l_pNameList[x];
		pFrequencyBands->setDimensionLabel(0,x,escapeMatlabString(l_sNameTmp));
	}
	pFrequencyBands->setDimensionLabel(1,0,"Min");
	pFrequencyBands->setDimensionLabel(1,1,"Max");
	// Adding the bands:
	System::Memory::copy(pFrequencyBands->getBuffer(), ::mxGetPr(l_pBands), (uint64)l_ui32NbBands*sizeof(float64)*2);

	pMatrix->setDimensionCount(2);
	pMatrix->setDimensionSize(0,l_ui32NbChannels);
	pMatrix->setDimensionSize(1,l_ui32NbBands);
	for(uint32 x=0;x<l_ui32NbChannels;x++)
	{
		CString l_sNameTmp = l_pNameList[x];
		pMatrix->setDimensionLabel(0,x,escapeMatlabString(l_sNameTmp));
	}
	for(uint32 x=0;x<l_ui32NbBands;x++)
	{
		CString l_sNameTmp = l_pBandNameList[x];
		pMatrix->setDimensionLabel(1,x,escapeMatlabString(l_sNameTmp));
	}

	delete[] l_pNameList;
	delete[] l_pBandNameList;

	mxDestroyArray(l_pNbChannels);
	mxDestroyArray(l_pNames);
	mxDestroyArray(l_pNbBands);
	mxDestroyArray(l_pBandNames);
	mxDestroyArray(l_pBands);

	return true;
}

boolean CMatlabHelper::setStimulationsOutputHeader(uint32 ui32OutputIndex, IStimulationSet * pStimulationSet)
{
	// Nothing to do, the stimulation header is empty.
	return true;
}

//---------------------------------------------------------------------------------------------------------------

boolean CMatlabHelper::popStreamedMatrixOutputBuffer(uint32 ui32OutputIndex, IMatrix * pMatrix, OpenViBE::uint64& rStartTime, OpenViBE::uint64& rEndTime)
{
	char l_sBuffer[256];
	sprintf(l_sBuffer,"[%s, OV_START_TIME, OV_END_TIME, OV_LINEAR_DATA_SIZE, OV_LINEAR_DATA] = OV_popOutputBufferReshape(%s, %i);",(const char*)m_sBoxInstanceVariableName,(const char*)m_sBoxInstanceVariableName,ui32OutputIndex+1);
	uint32 l_ui32Result = ::engEvalString(m_pMatlabEngine, l_sBuffer);
	if(l_ui32Result != 0)
	{
		return false;
	}

	mxArray * l_pStartTime = ::engGetVariable(m_pMatlabEngine,"OV_START_TIME");
	mxArray * l_pEndTime   = ::engGetVariable(m_pMatlabEngine,"OV_END_TIME");
	mxArray * l_pSize      = ::engGetVariable(m_pMatlabEngine,"OV_LINEAR_DATA_SIZE");
	mxArray * l_pData      = ::engGetVariable(m_pMatlabEngine,"OV_LINEAR_DATA");

	uint64 l_ui64StartTime  = (uint64) ( ((float64*)::mxGetPr(l_pStartTime))[0] * 65536.);
	l_ui64StartTime = l_ui64StartTime << 16;
	uint64 l_ui64EndTime = (uint64) ( ((float64*)::mxGetPr(l_pEndTime))[0] * 65536.);
	l_ui64EndTime = l_ui64EndTime << 16;
	
	// ti be copied direclty in openvibe buffer, the linear matrix must be ordered line by line
	System::Memory::copy(pMatrix->getBuffer(), ::mxGetPr(l_pData), (uint64)*mxGetPr(l_pSize)*sizeof(float64));
	rStartTime = l_ui64StartTime;
	rEndTime = l_ui64EndTime;

	mxDestroyArray(l_pStartTime);
	mxDestroyArray(l_pEndTime);
	mxDestroyArray(l_pData);
	mxDestroyArray(l_pSize);

	return true;
}

boolean CMatlabHelper::popStimulationsOutputBuffer(uint32 ui32OutputIndex, IStimulationSet * pStimulationSet, OpenViBE::uint64& rStartTime, OpenViBE::uint64& rEndTime)
{
	char l_sBuffer[256];
	sprintf(l_sBuffer,"[%s, OV_START_TIME, OV_END_TIME, OV_LINEAR_MATRIX_SIZE, OV_LINEAR_DATA] = OV_popOutputBuffer(%s, %i);",(const char*)m_sBoxInstanceVariableName,(const char*)m_sBoxInstanceVariableName,ui32OutputIndex+1);
	uint32 l_ui32Result = ::engEvalString(m_pMatlabEngine, l_sBuffer);
	if(l_ui32Result != 0)
	{
		return false;
	}

	mxArray * l_pStartTime = ::engGetVariable(m_pMatlabEngine,"OV_START_TIME");
	mxArray * l_pEndTime   = ::engGetVariable(m_pMatlabEngine,"OV_END_TIME");
	mxArray * l_pSize      = ::engGetVariable(m_pMatlabEngine,"OV_LINEAR_MATRIX_SIZE");
	mxArray * l_pData      = ::engGetVariable(m_pMatlabEngine,"OV_LINEAR_DATA");

	uint64 l_ui64StartTime  = (uint64) ( ((float64*)::mxGetPr(l_pStartTime))[0] * 65536.);
	l_ui64StartTime = l_ui64StartTime << 16;
	uint64 l_ui64EndTime = (uint64) ( ((float64*)::mxGetPr(l_pEndTime))[0] * 65536.);
	l_ui64EndTime = l_ui64EndTime << 16;
	
	rStartTime = l_ui64StartTime;
	rEndTime = l_ui64EndTime;

	for(uint32 i = 0; i < (uint32)(*::mxGetPr(l_pSize)); i+=3)
	{
		uint64 l_ui64Identifier = (uint64) ( ((float64*)::mxGetPr(l_pData))[i]);
		uint64 l_ui64Date       = (uint64) ( ((float64*)::mxGetPr(l_pData))[i+1] * 65536.);
		l_ui64Date = l_ui64Date << 16;
		uint64 l_ui64Duration   = (uint64) ( ((float64*)::mxGetPr(l_pData))[i+2] * 65536.);
		l_ui64Duration = l_ui64Duration << 16;
		pStimulationSet->appendStimulation(l_ui64Identifier,l_ui64Date, l_ui64Duration);
	}

	mxDestroyArray(l_pStartTime);
	mxDestroyArray(l_pEndTime);
	mxDestroyArray(l_pData);
	mxDestroyArray(l_pSize);

	return true;
}

//---------------------------------------------------------------------------------------------------------------

CString CMatlabHelper::escapeMatlabString(CString sStringToEscape)
{
	string str = string((const char *)sStringToEscape);
	string::iterator it = str.begin();
	while(it != str.end())
	{
		if( *it =='\'')
		{
			str.insert(it,'\'');
			it++;
		}
		it++;
	}
	return CString(str.c_str());
}
#endif // TARGET_HAS_ThirdPartyMatlab
