// headers
#include <iostream>
#include "../V2X_interfaces/CAM_VDP_Skeleton.h"
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


// class that simulates SM class
class SM
{
   public:
   bool process(std::vector<uint8_t> data)
   {
      std::cout << "SM process function\n";
      for(uint8_t x : data) 
      {
         if(x != 44)
            return false;
      }
      return true;
   }
};


int main()
{   
   // Create Object from CAM_VDP_Skeleton 
   InstanceIdentifier id("7");
   CAM_VDP_Skeleton mySkeleton(id);


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
   SM smObject;
   std::vector<uint8_t> currentValue = {47,48,49};
   auto handle = std::bind(&SM::process, &smObject, std::placeholders::_1);
   mySkeleton.init(currentValue, handle);


   // offer the service
   std::cout << "before offering the service\n";
   mySkeleton.offerService();
   std::cout << "after offering the service\n";


   // Join the thread with the main thread
   t1.join();
   return 0;
}