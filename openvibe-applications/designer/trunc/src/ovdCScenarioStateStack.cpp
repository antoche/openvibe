#include "ovdCScenarioStateStack.h"

#include <zlib.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEDesigner;
using namespace OpenViBEToolkit;

CScenarioStateStack::CScenarioStateStack(const IKernelContext& rKernelContext, CApplication& rApplication, IScenario& rScenario)
	:m_rKernelContext(rKernelContext)
	,m_rApplication(rApplication)
	,m_rScenario(rScenario)
	,m_ui32MaximumStateCount(0)
{
	m_itCurrentState=m_vStates.begin();
	m_ui32MaximumStateCount=m_rKernelContext.getConfigurationManager().expandAsUInteger("${Designer_UndoRedoStackSize}", 64);
}

CScenarioStateStack::~CScenarioStateStack(void)
{
	std::list < CMemoryBuffer* >::iterator itState;
	for(itState=m_vStates.begin(); itState!=m_vStates.end(); itState++)
	{
		delete *itState;
	}
}

boolean CScenarioStateStack::undo(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "enters undo\n";

	std::list < CMemoryBuffer* >::iterator itState=m_itCurrentState;
	if(itState==m_vStates.begin())
	{
		return false;
	}

	itState--;

	m_itCurrentState=itState;

	if(!this->restoreState(**m_itCurrentState))
	{
		return false;
	}

	m_rKernelContext.getLogManager() << LogLevel_Trace << "leaves undo\n";
	return true;
}

boolean CScenarioStateStack::redo(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "enters redo\n";

	std::list < CMemoryBuffer* >::iterator itState=m_itCurrentState;
	if(itState==m_vStates.end())
	{
		return false;
	}

	itState++;
	if(itState==m_vStates.end())
	{
		return false;
	}

	m_itCurrentState=itState;

	if(!this->restoreState(**m_itCurrentState))
	{
		return false;
	}

	m_rKernelContext.getLogManager() << LogLevel_Trace << "leaves redo\n";
	return true;
}

boolean CScenarioStateStack::snapshot(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Trace << "enters snapshot\n";

	CMemoryBuffer* l_pNewState=new CMemoryBuffer();

	if(!this->dumpState(*l_pNewState))
	{
		delete l_pNewState;
		return false;
	}

	if(m_itCurrentState!=m_vStates.end())
	{
		m_itCurrentState++;
	}

	while(m_itCurrentState!=m_vStates.end())
	{
		delete *m_itCurrentState;
		m_itCurrentState=m_vStates.erase(m_itCurrentState);
	}

	if(m_ui32MaximumStateCount!=0)
	{
		while(m_vStates.size() >= m_ui32MaximumStateCount)
		{
			m_vStates.erase(m_vStates.begin());
		}
	}

	m_vStates.push_back(l_pNewState);

	m_itCurrentState=m_vStates.end();
	m_itCurrentState--;

	m_rKernelContext.getLogManager() << LogLevel_Trace << "leaves snapshot\n";
	return true;
}

boolean CScenarioStateStack::restoreState(const IMemoryBuffer& rState)
{
	CMemoryBuffer l_oUncompressedMemoryBuffer;

	if(rState.getSize()==0)
	{
		return false;
	}

	CIdentifier l_oImporterIdentifier=m_rKernelContext.getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_XMLScenarioImporter);
	if(l_oImporterIdentifier==OV_UndefinedIdentifier)
	{
		return false;
	}

	IAlgorithmProxy* l_pImporter=&m_rKernelContext.getAlgorithmManager().getAlgorithm(l_oImporterIdentifier);
	if(!l_pImporter)
	{
		return false;
	}

	::uLongf l_ulSrcSize  =(::uLongf)rState.getSize()-sizeof(uLongf);
	::Bytef* l_pSrcBuffer =(::Bytef*)rState.getDirectPointer();

	::uLongf l_ulDestSize =*(::uLongf*)(rState.getDirectPointer()+rState.getSize()-sizeof(uLongf));
	l_oUncompressedMemoryBuffer.setSize(l_ulDestSize, true);
	::Bytef* l_pDestBuffer=(::Bytef*)l_oUncompressedMemoryBuffer.getDirectPointer();

	if(::uncompress(l_pDestBuffer, &l_ulDestSize, l_pSrcBuffer, l_ulSrcSize)!=Z_OK)
	{
		return false;
	}

	l_pImporter->initialize();

	TParameterHandler < const IMemoryBuffer* > ip_pMemoryBuffer(l_pImporter->getInputParameter(OVTK_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer));
	TParameterHandler < IScenario* > op_pScenario(l_pImporter->getOutputParameter(OVTK_Algorithm_ScenarioImporter_OutputParameterId_Scenario));

	CIdentifier l_oWidgetIdentifier;
	while(m_rScenario.getVisualisationTreeDetails().getNextVisualisationWidgetIdentifier(l_oWidgetIdentifier))
	{
		m_rScenario.getVisualisationTreeDetails().destroyHierarchy(l_oWidgetIdentifier);
		l_oWidgetIdentifier=OV_UndefinedIdentifier;
	}
	m_rScenario.getVisualisationTreeDetails().reloadTree();
	m_rScenario.clear();

	ip_pMemoryBuffer=&l_oUncompressedMemoryBuffer;
	op_pScenario=&m_rScenario;

	l_pImporter->process();
	l_pImporter->uninitialize();
	m_rKernelContext.getAlgorithmManager().releaseAlgorithm(*l_pImporter);

	m_rScenario.getVisualisationTreeDetails().reloadTree();

	return true;
}

