#ifndef DENM_CONTROL_SKELETON_H
#define DENM_CONTROL_SKELETON_H    

#include "../someip/service/event_Only/event_service_provider.h"
#include "../helper/instance_id.h"

namespace ara 
{
    namespace com
    {
        namespace skelton
        {
            class DENM_Control_Skeleton : public ara::com::someip::sd::EventServiceProvider 
            {                    
                public:
                    /****************** constructor ************************************/

                    DENM_Control_Skeleton( ara::com::InstanceIdentifier id);
                    
                    /*
                    init
                    offerSerivce
                    update
                    getState
                    */

                    /******** disable copy constructor and equal assigment operator *****/
      
                    DENM_Control_Skeleton(const DENM_Control_Skeleton &other) = delete;
                    DENM_Control_Skeleton & operator=(const DENM_Control_Skeleton &other) = delete;
                    
      
                    /********************* deconstructor *******************/

                    ~DENM_Control_Skeleton();
            };
        }
    }
}

#endif