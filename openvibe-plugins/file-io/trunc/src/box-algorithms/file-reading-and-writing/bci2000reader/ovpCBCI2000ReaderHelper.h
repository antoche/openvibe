#ifndef BCI2000READER_H
#define BCI2000READER_H


#include <string>
#include <fstream>
#include <istream>
#include <ostream>
#include <vector>
#include <map>
#include <boost/regex.hpp>

#include "ovpCBitfield.h"

namespace BCI2000
{
	/**
	* \class CBCI2000ReaderHelper
	* \author Olivier Rochel (INRIA)
	* \brief BCI2000 file format parser and utilities. Uses the m_oBitfield utility class.
	**/
	class CBCI2000ReaderHelper
	{
	protected:
		std::ifstream m_oBCIFile;

		OpenViBE::float32 m_f32BCI2000version; // file version.
		OpenViBE::int32 m_i32HeaderLength;       // header size (inc. meta)
		OpenViBE::int32 m_i32SourceChannelCount;        // number of channels
		OpenViBE::int32 m_i32StateVectorLength; // size of state field
		OpenViBE::CString m_sDataFormat; // data format (float32, int16...)
		
		std::vector<OpenViBE::CString> m_vChannelNames;

		OpenViBE::int32 m_i32NbSamples;
		OpenViBE::int32 m_i32SampleSize;
		OpenViBE::int32 m_i32SamplesLeft;

		OpenViBE::boolean m_bGood; 	// m_bGood is true if file open, header looks m_bGood (may
		// still be truncated or broken in a silly way)

		std::map<OpenViBE::CString,OpenViBE::CString> m_mParameters;
		// state vector
		BCI2000::CBitfield m_oBitfield;
		// helpers
		OpenViBE::boolean parseMeta(OpenViBE::CString &rMeta);
		OpenViBE::boolean parseHeader(std::istream &is);

	private:
		template <class TFrom, class TTo>
		OpenViBE::int32 readSamplesInternal(TTo* pSamples, OpenViBE::uint32 * pStates, OpenViBE::int32 i32Nb);

	public:
		/**
		* Constructor from a BCI2000 file.
		* \param filename BCI2000 file name.
		**/
		CBCI2000ReaderHelper(const char * filename);
		~CBCI2000ReaderHelper();

		void printInfo(std::ostream &os);
		OpenViBE::float32 getRate(void) const;
		OpenViBE::CString getChannelName(OpenViBE::uint32 ui32Index) const;
		
		std::vector<OpenViBE::float32> readSample();
		OpenViBE::int32 readSamples(OpenViBE::float64* pSamples, OpenViBE::uint32 * pStates,OpenViBE::int32 i32Nb);

		// getters
		OpenViBE::int32 getNbSamples() const
		{
			return m_i32NbSamples;
		}
		OpenViBE::int32 getSampleSize() const
		{
			return m_i32SampleSize;
		}
		OpenViBE::int32 getChannels() const
		{
			return m_i32SourceChannelCount;
		}

		OpenViBE::int32 getSamplesLeft() const
		{
			return m_i32SamplesLeft;
		}
		OpenViBE::boolean isGood() const
		{
			return m_bGood;
		}
		OpenViBE::int32 getStateVectorSize() const
		{
			return m_oBitfield.size();
		}
		const OpenViBE::CString & getStateName(OpenViBE::int32 i) const
		{
			return m_oBitfield.getFieldName(i);
		}
	};
}

#endif
