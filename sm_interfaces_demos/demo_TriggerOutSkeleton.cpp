// headers
#include <iostream>
#include "../sm_interfaces/TriggerOutSkeleton.h"
#include <thread>

// namespaces
using namespace ara::com::someip::sd;
using namespace ara::com::helper;
using namespace AsyncBsdSocketLib;
using namespace ara::com;
using namespace ara::com::skelton;


// global variables
bool executing = true;
Poller* poller;
const int cTimeoutMs = 100;


int main()
{   
   // Create Object from TriggerOutSkeleton 
   InstanceIdentifier id("4");
   TriggerOutSkelton mySkeleton(id);

 
   // Create thread using a lambda expression
   poller = new Poller();
   poller = mySkeleton.getPoller();
   std::thread t1([](){
      while(executing)
      {
        poller->TryPoll(cTimeoutMs);
      }
   });


   // initilize the service before offer it
   std::vector<uint8_t> currentValue = {47,48,49};
   mySkeleton.init(currentValue);


   // offer the service
   std::cout << "before offering the service\n";
   mySkeleton.offerService();
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