// headers
#include <iostream>
#include "../V2X_interfaces/DENM_VDP_Skeleton.h"
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
   // Create Object from CAM_VDP_Skeleton 
   InstanceIdentifier id("8");
   DENM_VDP_Skeleton mySkeleton(id);


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
   mySkeleton.init();


   // offer the service
   std::cout << "before offering the service\n";
   mySkeleton.offerService();
   std::cout << "after offering the service\n";


   int counter = false;
   std::vector<uint8_t> data;
   while(1)
   {    
      if(mySkeleton.eventServer->GetState() == PubSubState::Subscribed && counter == false)
      {
         // Introduce a delay of 7 seconds
         std::this_thread::sleep_for(std::chrono::seconds(7));
         std::cout << "---preparing sample1 to send---\n";
         for(int i =0; i< 2400; i++)
         {
            data.push_back(i);
         }
         mySkeleton.eventServer->update(data);

         /*
         // Introduce a delay of 7 seconds
         std::this_thread::sleep_for(std::chrono::seconds(7));
         std::cout << "\n---preparing sample2 to send---\n";
         data= {4,5,6,7};
         mySkeleton.eventServer->update(data);

         // Introduce a delay of 7 seconds
         std::this_thread::sleep_for(std::chrono::seconds(7));
         std::cout << "\n---preparing sample2 to send---\n";
         data= {8,9,10,11};
         mySkeleton.eventServer->update(data);
         */

         counter = true;
      } 
   }


   // Join the thread with the main thread
   t1.join();
   return 0;
}