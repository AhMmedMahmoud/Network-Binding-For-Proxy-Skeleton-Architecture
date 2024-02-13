#ifndef POTI_MANAGEMENT_SkELETON_H
#define POTI_MANAGEMENT_SkELETON_H  

#include "../someip/service/event_Only/event_service_provider.h"
#include "../helper/instance_id.h"

namespace ara 
{
    namespace com
    {
        namespace skelton
        {
            class POTI_Management_Skeleton : public ara::com::someip::sd::EventServiceProvider 
            {                    
                public:
                    /****************** constructor ************************************/

                    POTI_Management_Skeleton( ara::com::InstanceIdentifier id);
                    
                    /*
                    init
                    offerSerivce
                    update
                    getState
                    */

                    /******** disable copy constructor and equal assigment operator *****/
      
                    POTI_Management_Skeleton(const POTI_Management_Skeleton &other) = delete;
                    POTI_Management_Skeleton & operator=(const POTI_Management_Skeleton &other) = delete;
                    
      
                    /********************* deconstructor *******************/

                    ~POTI_Management_Skeleton();
            };
        }
    }
}

#endif