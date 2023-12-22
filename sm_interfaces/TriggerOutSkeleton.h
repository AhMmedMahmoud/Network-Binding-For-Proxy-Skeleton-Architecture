#include "../someip/service/event_Only/event_service_provider.h"
#include "../helper/instance_id.h"


const std::string c_AnyIpAddress{"0.0.0.0"};
const std::string c_NicIpAddress{"127.0.0.1"};
const std::string c_MulticastGroup{"239.0.0.1"};
const uint16_t c_ServiceDiscoveryOfferingPort{5555};
const uint16_t c_Udp_port = 9000;
const uint16_t c_ServiceId = 10;
const uint8_t c_MajorVersion = 10;
const uint8_t c_MinorVersion = 4;
const uint16_t c_EventgroupId = 5;
const uint8_t c_ProtocolVersion = 20;
const uint16_t c_InterfaceVersion = 2;
const uint16_t c_ClientId = 1;

const int cTimeoutMs = 100;

namespace ara 
{
    namespace com
    {
        namespace skelton
        {
            class TriggerOutSkelton : public ara::com::someip::sd::EventServiceProvider 
            {                    
                public:
                    TriggerOutSkelton( ara::com::InstanceIdentifier id) : EventServiceProvider
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
                                                                            c_Udp_port,        
                                                                            c_ProtocolVersion
                                                                          )  
                    {}  
                    
                    /*
                    init
                    offerSerivce
                    update
                    getState
                    */


                    /******** disable copy constructor and equal assigment operator *****/
      
                    TriggerOutSkelton(const TriggerOutSkelton &other) = delete;
                    TriggerOutSkelton & operator=(const TriggerOutSkelton &other) = delete;
                    
      
                    /********************* deconstructor *******************/

                    ~TriggerOutSkelton()
                    {

                    }
            };
        }
    }
}
