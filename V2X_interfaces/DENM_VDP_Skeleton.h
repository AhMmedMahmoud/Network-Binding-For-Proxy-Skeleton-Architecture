#ifndef DENM_VDP_SkELETON_H
#define DENM_VDP_SkELETON_H    

#include "../someip/service/event_Only/event_service_provider.h"
#include "../helper/instance_id.h"

namespace ara 
{
    namespace com
    {
        namespace skelton
        {
            class DENM_VDP_Skeleton : public ara::com::someip::sd::EventServiceProvider 
            {                    
                public:
                    /****************** constructor ************************************/

                    DENM_VDP_Skeleton( ara::com::InstanceIdentifier id);
                    
                    /*
                    init
                    offerSerivce
                    update
                    getState
                    */

                    /******** disable copy constructor and equal assigment operator *****/
      
                    DENM_VDP_Skeleton(const DENM_VDP_Skeleton &other) = delete;
                    DENM_VDP_Skeleton & operator=(const DENM_VDP_Skeleton &other) = delete;
                    
      
                    /********************* deconstructor *******************/

                    ~DENM_VDP_Skeleton();
            };
        }
    }
}

#endif