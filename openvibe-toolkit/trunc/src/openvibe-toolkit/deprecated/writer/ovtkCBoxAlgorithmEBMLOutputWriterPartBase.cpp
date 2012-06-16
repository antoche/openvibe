#include "ovtkCBoxAlgorithmEBMLOutputWriterPartBase.inl"

using namespace OpenViBEToolkit;
using namespace OpenViBE;
using namespace std;

// ________________________________________________________________________________________________________________
//

boolean CBoxAlgorithmEBMLOutputWriterPartBase::registerBoxAlgorithmEBMLOutputWriterPart(IBoxAlgorithmEBMLOutputWriter& rBoxAlgorithmEBMLOutputWriter)
{
	m_vBoxAlgorithmEBMLOutputWriter.push_back(&rBoxAlgorithmEBMLOutputWriter);
	return true;
}

boolean CBoxAlgorithmEBMLOutputWriterPartBase::unregisterBoxAlgorithmEBMLOutputWriterPart(IBoxAlgorithmEBMLOutputWriter& rBoxAlgorithmEBMLOutputWriter)
{
	boolean l_bRemoved=false;
	vector<IBoxAlgorithmEBMLOutputWriter*>::iterator it;
	it=m_vBoxAlgorithmEBMLOutputWriter.begin();
	while(it!=m_vBoxAlgorithmEBMLOutputWriter.end())
	{
		if((*it)==&rBoxAlgorithmEBMLOutputWriter)
		{
			it=m_vBoxAlgorithmEBMLOutputWriter.erase(it);
			l_bRemoved=true;
		}
		else
		{
			it++;
		}
	}
	return l_bRemoved;
}

// ________________________________________________________________________________________________________________
//

boolean CBoxAlgorithmEBMLOutputWriterPartBase::writeHeader(EBML::IWriter& rWriter)
{
	vector<IBoxAlgorithmEBMLOutputWriter*>::iterator it;
	boolean l_bResult=true;
	l_bResult&=this->writeHeaderStart(rWriter);
	for(it=m_vBoxAlgorithmEBMLOutputWriter.begin(); it!=m_vBoxAlgorithmEBMLOutputWriter.end(); it++)
		l_bResult&=(*it)->writeHeader(rWriter);
	l_bResult&=this->writeHeaderStop(rWriter);
	return l_bResult;
}

boolean CBoxAlgorithmEBMLOutputWriterPartBase::writeBuffer(EBML::IWriter& rWriter)
{
	vector<IBoxAlgorithmEBMLOutputWriter*>::iterator it;
	boolean l_bResult=true;
	l_bResult&=this->writeBufferStart(rWriter);
	for(it=m_vBoxAlgorithmEBMLOutputWriter.begin(); it!=m_vBoxAlgorithmEBMLOutputWriter.end(); it++)
		l_bResult&=(*it)->writeBuffer(rWriter);
	l_bResult&=this->writeBufferStop(rWriter);
	return l_bResult;
}

boolean CBoxAlgorithmEBMLOutputWriterPartBase::writeEnd(EBML::IWriter& rWriter)
{
	vector<IBoxAlgorithmEBMLOutputWriter*>::iterator it;
	boolean l_bResult=true;
	l_bResult&=this->writeEndStart(rWriter);
	for(it=m_vBoxAlgorithmEBMLOutputWriter.begin(); it!=m_vBoxAlgorithmEBMLOutputWriter.end(); it++)
		l_bResult&=(*it)->writeEnd(rWriter);
	l_bResult&=this->writeEndStop(rWriter);
	return l_bResult;
}

// ________________________________________________________________________________________________________________
//

boolean CBoxAlgorithmEBMLOutputWriterPartBase::writeHeaderStart(EBML::IWriter& rWriter)
{
	return true;
}

boolean CBoxAlgorithmEBMLOutputWriterPartBase::writeHeaderStop(EBML::IWriter& rWriter)
{
	return true;
}

boolean CBoxAlgorithmEBMLOutputWriterPartBase::writeBufferStart(EBML::IWriter& rWriter)
{
	return true;
}

boolean CBoxAlgorithmEBMLOutputWriterPartBase::writeBufferStop(EBML::IWriter& rWriter)
{
	return true;
}

boolean CBoxAlgorithmEBMLOutputWriterPartBase::writeEndStart(EBML::IWriter& rWriter)
{
	return true;
}

boolean CBoxAlgorithmEBMLOutputWriterPartBase::writeEndStop(EBML::IWriter& rWriter)
{
	return true;
}

// ________________________________________________________________________________________________________________
//
