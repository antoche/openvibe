#ifndef __OpenViBEApplication_CAbstractVrpnPeripheral_H__
#define __OpenViBEApplication_CAbstractVrpnPeripheral_H__

class CDeviceInfo;

#include <list>
#include <string>
#include <map>

namespace OpenViBEVRDemos 
{
	/**
	 * \class CAbstractVrpnPeripheral
	 * \author Laurent Bonnet (INRIA/IRISA)
	 * \date 2010-02-16
	 * \brief A VRPN peripheral abstraction, client side.
	 *
	 * \details The CAbstractVrpnPeripheral handles the connection to at most one Analog Server and one Button Server.
	 * 
	 */
	class CAbstractVrpnPeripheral
	{
	public:

		std::list < std::pair < int, int > > m_vButton; //!< list (chronological) of pairs (button_id, button_state).
		std::list < std::list < double > > m_vAnalog;   //!< list (chronological) of list (channels) of values.
		double m_dAnalogScale;                          //!< Scalar applied to any value read on the Analog server.
		double m_dAnalogOffset;                         //!< Offset applied to any value read on the Analog server.

		std::string m_sDeviceAddress;                   //!< The device address ([peripheral-name]@[hostname]).
		CDeviceInfo* m_pDevice;                         //!< The VRPN Device

		/**
		* \brief Default constructor.
		*/
		CAbstractVrpnPeripheral(void);
		/**
		* \brief Constructor.
		* \param deviceAddress The device address.
		*/
		CAbstractVrpnPeripheral(const std::string deviceAddress);
		/**
		* \brief Destructor.
		*/
		virtual ~CAbstractVrpnPeripheral(void);

		/**
		* \brief Initialize the VRPN Remote object and handlers.
		*/
		virtual void init(void);
		
		/**
		* \brief Main loop.
		*/
		virtual void loop(void);
			
	};
};
#endif
