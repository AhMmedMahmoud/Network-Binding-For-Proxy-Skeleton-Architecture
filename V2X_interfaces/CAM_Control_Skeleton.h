#ifndef CAM_CONTROL_SKELETON_H
#define CAM_CONTROL_SKELETON_H    

#include "../someip/service/event_Only/event_service_provider.h"
#include "../helper/instance_id.h"

namespace ara 
{
    namespace com
    {
        namespace skelton
        {
            class CAM_Control_Skeleton : public ara::com::someip::sd::EventServiceProvider 
            {                    
                public:
                    /****************** constructor ************************************/

                    CAM_Control_Skeleton( ara::com::InstanceIdentifier id);
                    
                    /*
                    init
                    offerSerivce
                    update
                    getState
                    */

                    /******** disable copy constructor and equal assigment operator *****/
      
                    CAM_Control_Skeleton(const CAM_Control_Skeleton &other) = delete;
                    CAM_Control_Skeleton & operator=(const CAM_Control_Skeleton &other) = delete;
                    
      
                    /********************* deconstructor *******************/

                    ~CAM_Control_Skeleton();
            };
        }
    }
}

#endif