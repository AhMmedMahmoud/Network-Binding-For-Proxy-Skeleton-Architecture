// headers
#include <iostream>
#include "../V2X_interfaces/DENM_VDP_Skeleton.h"  
#include "../V2X_interfaces/DENM_Control_Skeleton.h"  

#include <thread>

// namespaces
using namespace ara::com::someip::sd;
using namespace ara::com::helper;
using namespace AsyncBsdSocketLib;
using namespace ara::com;
using namespace ara::com::skelton;


// global variables
bool executing = true;
Poller* poller_DENM_VDP;
Poller* poller_DENM_Control;
const int cTimeoutMs = 100;


int main()
{   
   // Create Object from DENM_VDP_Skeleton 
   InstanceIdentifier id_DENM_VDP("8");
   DENM_VDP_Skeleton mySkeleton_DENM_VDP(id_DENM_VDP);

   // Create Object from DENM_Control_Skeleton 
   InstanceIdentifier id_DENM_Control("9");
   DENM_Control_Skeleton mySkeleton_DENM_Control(id_DENM_Control);

 
   // Create thread using a lambda expression
   poller_DENM_VDP = mySkeleton_DENM_VDP.getPoller();
   poller_DENM_Control = mySkeleton_DENM_Control.getPoller();

   std::thread t1([](){
      while(executing)
      {
        poller_DENM_VDP->TryPoll(cTimeoutMs);
        poller_DENM_Control->TryPoll(cTimeoutMs);
      }
   });


   // initilize the service before offer it
   std::vector<uint8_t> currentValue_DENM_VDP = {47,48,49};
   std::vector<uint8_t> currentValue_DENM_Control = {33,34,35};
   mySkeleton_DENM_VDP.init(currentValue_DENM_VDP);
   mySkeleton_DENM_Control.init(currentValue_DENM_Control);



   // offer the service
   std::cout << "before offering the service\n";
   mySkeleton_DENM_VDP.offerService();
   mySkeleton_DENM_Control.offerService();
   std::cout << "after offering the service\n";

   /*
   int counter = false;
   std::vector<uint8_t> data;
   while(1)
   {    
      if(mySkeleton.eventServer->GetState() == PubSubState::Subscribed && counter == false)
      {
         // Introduce a delay of 7 seconds
         std::this_thread::sleep_for(std::chrono::seconds(7));
         std::cout << "---preparing sample to send---\n";
         data= {10,20,30,40};
         mySkeleton.eventServer->update(data);

         // Introduce a delay of 7 seconds
         std::this_thread::sleep_for(std::chrono::seconds(20));
         counter = true;
      } 
   }
   */
   

   // Join the thread with the main thread
   t1.join();
   return 0;
}