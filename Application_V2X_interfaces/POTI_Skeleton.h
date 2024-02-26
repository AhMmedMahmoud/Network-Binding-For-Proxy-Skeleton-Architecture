#ifndef POTI_SKELETON_H
#define POTI_SKELETON_H

#include "../someip/service/event_Only/event_service_provider.h"
#include "../helper/instance_id.h"

namespace ara 
{
    namespace com
    {
        namespace skelton
        {
            class POTI_Skeleton : public ara::com::someip::sd::EventServiceProvider 
            {                    
                public:
                    /****************** constructor ************************************/

                    POTI_Skeleton( ara::com::InstanceIdentifier id);
                    
                    /*
                    init
                    offerSerivce
                    update
                    getState
                    */

                    /******** disable copy constructor and equal assigment operator *****/
      
                    POTI_Skeleton(const POTI_Skeleton &other) = delete;
                    POTI_Skeleton & operator=(const POTI_Skeleton &other) = delete;
                    
      
                    /********************* deconstructor *******************/

                    ~POTI_Skeleton();
            };
        }
    }
}

#endif