#include "VDP_Proxy.h"

static const std::string c_AnyIpAddress{"0.0.0.0"};
static const std::string c_NicIpAddress{"127.0.0.1"};
static const std::string c_MulticastGroup{"239.0.0.1"};
static const uint16_t c_ServiceDiscoveryFindingPort{6666};
static const uint16_t c_ServiceId = 2;
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
        namespace proxy
        {    
                VDP_Proxy::HandleType::HandleType(InstanceIdentifier id,int16_t p): identifier{id},
                                                                                            port_no{p}
                {}

                void VDP_Proxy::HandleType::setId(InstanceIdentifier id)
                {   identifier=id;  }

                void VDP_Proxy::HandleType::setPort(int16_t p)
                {   port_no=p;  }

                int16_t VDP_Proxy::HandleType::getPort()const
                {   return port_no; }

                const InstanceIdentifier& VDP_Proxy::HandleType::GetInstanceId()const
                {   return identifier;  }

                void VDP_Proxy::HandleType::setRequester(EventServiceRequester *r)
                {   requester = r;  }

                EventServiceRequester* VDP_Proxy::HandleType::getRequester()
                { return requester; }  


                ServiceHandleContainer<VDP_Proxy::HandleType> VDP_Proxy::findSerivce(InstanceIdentifier id)
                {
                    EventServiceRequester *requester;
                    requester = new EventServiceRequester(c_ServiceId,
                                            id.getInstanceId(),
                                            c_MajorVersion,
                                            c_MinorVersion,
                                            c_EventgroupId,
                                            new AsyncBsdSocketLib::Poller(),
                                            c_NicIpAddress,
                                            c_MulticastGroup,
                                            c_ServiceDiscoveryFindingPort,
                                            c_ProtocolVersion);
                    
                    Poller *poller = requester->getPoller();
                    bool *running = new bool(true);
                    std::thread t1([poller,running](){
                        while(*running)
                        {
                            poller->TryPoll(cTimeoutMs);
                        }
                    });

                    uint16_t _port;
                    bool _result = requester->findService(_port);
                    if(!_result)
                    {
                        std::vector<HandleType> handles;
                        
                        *running = false;
                        //std::cout << "11111111111111\n";
                        t1.join();
                        delete running;
                        //std::cout << "11111111111111\n";
                        return handles;
                    }
                    else
                    {
                        std::vector<HandleType> handles;
                        HandleType handleTypeObj(id,_port);
                        handleTypeObj.setRequester(requester);
                        handles.push_back(handleTypeObj);
                        
                        *running = false;
                        //std::cout << "2222222222222222222\n";
                        t1.join();
                        delete running;
                        //std::cout << "2222222222222222222\n";
                        return handles;
                    }
                }

                /************************* constructor **************************/

                VDP_Proxy::VDP_Proxy (HandleType &handle)
                {
                    requester = handle.getRequester();
                }


                /************************ fundemental funtions *******************/

                void VDP_Proxy::subscribe(size_t maxSampleCount)
                {
                    requester->eventClient->Subscribe(maxSampleCount);
                }

                helper::SubscriptionState VDP_Proxy::GetSubscriptionState() const
                {
                    return requester->eventClient->GetSubscriptionState();
                }

                std::future<bool> VDP_Proxy::setter(std::vector<uint8_t> data)
                {
                    return requester->eventClient->setter(data);
                }

                std::future<bool> VDP_Proxy::getter(std::vector<uint8_t> &data)
                {
                    return requester->eventClient->getter(data);
                }

                void VDP_Proxy::requestGetting()
                {
                    return requester->eventClient->requestGetting();
                }

                void VDP_Proxy::printSubscriptionState()
                {
                    requester->eventClient->printCurrentState();
                }

                /*
                bool VDP_Proxy::isSubscribed(int duration)
                {
                    return requester->eventClient->isSubscribed(duration);
                }
                */

                void VDP_Proxy::SetReceiveHandler(Handler h)
                {
                    requester->eventClient->SetReceiveHandler(h);
                }

                void VDP_Proxy::UnsetReceiveHandler()
                {
                    requester->eventClient->UnsetReceiveHandler();
                }


                /********************* deconstructor *******************/
                
                VDP_Proxy::~VDP_Proxy()
                {

                }
        }
    }

}