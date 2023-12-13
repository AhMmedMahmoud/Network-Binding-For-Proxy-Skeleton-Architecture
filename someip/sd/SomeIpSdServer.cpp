#include <stdexcept>
#include <thread>
#include "SomeIpSdServer.h"
#include <iostream>

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace sd
            {
                /**************************** useful in InvokeHandler *********************/

                bool SomeIpSdServer::hasFindingEntry(const SomeIpSdMessage &message) const
                {
                    // Iterate over all the message entry to search for the first Service Finding entry
                    for (auto &_entry : message.Entries())
                    {
                        if (_entry->Type() == entry::EntryType::Finding)
                        {
                            if (auto _serviceEnty = dynamic_cast<entry::ServiceEntry *>(_entry.get()))
                            {
                                // Compare service ID, instance ID, major version, and minor version
                                bool _result =
                                    (_serviceEnty->ServiceId() == mServiceId) &&
                                    (_serviceEnty->InstanceId() == entry::Entry::cAnyInstanceId ||
                                     _serviceEnty->InstanceId() == mInstanceId) &&
                                    (_serviceEnty->MajorVersion() == entry::Entry::cAnyMajorVersion ||
                                     _serviceEnty->MajorVersion() == mMajorVersion) &&
                                    (_serviceEnty->MinorVersion() == entry::ServiceEntry::cAnyMinorVersion ||
                                     _serviceEnty->MinorVersion() == mMinorVersion);

                                return _result;
                            }
                        }
                    }

                    return false;
                }



                /******************** function take any someip/sd message *****************/

                void SomeIpSdServer::InvokeHandler(SomeIpSdMessage &&message)
                {
                    bool _matches = hasFindingEntry(message);
                    // Enqueue the offer if the finding message matches the service
                    if (_matches)
                    {
                        while (!mMessageBuffer.TryEnqueue(std::move(message)))
                        {
                            std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        }
                    }
                }



                /**************************** useful in constructor *********************/

                void SomeIpSdServer::sendOffer()
                {
                    std::cout << "sendOffer of SomeIpSdServer is called\n";

                    // Avoid starvation
                    while (!mMessageBuffer.Empty())
                    {                        
                        SomeIpSdMessage _message;
                        if (mMessageBuffer.TryDequeue(_message))
                        {
                            this->CommunicationLayer->Send(mOfferServiceMessage);
                            mOfferServiceMessage.IncrementSessionId();
                        }

                        std::this_thread::yield();
                    }
                }



                /************************* useful in notReady state *************************/
                
                void SomeIpSdServer::onServiceStopped()
                {
                    this->CommunicationLayer->Send(mStopOfferMessage);
                    mStopOfferMessage.IncrementSessionId();
                }


  
                /******************************* constructor  *******************************/

                SomeIpSdServer::SomeIpSdServer(
                    helper::AbstractNetworkLayer<SomeIpSdMessage> *networkLayer,
                    uint16_t serviceId,
                    uint16_t instanceId,
                    uint8_t majorVersion,
                    uint32_t minorVersion,
                    helper::Ipv4Address ipAddress,
                    uint16_t port,
                    int initialDelayMin,
                    int initialDelayMax,
                    int repetitionBaseDelay,
                    int cycleOfferDelay,
                    uint32_t repetitionMax) : SomeIpSdAgent<helper::SdServerState>(networkLayer),
                                              mNotReadyState
                                              (
                                                std::bind(&SomeIpSdServer::onServiceStopped, this)
                                              ),
                                              mInitialWaitState
                                              (
                                                helper::SdServerState::InitialWaitPhase,
                                                helper::SdServerState::RepetitionPhase,
                                                helper::SdServerState::NotReady,
                                                std::bind(&SomeIpSdServer::sendOffer, this),
                                                initialDelayMin,
                                                initialDelayMax
                                              ),
                                              mRepetitionState
                                              (
                                                helper::SdServerState::RepetitionPhase,
                                                helper::SdServerState::MainPhase,
                                                helper::SdServerState::NotReady,
                                                std::bind(&SomeIpSdServer::sendOffer, this),
                                                repetitionMax,
                                                repetitionBaseDelay
                                              ),
                                              mMainState
                                              (
                                                std::bind(&SomeIpSdServer::sendOffer, this),
                                                cycleOfferDelay
                                              ),
                                              mServiceId{serviceId},
                                              mInstanceId{instanceId},
                                              mMajorVersion{majorVersion},
                                              mMinorVersion{minorVersion}
                {
                    // prepare offering entry
                    auto _offerServiceEntry
                    {
                        entry::ServiceEntry::CreateOfferServiceEntry( serviceId,
                                                                      instanceId,
                                                                      majorVersion,
                                                                      minorVersion
                                                                    )
                    };

                    // prepare stop offering entry
                    auto _stopOfferEntry
                    {
                        entry::ServiceEntry::CreateStopOfferEntry( serviceId,
                                                                   instanceId,
                                                                   majorVersion,
                                                                   minorVersion
                                                                 )
                    };

                    // prepare unicast endpoint option
                    auto _offerEndpointOption
                    {
                        option::Ipv4EndpointOption::CreateUnitcastEndpoint( false,
                                                                            ipAddress,
                                                                            option::Layer4ProtocolType::Tcp,
                                                                            port
                                                                          )
                    };

                    auto _stopOfferEndpointOption
                    {
                        option::Ipv4EndpointOption::CreateUnitcastEndpoint( false,
                                                                            ipAddress,
                                                                            option::Layer4ProtocolType::Tcp,
                                                                            port
                                                                          )
                    };


                    // prepare SOMEIP/SD message contain offering entry to use at need
                    _offerServiceEntry->AddFirstOption(std::move(_offerEndpointOption));
                    mOfferServiceMessage.AddEntry(std::move(_offerServiceEntry));


                    // prepare SOMEIP/SD message contain stop offering entry to use at need
                    _stopOfferEntry->AddFirstOption(std::move(_stopOfferEndpointOption));
                    mStopOfferMessage.AddEntry(std::move(_stopOfferEntry));


                    // add states to StateMachine and choose that NotReady as start state
                    // for each state regist that its FSM which is represented by StateMachine 
                    this->StateMachine.Initialize( 
                                   {&mNotReadyState, &mInitialWaitState, &mRepetitionState, &mMainState},
                                   helper::SdServerState::NotReady
                                                 );


                    // regist receive handler to poller
                    auto _receiver = std::bind( &SomeIpSdServer::InvokeHandler, this, std::placeholders::_1);
                    this->CommunicationLayer->SetReceiver(this, _receiver);
                }


                /*
                    void NotReadyState::ServiceActivated()
                    {
                        Transit(helper::SdServerState::InitialWaitPhase);
                    }
                */

                /************************* functions that parent need ************************/

                void SomeIpSdServer::StartAgent(helper::SdServerState state)
                {
                    if (state == helper::SdServerState::NotReady)
                    {
                        std::cout << "----- initial state is NotReady -----\n";

                        // Set the timer from a new thread with a random initial delay.
                        /*
                            transit from NotReady state into Initial wait state 
                            and call activate on function Initial wait state
                        */
                        this->Future = std::async(  std::launch::async,
                                                    &fsm::NotReadyState::ServiceActivated,
                                                    &mNotReadyState
                                                 );
                    }
                }

                void SomeIpSdServer::StopAgent()
                {
                    mInitialWaitState.ServiceStopped();
                    mRepetitionState.ServiceStopped();
                    mMainState.ServiceStopped();
                    Join();
                }



                /************************ override desctructor  *****************************/

                SomeIpSdServer::~SomeIpSdServer()
                {
                    Stop();
                }
            }
        }
    }
}