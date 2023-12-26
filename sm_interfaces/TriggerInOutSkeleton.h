#ifndef TRIGGER_IN_OUT_SKELETON_H
#define TRIGGER_IN_OUT_SKELETON_H

#include "../someip/service/event_Only/event_service_provider.h"
#include "../helper/instance_id.h"

namespace ara 
{
    namespace com
    {
        namespace skelton
        {
            class TriggerInOutSkelton : public ara::com::someip::sd::EventServiceProvider 
            {                    
                public:
                    /****************** constructor ************************************/

                    TriggerInOutSkelton( ara::com::InstanceIdentifier id);
                    
                    /*
                    init
                    offerSerivce
                    update
                    getState
                    */

                    /******** disable copy constructor and equal assigment operator *****/
      
                    TriggerInOutSkelton(const TriggerInOutSkelton &other) = delete;
                    TriggerInOutSkelton & operator=(const TriggerInOutSkelton &other) = delete;
                    
      
                    /********************* deconstructor *******************/

                    ~TriggerInOutSkelton();
            };
        }
    }
}

#endif