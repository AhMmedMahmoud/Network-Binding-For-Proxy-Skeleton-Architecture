#ifndef POTI_VDP_SKELETON_H
#define POTI_VDP_SKELETON_H

#include "../someip/service/event_Only/event_service_provider.h"
#include "../helper/instance_id.h"

namespace ara 
{
    namespace com
    {
        namespace skelton
        {
            class POTI_VDP_Skeleton : public ara::com::someip::sd::EventServiceProvider 
            {                    
                public:
                    /****************** constructor ************************************/

                    POTI_VDP_Skeleton( ara::com::InstanceIdentifier id);
                    
                    /*
                    init
                    offerSerivce
                    update
                    getState
                    */

                    /******** disable copy constructor and equal assigment operator *****/
      
                    POTI_VDP_Skeleton(const POTI_VDP_Skeleton &other) = delete;
                    POTI_VDP_Skeleton & operator=(const POTI_VDP_Skeleton &other) = delete;
                    
      
                    /********************* deconstructor *******************/

                    ~POTI_VDP_Skeleton();
            };
        }
    }
}

#endif