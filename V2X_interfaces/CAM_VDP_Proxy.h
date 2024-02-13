#ifndef CAM_VDP_PROXY_H     
#define CAM_VDP_PROXY_H   

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

            class CAM_VDP_Proxy
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

                    CAM_VDP_Proxy (HandleType &handle);
   

                    /************************ fundemental funtions *******************/

                    void subscribe(size_t maxSampleCount);
 
                    helper::SubscriptionState GetSubscriptionState() const;

                    std::future<bool> setter(std::vector<uint8_t> data);

                    std::future<bool> getter(std::vector<uint8_t> &data);

                    void requestGetting();

                    void printSubscriptionState();
                    
                    //bool isSubscribed(int duration);


                    /******** disable copy constructor and equal assigment operator *****/
                    
                    CAM_VDP_Proxy(const CAM_VDP_Proxy &other) = delete;
                    CAM_VDP_Proxy& operator=(const CAM_VDP_Proxy &other) = delete;


                    /********************* deconstructor *******************/
                    
                    ~CAM_VDP_Proxy();

                private:
                    /*************************** atttibutes ****************/

                    EventServiceRequester *requester;
            };
        }
    }
}

#endif