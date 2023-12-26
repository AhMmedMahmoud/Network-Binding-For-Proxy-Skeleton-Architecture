#include "Skeleton.h"


static const std::string c_AnyIpAddress{"0.0.0.0"};
static const std::string c_NicIpAddress{"127.0.0.1"};
static const std::string c_MulticastGroup{"239.0.0.1"};
static const uint16_t c_ServiceDiscoveryOfferingPort{5555};
static const uint16_t c_Tcp_port = 7000;
static const uint16_t c_Udp_port = 8000;   // let difference between tcp and udp 1000
static const uint16_t c_ServiceId = 10;
static const uint8_t c_MajorVersion = 10;
static const uint8_t c_MinorVersion = 4;
static const uint16_t c_EventgroupId = 5;
static const uint8_t c_ProtocolVersion = 20;
static const uint16_t c_InterfaceVersion = 2;
static const uint16_t c_ClientId = 1;
static const int cTimeoutMs = 100;


namespace ara 
{
    namespace com
    {
        namespace skelton
        {
            /***************** constructor ************************/

            Skelton::Skelton( ara::com::InstanceIdentifier id) : Provider
                                                                ( 
                                                                    c_ServiceId,        
                                                                    id.getInstanceId(),
                                                                    c_MajorVersion,                   
                                                                    c_MinorVersion,                       
                                                                    c_EventgroupId,                          
                                                                    new AsyncBsdSocketLib::Poller(),
                                                                    c_NicIpAddress,
                                                                    c_MulticastGroup,
                                                                    c_ServiceDiscoveryOfferingPort,
                                                                    c_Tcp_port,              
                                                                    c_Udp_port,        
                                                                    c_ProtocolVersion
                                                                )  
            {}  


            /********************* deconstructor *******************/

            Skelton::~Skelton()
            {

            }
        }
    }
}
