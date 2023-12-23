#include "../someip/service/event_Only/event_service_requester.h"
#include "../helper/instance_id.h"

using namespace ara::com::someip::sd;
using namespace ara::com::helper;
using namespace AsyncBsdSocketLib;

const std::string c_AnyIpAddress{"0.0.0.0"};
const std::string c_NicIpAddress{"127.0.0.1"};
const std::string c_MulticastGroup{"239.0.0.1"};
const uint16_t c_ServiceDiscoveryFindingPort{6666};
const uint16_t c_ServiceId = 11;
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
        namespace proxy
        {    
            template <typename t>
            using ServiceHandleContainer = std::vector<t>;

            class TriggerOutProxy
            {
                public:
                    class HandleType
                    {
                        private:  
                            /********************** attributes ************************/
                         
                            ara::com::InstanceIdentifier identifier;
                            int16_t port_no;
                            EventServiceRequester *requester;

                        
                        public:
                            /******************** constructors ************************/
                        
                            HandleType(InstanceIdentifier id,int16_t p): identifier{id},
                                                                                   port_no{p}
                            {}


                            /************ setters and getters *************************/

                            void setId(InstanceIdentifier id)
                            {   identifier=id;  }

                            void setPort(int16_t p)
                            {   port_no=p;  }

                            int16_t getPort()const
                            {   return port_no; }

                            const InstanceIdentifier &GetInstanceId()const
                            {   return identifier;  }

                            void setRequester(EventServiceRequester *r)
                            {   requester = r;  }

                            EventServiceRequester* getRequester()
                            { return requester; }    
                    };

                    static ServiceHandleContainer<HandleType> findSerivce(InstanceIdentifier id)
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

                    TriggerOutProxy (HandleType &handle)
                    {
                        requester = handle.getRequester();
                    }
   

                    /************************ fundemental funtions *******************/

                    void subscribe()
                    {
                        requester->eventClient->Subscribe();
                    }

                    bool isSubscribed(int duration)
                    {
                        return requester->eventClient->isSubscribed(duration);
                    }
                    
                    std::future<bool> setter(std::vector<uint8_t> data)
                    {
                        return requester->eventClient->setter(data);
                    }

                    std::future<bool> getter(std::vector<uint8_t> &data)
                    {
                        return requester->eventClient->getter(data);
                    }

                    void requestGetting()
                    {
                       return requester->eventClient->requestGetting();
                    }


                    /******** disable copy constructor and equal assigment operator *****/
                    
                    TriggerOutProxy(const TriggerOutProxy &other) = delete;
                    TriggerOutProxy& operator=(const TriggerOutProxy &other) = delete;


                    /********************* deconstructor *******************/
                    
                    ~TriggerOutProxy()
                    {

                    }

                private:
                    /*************************** atttibutes ****************/

                    HandleType  *Handle;
                    EventServiceRequester *requester;
            };
        }
    }

}