boolean CScenarioStateStack::dumpState(IMemoryBuffer& rState)
{
	CMemoryBuffer l_oUncompressedMemoryBuffer;
	CMemoryBuffer l_oCompressedMemoryBuffer;

	CIdentifier l_oExporterIdentifier=m_rKernelContext.getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_XMLScenarioExporter);
	if(l_oExporterIdentifier==OV_UndefinedIdentifier)
	{
		return false;
	}

	IAlgorithmProxy* l_pExporter=&m_rKernelContext.getAlgorithmManager().getAlgorithm(l_oExporterIdentifier);
	if(!l_pExporter)
	{
		return false;
	}

	l_pExporter->initialize();

	TParameterHandler < const IScenario* > ip_pScenario(l_pExporter->getInputParameter(OVTK_Algorithm_ScenarioExporter_InputParameterId_Scenario));
	TParameterHandler < IMemoryBuffer* > op_pMemoryBuffer(l_pExporter->getOutputParameter(OVTK_Algorithm_ScenarioExporter_OutputParameterId_MemoryBuffer));

	ip_pScenario=&m_rScenario;
	op_pMemoryBuffer=&l_oUncompressedMemoryBuffer;

	l_pExporter->process();
	l_pExporter->uninitialize();
	m_rKernelContext.getAlgorithmManager().releaseAlgorithm(*l_pExporter);

#if 0
	FILE* pFile=fopen(m_rKernelContext.getConfigurationManager().expand("undo-redo-snapshot-$core{index}.xml").toASCIIString(), "wb");
	fwrite(l_oUncompressedMemoryBuffer.getDirectPointer(), l_oUncompressedMemoryBuffer.getSize(), 1, pFile);
	fclose(pFile);
#endif

	::uLongf l_ulSrcSize  =(::uLongf)l_oUncompressedMemoryBuffer.getSize();
	::Bytef* l_pSrcBuffer =(::Bytef*)l_oUncompressedMemoryBuffer.getDirectPointer();

	l_oCompressedMemoryBuffer.setSize(12+(uint64)(l_ulSrcSize*1.1), true);

	::uLongf l_ulDestSize =(::uLongf)l_oCompressedMemoryBuffer.getSize();
	::Bytef* l_pDestBuffer=(::Bytef*)l_oCompressedMemoryBuffer.getDirectPointer();

	if(::compress(l_pDestBuffer, &l_ulDestSize, l_pSrcBuffer, l_ulSrcSize)!=Z_OK)
	{
		return false;
	}

	rState.setSize(0, true);
	rState.append(l_oCompressedMemoryBuffer.getDirectPointer(), l_ulDestSize);
	rState.append((const uint8*)&l_ulSrcSize, sizeof(::uLongf));

	m_rKernelContext.getLogManager() << LogLevel_Debug << "Pushed compressed scenario state [uncompressed:" << (uint64)l_ulSrcSize << "|compressed:" << (uint64)l_ulDestSize << "|ratio:" << (float64)(l_ulDestSize*100./l_ulSrcSize) << "]\n";

	return true;
}
