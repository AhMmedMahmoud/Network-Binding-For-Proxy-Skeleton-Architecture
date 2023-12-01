#ifndef SD_NETWORK_LAYER_H
#define SD_NETWORK_LAYER_H

#include "../../sockets/include/poller.h"
#include "../../sockets/include/udp_client.h"
#include "../../helper/concurrent_queue.h"
#include "../../helper/network_layer.h"
#include "../someipSdMsg/someip_sd_message.h"

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
                /// @brief SOME/IP service discovery multicast network layer
                class SdNetworkLayer : public helper::NetworkLayer<SomeIpSdMessage>
                {
                    /******************************* inherited ********************************
                        ////// private attribures //////
                        std::map<void *, std::function<void(T)>> mReceiverCallbacks;
                        
                        ////// public metohds //////
                        void SetReceiver(void *object, std::function<void(T)> receiver)
                        {
                            mReceiverCallbacks[object] = receiver;
                        }

                        ////// private metohds //////
                        void FireReceiverCallbacks(const std::vector<uint8_t> &payload)
                        {
                            for (auto objectCallbackPair : mReceiverCallbacks)
                            {
                                // Create the received message from the received payload
                                T _receivedMessage{T::Deserialize(payload)};

                                std::function<void(T)> _callback = objectCallbackPair.second;
                                _callback(std::move(_receivedMessage));
                            }
                        }
                    ***************************************************************************/
                private:
                    /*********************** extra attributes ******************************/

                    AsyncBsdSocketLib::Poller *const mPoller;
                    const std::string cNicIpAddress;
                    const std::string cMulticastGroup;
                    const uint16_t cPort;


                    static const size_t cBufferSize;
                    helper::ConcurrentQueue<std::vector<uint8_t>> mSendingQueue;
                    AsyncBsdSocketLib::UdpClient mUdpSocket;
                    static const std::string cAnyIpAddress;
                    


                    /**************************** backend functions  **********************************/

                    void onReceive();
                    void onSend();

                public:
                    /******************************* constructors  ******************************/

                    /// @brief Constructor
                    /// @param poller BSD sockets poller
                    /// @param nicIpAddress Network interface controller IPv4 address
                    /// @param multicastGroup Multicast group IPv4 address
                    /// @param port Multicast UDP port number
                    /// @throws std::runtime_error Throws when the UDP socket configuration failed
                    SdNetworkLayer(
                        AsyncBsdSocketLib::Poller *poller,
                        std::string nicIpAddress,
                        std::string multicastGroup,
                        uint16_t port);



                    /*************** override virtual functions inherited from parent*************/

                    void Send(const SomeIpSdMessage &message) override;



                    /**************************** override deconstructor  ************************/

                    ~SdNetworkLayer() override;
                };
            }
        }
    }
}

#endif