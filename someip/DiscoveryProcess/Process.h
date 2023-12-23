#ifndef SERVICE_DISCOVERY_H
#define SERVICE_DISCOVERY_H

#include "../../sockets/include/poller.h"
#include "../../sockets/include/udp_client.h"

#include "../../helper/concurrent_queue.h"
#include "../../helper/ipv4_address.h"

#include "../someipSdMsg/entry/service_entry.h"
#include "../someipSdMsg/entry/eventgroup_entry.h"
#include "../someipSdMsg/option/ipv4_endpoint_option.h"
#include "../someipSdMsg/someip_sd_message.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <condition_variable>


namespace ara
{
    namespace com
    {
        namespace someip
        {
            /// @brief SOME/IP service discovery namespace
            /// @note The namespace is not part of the ARA standard.
            namespace sd
            {
                enum class protocol
                {
                   tcp,
                   udp
                };
                
                struct registryKey
                {
                    uint16_t serviceId;
                    uint16_t instanceId;

                    bool operator<(const registryKey& other) const {
                        if (serviceId != other.serviceId) {
                            return serviceId < other.serviceId;
                        } else {
                            return instanceId < other.instanceId;
                        }
                    }
                };

                struct requestData
                {
                    uint16_t serviceId;
                    uint16_t instanceId;
                    //uint8_t majorVersion;
                    //uint32_t minorVersion;

                    bool operator<(const requestData& other) const {
                        if (serviceId != other.serviceId) {
                            return serviceId < other.serviceId;
                        } else if (instanceId != other.instanceId) {
                            return instanceId < other.instanceId;
                        } /* else if (majorVersion != other.majorVersion) {
                            return majorVersion < other.majorVersion;
                        } else {
                            return minorVersion < other.minorVersion;
                        }*/
                    }
                };

                struct transportInfo 
                {
                    std::string ipAddress;
                    uint16_t port;
                    protocol proto;
                };

                class ServiceDiscoveryProcess
                {
                private:
                    std::map<registryKey, transportInfo> serviceRegistry;

                    //std::vector<requestData> mFindingRequestsToNotOfferedServices;

                    void storeInfoOfServiceInstance(registryKey k, transportInfo info)
                    {
                        serviceRegistry[k] = info;
                    }

                    bool eraseInfoOfServiceInstance(registryKey k)
                    {
                        auto it = serviceRegistry.find(k);
                        if (it != serviceRegistry.end()) 
                        {
                            // The element with the specified key exists
                            // Remove the element from the map
                            serviceRegistry.erase(it);
                            return true;
                        } 
                        else
                        {
                            // The element with the specified key does not exist
                            // Handle the case where the element does not exist
                            std::cout << "info not exist\n";
                            return false;
                        }
                    }


                    /************************* poller attributes ******************************/

                    AsyncBsdSocketLib::Poller *const mPoller;
                    const std::string cNicIpAddress;
                    const std::string cMulticastGroup;
                    const uint16_t cOfferingPort;
                    const uint16_t cFindingPort;

                    static const size_t cBufferSize;
                    helper::ConcurrentQueue<std::vector<uint8_t>> mSendingQueueForOffering;
                    
                    //helper::ConcurrentQueue<std::vector<uint8_t>> mSendingQueueForSubscring;

                    AsyncBsdSocketLib::UdpClient mFindingUdpSocket;
                    AsyncBsdSocketLib::UdpClient mOfferingUdpSocket;
                    static const std::string cAnyIpAddress;
                    
                    /*
                    // mutex object
                    std::mutex mFindingRequestsMutex;
                    // trier to take mutex
                    std::unique_lock<std::mutex> mFindingRequestsLock;
                    std::condition_variable mFindingRequestsConditionVariable;
                    */

                    /**************************** poller functions  ***************************/

                    void onReceiveOffering();

                    void onReceiveFinding();

                    void onSendOfferingOrAck();
                    
                    void SendOfferingOrAck(const SomeIpSdMessage &message); 
                   
                    // void onSendSubscribing();
                    // void SendSubscring(const SomeIpSdMessage &message); 



                    /***************************** main functions ****************************/

                    void handleSubscribing(sd::SomeIpSdMessage &&message);

                    bool hasSubscribingEntry(const SomeIpSdMessage &message);

                    void handleOffering(sd::SomeIpSdMessage &&message);
                    
                    bool hasOfferingEntry(const SomeIpSdMessage &message, uint32_t &ttl) const;

                    bool ExtractInfoToStore(
                        const SomeIpSdMessage &message,
                        std::string &ipAddress,
                        uint16_t &port,
                        uint16_t &_serviceId,
                        uint16_t &_instanceId) const;

                    bool hasFindingEntry(const SomeIpSdMessage &message,
                        transportInfo &info,
                        uint16_t &serviceId,
                        uint16_t &instanceId,
                        uint8_t &majorVersion,
                        uint32_t &minorVersion 
                    ); // const;

                    void handleFinding(sd::SomeIpSdMessage &&message);

                    bool isRegisted(const registryKey& k, transportInfo &info) const;

                public:
                    void printRegistry();

                    /******************************* constructors  ******************************/

                    /// @brief Constructor
                    /// @param poller BSD sockets poller
                    /// @param nicIpAddress Network interface controller IPv4 address
                    /// @param multicastGroup Multicast group IPv4 address
                    /// @param port Multicast UDP port number
                    /// @throws std::runtime_error Throws when the UDP socket configuration failed
                    ServiceDiscoveryProcess(
                        AsyncBsdSocketLib::Poller *poller,
                        std::string nicIpAddress,
                        std::string multicastGroup,
                        uint16_t offeringPort,
                        uint16_t findingPort);

                    /**************************** deconstructor  ************************/

                    ~ServiceDiscoveryProcess();
                };
            }
        }
    }
}

#endif