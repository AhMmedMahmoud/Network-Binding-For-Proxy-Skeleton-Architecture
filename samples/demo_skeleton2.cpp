#include <iostream>
#include "../Proxy_Skeleton/Skeleton.h"
#include <thread>

using namespace ara::com::someip::sd;
using namespace ara::com::helper;
using namespace AsyncBsdSocketLib;
using namespace ara::com;
using namespace ara::com::skelton;

bool executing = true;
Poller* poller;


int main()
{
    poller = new Poller();
    
    InstanceIdentifier id("3");
    Skelton mySkeleton(id);

    poller = mySkeleton.getPoller();


   // Create thread using a lambda expression
   std::thread t1([](){
      while(executing)
      {
        poller->TryPoll(cTimeoutMs);
      }
   });

   mySkeleton.init();

   std::cout << "before offering the service\n";

   mySkeleton.offerService();
   
   std::cout << "after offering the service\n";

   std::this_thread::sleep_for(std::chrono::seconds(14));

   // std::cout << "before stop offer service\n";

   // mySkeleton.stopService();

   // std::cout << "after stop offer service\n";

#if(EXAMPLE == PUBSUB)
   int counter = false;
   std::vector<uint8_t> data;

   /*
   while(1)
   {    
      if(mySkeleton.eventServer->GetState() == PubSubState::Subscribed && counter == false)
      {
         // Introduce a delay of 7 seconds
         std::this_thread::sleep_for(std::chrono::seconds(7));
         std::cout << "---preparing sample1 to send---\n";
         data= {1,2,3,4};
         mySkeleton.eventServer->update(data);

         // Introduce a delay of 7 seconds
         std::this_thread::sleep_for(std::chrono::seconds(20));
         counter = true;
      } 
   }
   */
#endif
   


   // Join the thread with the main thread
   t1.join();
   return 0;
}