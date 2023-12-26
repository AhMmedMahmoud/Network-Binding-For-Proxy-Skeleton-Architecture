#ifndef SKELETON_H
#define SKELETON_H

#include "../someip/service/event_rpcs/provider.h"
#include "../helper/instance_id.h"

namespace ara 
{
    namespace com
    {
        namespace skelton
        {
            class Skelton : public ara::com::someip::sd::Provider 
            {                    
                public:
                    /***************** constructor ************************/
                    
                    Skelton( ara::com::InstanceIdentifier id);
                    

                    /*
                    init
                    offerSerivce
                    update
                    getState
                    */


                    /******** disable copy constructor and equal assigment operator *****/
      
                    Skelton(const Skelton &other) = delete;
                    Skelton & operator=(const Skelton &other) = delete;
                    
      
                    /********************* deconstructor *******************/

                    ~Skelton();
            };
            
        }
    }
}

#endif