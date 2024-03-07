#ifndef EVENT_DENM_SKELETON_H
#define EVENT_DENM_SKELETON_H    

#include "../someip/service/event_Only/event_service_provider.h"
#include "../helper/instance_id.h"

namespace ara 
{
    namespace com
    {
        namespace skelton
        {
            class Event_DENM_Skeleton : public ara::com::someip::sd::EventServiceProvider 
            {                    
                public:
                    /****************** constructor ************************************/

                    Event_DENM_Skeleton( ara::com::InstanceIdentifier id);
                    
                    /*
                    init
                    offerSerivce
                    update
                    getState
                    */

                    /******** disable copy constructor and equal assigment operator *****/
      
                    Event_DENM_Skeleton(const Event_DENM_Skeleton &other) = delete;
                    Event_DENM_Skeleton & operator=(const Event_DENM_Skeleton &other) = delete;
                    
      
                    /********************* deconstructor *******************/

                    ~Event_DENM_Skeleton();
            };
        }
    }
}

#endif