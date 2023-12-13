#ifndef SD_NETWORK_LAYER_H
#define SD_NETWORK_LAYER_H

#include "../../sockets/include/poller.h"
#include "../../sockets/include/udp_client.h"

#include "../../helper/concurrent_queue.h"
#include "../../helper/ipv4_address.h"

#include "../someipSdMsg/entry/service_entry.h"
#include "../someipSdMsg/entry/eventgroup_entry.h"
#include "../someipSdMsg/option/ipv4_endpoint_option.h"
#include "../someipSdMsg/someip_sd_message.h"

//#include <iostream>

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
                
                struct myKey
                {
                    uint16_t serviceId;
                    uint16_t instanceId;

                    bool operator<(const myKey& other) const {
                        if (serviceId != other.serviceId) {
                            return serviceId < other.serviceId;
                        } else {
                            return instanceId < other.instanceId;
                        }
                    }
                };

                struct transportInfo 
                {
                    std::string ipAddress;
                    uint16_t port;
                    protocol proto;
                };

                class ServiceRegistryProcess
                {
                private:
                    std::map<myKey, transportInfo> table;
                    
                    void storeInfoOfServiceInstance(myKey k, transportInfo info)
                    {
                        table[k] = info;
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
                    


                    /**************************** poller functions  ***************************/

                    void onReceiveOffering();
                    void onReceiveFinding();
                    void onSendOfferingOrAck();
                    
                    // void onSendSubscribing();
                    
                    void SendOfferingOrAck(const SomeIpSdMessage &message); 
                   
                    // void SendSubscring(const SomeIpSdMessage &message); 



                    /***************************** main functions ****************************/

                    void handleSubscribing(sd::SomeIpSdMessage &&message);

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
                    ) const;

                    void handleFinding(sd::SomeIpSdMessage &&message);

                    bool hasSubscribingEntry(const SomeIpSdMessage &message);

                    bool isRegisted(const myKey& k, transportInfo &info) const;

                public:
                    void printRegistry();

                    /******************************* constructors  ******************************/

                    /// @brief Constructor
                    /// @param poller BSD sockets poller
                    /// @param nicIpAddress Network interface controller IPv4 address
                    /// @param multicastGroup Multicast group IPv4 address
                    /// @param port Multicast UDP port number
                    /// @throws std::runtime_error Throws when the UDP socket configuration failed
                    ServiceRegistryProcess(
                        AsyncBsdSocketLib::Poller *poller,
                        std::string nicIpAddress,
                        std::string multicastGroup,
                        uint16_t offeringPort,
                        uint16_t findingPort);


                    /**************************** deconstructor  ************************/

                    ~ServiceRegistryProcess();
                };
            }
        }
    }
}

#endif