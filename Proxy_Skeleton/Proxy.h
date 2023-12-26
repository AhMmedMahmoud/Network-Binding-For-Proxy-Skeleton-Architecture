#ifndef PROXY_H
#define PROXY_H

#include "../someip/service/event_rpcs/requester.h"
#include "../helper/instance_id.h"

using namespace ara::com::someip::sd;
using namespace ara::com::helper;
using namespace AsyncBsdSocketLib;

namespace ara 
{
    namespace com
    {
        namespace proxy
        {    
            template <typename t>
            using ServiceHandleContainer = std::vector<t>;

            class Proxy
            {
                public:
                    class HandleType
                    {
                        private:  
                            /********************** attributes ************************/
                         
                            ara::com::InstanceIdentifier identifier;
                            int16_t port_no;
                            Requester *requester;

                        
                        public:
                            /******************** constructor ************************/
                        
                            HandleType(InstanceIdentifier id,int16_t p);


                            /************ setters and getters *************************/

                            void setId(InstanceIdentifier id);

                            void setPort(int16_t p);

                            int16_t getPort()const;

                            const InstanceIdentifier &GetInstanceId()const;

                            void setRequester(Requester *r);

                            Requester* getRequester();  
                    };


                    /*********************** static functions **********************/

                    static ServiceHandleContainer<HandleType> findSerivce(InstanceIdentifier id);


                    /************************* constructor **************************/

                    Proxy (HandleType &handle);
   

                    /************************ fundemental funtions *******************/

#if(EXAMPLE == PUBSUB)
                    void subscribe(size_t maxSampleCount);
                    
                    helper::SubscriptionState GetSubscriptionState() const;
                    
                    std::future<bool> setter(std::vector<uint8_t> data);

                    std::future<bool> getter(std::vector<uint8_t> &data);

                    void requestGetting();

                    void printSubscriptionState();

                    // bool isSubscribed(int duration);
                    
#elif(EXAMPLE == RPCS)

                    std::future<bool> calculateSum(const std::vector<uint8_t> &payload,
                                   std::vector<uint8_t> &data);
#endif


                    /******** disable copy constructor and equal assigment operator *****/
                    
                    Proxy(const Proxy &other) = delete;
                    Proxy& operator=(const Proxy &other) = delete;


                    /********************* deconstructor *******************/
                    
                    ~Proxy();


                private:
                    /*************************** atttibutes ****************/

                    HandleType  *Handle;
                    Requester *requester;
            };
        }
    }

}

#endif