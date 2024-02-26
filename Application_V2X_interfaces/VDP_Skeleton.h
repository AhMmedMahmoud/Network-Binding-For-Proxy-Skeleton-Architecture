#ifndef VDP_SKELETON_H
#define VDP_SKELETON_H

#include "../someip/service/event_Only/event_service_provider.h"
#include "../helper/instance_id.h"

namespace ara 
{
    namespace com
    {
        namespace skelton
        {
            class VDP_Skeleton : public ara::com::someip::sd::EventServiceProvider 
            {                    
                public:
                    /****************** constructor ************************************/

                    VDP_Skeleton( ara::com::InstanceIdentifier id);
                    
                    /*
                    init
                    offerSerivce
                    update
                    getState
                    */

                    /******** disable copy constructor and equal assigment operator *****/
      
                    VDP_Skeleton(const VDP_Skeleton &other) = delete;
                    VDP_Skeleton & operator=(const VDP_Skeleton &other) = delete;
                    
      
                    /********************* deconstructor *******************/

                    ~VDP_Skeleton();
            };
        }
    }
}

#endif