#include "TriggerOutProxy.h"

static const std::string c_AnyIpAddress{"0.0.0.0"};
static const std::string c_NicIpAddress{"127.0.0.1"};
static const std::string c_MulticastGroup{"239.0.0.1"};
static const uint16_t c_ServiceDiscoveryFindingPort{6666};
static const uint16_t c_ServiceId = 1;
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
                /******************** constructors ************************/
                    
                TriggerOutProxy::HandleType::HandleType(InstanceIdentifier id,int16_t p): identifier{id},
                                                                                            port_no{p}
                {}


                /************ setters and getters *************************/

                void TriggerOutProxy::HandleType::setId(InstanceIdentifier id)
                {   identifier=id;  }

                void TriggerOutProxy::HandleType::setPort(int16_t p)
                {   port_no=p;  }

                int16_t TriggerOutProxy::HandleType::getPort()const
                {   return port_no; }

                const InstanceIdentifier& TriggerOutProxy::HandleType::GetInstanceId()const
                {   return identifier;  }

                void TriggerOutProxy::HandleType::setRequester(EventServiceRequester *r)
                {   requester = r;  }

                EventServiceRequester* TriggerOutProxy::HandleType::getRequester()
                { return requester; }    

                /**/

                ServiceHandleContainer<TriggerOutProxy::HandleType> TriggerOutProxy::findSerivce(InstanceIdentifier id)
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

                TriggerOutProxy::TriggerOutProxy (HandleType &handle)
                {
                    requester = handle.getRequester();
                }


                /************************ fundemental funtions *******************/

                void TriggerOutProxy::subscribe(size_t maxSampleCount)
                {
                    requester->eventClient->Subscribe(maxSampleCount);
                }

                helper::SubscriptionState TriggerOutProxy::GetSubscriptionState() const
                {
                    return requester->eventClient->GetSubscriptionState();
                }
                
                std::future<bool> TriggerOutProxy::setter(std::vector<uint8_t> data)
                {
                    return requester->eventClient->setter(data);
                }

                std::future<bool> TriggerOutProxy::getter(std::vector<uint8_t> &data)
                {
                    return requester->eventClient->getter(data);
                }

                void TriggerOutProxy::requestGetting()
                {
                    return requester->eventClient->requestGetting();
                }

                void TriggerOutProxy::printSubscriptionState()
                {
                    requester->eventClient->printCurrentState();
                }

                /*
                bool TriggerOutProxy::isSubscribed(int duration)
                {
                    return requester->eventClient->isSubscribed(duration);
                }
                */


                /********************* deconstructor *******************/
                
                TriggerOutProxy::~TriggerOutProxy()
                {

                }
        }
    }

}