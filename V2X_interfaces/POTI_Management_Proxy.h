#ifndef POTI_MANAGEMENT_PROXY_H     
#define POTI_MANAGEMENT_PROXY_H

#include "../someip/service/event_Only/event_service_requester.h"
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

            class POTI_Management_Proxy
            {
                public:
                    using Handler = std::function<void(const std::vector<uint8_t> &)>;

                    class HandleType
                    {
                        private:  
                            /********************** attributes ************************/
                         
                            ara::com::InstanceIdentifier identifier;
                            int16_t port_no;
                            EventServiceRequester *requester;

                        
                        public:
                            /******************** constructors ************************/
                        
                            HandleType(InstanceIdentifier id,int16_t p);


                            /************ setters and getters *************************/

                            void setId(InstanceIdentifier id);

                            void setPort(int16_t p);

                            int16_t getPort()const;

                            const InstanceIdentifier &GetInstanceId()const;

                            void setRequester(EventServiceRequester *r);

                            EventServiceRequester* getRequester();  
                    };


                    /*********************** static functions **********************/

                    static ServiceHandleContainer<HandleType> findSerivce(InstanceIdentifier id);


                    /************************* constructor **************************/

                    POTI_Management_Proxy (HandleType &handle);
   

                    /************************ fundemental funtions *******************/

                    void subscribe(size_t maxSampleCount);
 
                    helper::SubscriptionState GetSubscriptionState() const;

                    std::future<bool> getter(std::vector<uint8_t> &data);

                    void printSubscriptionState();
                    
                    void SetReceiveHandler(Handler h);

                    void UnsetReceiveHandler();


                    /******** disable copy constructor and equal assigment operator *****/
                    
                    POTI_Management_Proxy(const POTI_Management_Proxy &other) = delete;
                    POTI_Management_Proxy& operator=(const POTI_Management_Proxy &other) = delete;


                    /********************* deconstructor *******************/
                    
                    ~POTI_Management_Proxy();

                private:
                    /*************************** atttibutes ****************/

                    EventServiceRequester *requester;
            };
        }
    }
}

#endif