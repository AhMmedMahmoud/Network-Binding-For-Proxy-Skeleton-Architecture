#ifndef CAM_VDP_SKELETON_H
#define CAM_VDP_SKELETON_H

#include "../someip/service/event_Only/event_service_provider.h"
#include "../helper/instance_id.h"

namespace ara 
{
    namespace com
    {
        namespace skelton
        {
            class CAM_VDP_Skeleton : public ara::com::someip::sd::EventServiceProvider 
            {                    
                public:
                    /****************** constructor ************************************/

                    CAM_VDP_Skeleton( ara::com::InstanceIdentifier id);
                    
                    /*
                    init
                    offerSerivce
                    update
                    getState
                    */

                    /******** disable copy constructor and equal assigment operator *****/
      
                    CAM_VDP_Skeleton(const CAM_VDP_Skeleton &other) = delete;
                    CAM_VDP_Skeleton & operator=(const CAM_VDP_Skeleton &other) = delete;
                    
      
                    /********************* deconstructor *******************/

                    ~CAM_VDP_Skeleton();
            };
        }
    }
}

#endif