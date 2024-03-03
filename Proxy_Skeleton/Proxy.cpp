#include "Proxy.h"

static const std::string c_AnyIpAddress{"0.0.0.0"};
static const std::string c_NicIpAddress{"127.0.0.1"};
static const std::string c_MulticastGroup{"239.0.0.1"};
static const uint16_t c_ServiceDiscoveryFindingPort{6666};
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
        namespace proxy
        {    
            /******************** constructors ************************/
        
            Proxy::HandleType::HandleType(InstanceIdentifier id,int16_t p): identifier{id},
                                                                    port_no{p}
            {}


            /************ setters and getters *************************/

            void Proxy::HandleType::setId(InstanceIdentifier id)
            {   identifier=id;  }

            void Proxy::HandleType::setPort(int16_t p)
            {   port_no=p;  }

            int16_t Proxy::HandleType::getPort()const
            {   return port_no; }

            const InstanceIdentifier& Proxy::HandleType::GetInstanceId()const
            {   return identifier;  }

            void Proxy::HandleType::setRequester(Requester *r)
            {   requester = r;  }

            Requester* Proxy::HandleType::getRequester()
            { return requester; }    

            ServiceHandleContainer<Proxy::HandleType> Proxy::findSerivce(InstanceIdentifier id)
            {
                Requester *requester;
                requester = new Requester(c_ServiceId,
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

            Proxy::Proxy (HandleType &handle)
            {
                requester = handle.getRequester();
            }


            /************************ fundemental funtions *******************/
            
#if(EXAMPLE == PUBSUB)
            void Proxy::subscribe(size_t maxSampleCount)
            {
                requester->eventClient->Subscribe(maxSampleCount);
            }
            
            helper::SubscriptionState Proxy::GetSubscriptionState() const
            {
                return requester->eventClient->GetSubscriptionState();
            }
            
            std::future<bool> Proxy::getter(std::vector<uint8_t> &data)
            {
                return requester->eventClient->getter(data);
            }

            void Proxy::printSubscriptionState()
            {
                requester->eventClient->printCurrentState();
            }

#elif(EXAMPLE == RPCS)
            std::future<bool> Proxy::calculateSum(const std::vector<uint8_t> &payload,
                            std::vector<uint8_t> &data)
            {
                return requester->calculateSum(payload,data);
            }
#endif


            /********************* deconstructor *******************/
            
            Proxy::~Proxy()
            {

            }
        }
    }

}