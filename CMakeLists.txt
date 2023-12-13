cmake_minimum_required(VERSION 3.14)

project(demo)

set(CMAKE_CXX_STANDARD 14)

######################### source files ##################################
add_library(
   araCom

   ##### sockets #####
   sockets/include/communicator.h
   sockets/include/network_socket.h
   sockets/include/poller.h
   sockets/include/tcp_client.h
   sockets/include/tcp_listener.h
   sockets/include/udp_client.h
   sockets/src/communicator.cpp
   sockets/src/network_socket.cpp
   sockets/src/poller.cpp
   sockets/src/tcp_client.cpp
   sockets/src/tcp_listener.cpp
   sockets/src/udp_client.cpp

   #### helper ####
   helper/abstract_state_machine.h
   helper/abstract_network_layer.h
   helper/concurrent_queue.h
   helper/state.h
   helper/fsm.h
   helper/ipv4_address.h
   helper/payload_helper.h
   helper/payload_helper.cpp
   helper/ttl_timer.h
   helper/ttl_timer.cpp


   ############  someip message ############ 
   someip/someipMsg/someip_message.h
   someip/someipMsg/someip_message.cpp

   ############  someip_rpc_message ############ 
   someip/someipRpcMsg/someip_rpc_message.cpp
   someip/someipRpcMsg/someip_rpc_message.h

   ## receiver for someip_rpc_message ##
   someip/rpc/rpcs_responser.h
   someip/rpc/rpcs_responser.cpp
   someip/rpc/socket_rpc_server.h
   someip/rpc/socket_rpc_server.cpp

   ## sender for someip_rpc_message ##
   someip/rpc/rpcs_requester.h
   someip/rpc/rpcs_requester.cpp
   someip/rpc/socket_rpc_client.h
   someip/rpc/socket_rpc_client.cpp

   ############ someip_sd message ############
   someip/someipSdMsg/someip_sd_message.cpp
   someip/someipSdMsg/someip_sd_message.h

   ## entry ##
   someip/someipSdMsg/entry/entry_deserializer.h
   someip/someipSdMsg/entry/entry.cpp
   someip/someipSdMsg/entry/entry.h
   someip/someipSdMsg/entry/eventgroup_entry.cpp
   someip/someipSdMsg/entry/eventgroup_entry.h
   someip/someipSdMsg/entry/service_entry.cpp
   someip/someipSdMsg/entry/service_entry.h

   ## option ##
   someip/someipSdMsg/option/ipv4_endpoint_option.cpp
   someip/someipSdMsg/option/ipv4_endpoint_option.h
   someip/someipSdMsg/option/loadbalancing_option.cpp
   someip/someipSdMsg/option/loadbalancing_option.h
   someip/someipSdMsg/option/option_deserializer.h
   someip/someipSdMsg/option/option.cpp
   someip/someipSdMsg/option/option.h


   ################# sd ##################

   someip/sd/network_layer.cpp
   someip/sd/network_layer.h
   someip/sd/SomeIpSdAgent.h
   someip/sd/SomeIpSdServer.cpp
   someip/sd/SomeIpSdServer.h
   someip/sd/SomeIpSdClient.cpp
   someip/sd/SomeIpSdClient.h
   ## fsm for sd client ##
   someip/sd/fsm/fsmClient/client_service_state.h
   someip/sd/fsm/fsmClient/client_initial_wait_state.h
   someip/sd/fsm/fsmClient/client_initial_wait_state.cpp
   someip/sd/fsm/fsmClient/client_repetition_state.h
   someip/sd/fsm/fsmClient/client_repetition_state.cpp
   someip/sd/fsm/fsmClient/service_notseen_state.h
   someip/sd/fsm/fsmClient/service_notseen_state.cpp
   someip/sd/fsm/fsmClient/service_ready_state.h
   someip/sd/fsm/fsmClient/service_ready_state.cpp
   someip/sd/fsm/fsmClient/service_seen_state.h
   someip/sd/fsm/fsmClient/service_seen_state.cpp
   someip/sd/fsm/fsmClient/stopped_state.h
   someip/sd/fsm/fsmClient/stopped_state.cpp
   ## fsm for sd server ##
   someip/sd/fsm/notready_state.h
   someip/sd/fsm/timer_set_state.h
   someip/sd/fsm/initial_wait_state.h
   someip/sd/fsm/repetition_state.h   
   someip/sd/fsm/main_state.h
   
  
   
   #### provider and subscripter for events ####
   someip/pubsub/fsm/notsubscribed_state.h
   someip/pubsub/fsm/service_down_state.h
   someip/pubsub/fsm/subscribed_state.h

   someip/pubsub/event_requester.cpp
   someip/pubsub/event_requester.h
   someip/pubsub/socket_pubSub_client.cpp
   someip/pubsub/socket_pubSub_client.h

   someip/pubsub/socket_pubSub_server.cpp
   someip/pubsub/socket_pubSub_server.h
   someip/pubsub/event_provider.cpp
   someip/pubsub/event_provider.h
   


   ####### DiscoveryProcess #####
   someip/DiscoveryProcess/Process.h
   someip/DiscoveryProcess/Process.cpp   

   #### provider ####
   someip/service/fsm/notsubscribed_state.h
   someip/service/fsm/service_down_state.h
   someip/service/fsm/subscribed_state.h
   someip/service/provider.h
   someip/service/provider.cpp

   #### requester ####
   someip/service/requester.h
   someip/service/requester.cpp
)

######################### samples ##################################
#### execuatables for rpc ####
add_executable(server_rpc samples/demo_rpc_server.cpp)
target_link_libraries(server_rpc araCom)

add_executable(client_rpc samples/demo_rpc_client.cpp)
target_link_libraries(client_rpc araCom)

#### executables for pubSub ####
add_executable(client_pubSub samples/demo_pubSub_client.cpp)
target_link_libraries(client_pubSub araCom)

add_executable(server_pubSub samples/demo_pubSub_server.cpp)
target_link_libraries(server_pubSub araCom)

#### execuatables for demo ####
add_executable(sd samples/demo_sd.cpp)
target_link_libraries(sd araCom)

#### execuatables for demo ####
add_executable(process samples/demo_process.cpp)
target_link_libraries(process araCom)


#### execuatables for demo ####
add_executable(provider samples/demo_provider.cpp)
target_link_libraries(provider araCom)


#### execuatables for demo ####
add_executable(requester samples/demo_requester.cpp)
target_link_libraries(requester araCom)