cmake_minimum_required(VERSION 3.14)
project(CommunicationManager)
set(CMAKE_CXX_STANDARD 14)

######################### source files ##################################
add_library(
   araCom

   ## sockets ##
   sockets/include/communicator.h
   sockets/src/communicator.cpp
   sockets/include/network_socket.h
   sockets/src/network_socket.cpp
   sockets/include/poller.h
   sockets/src/poller.cpp
   sockets/include/tcp_client.h
   sockets/src/tcp_client.cpp
   sockets/include/tcp_listener.h
   sockets/src/tcp_listener.cpp
   sockets/include/udp_client.h
   sockets/src/udp_client.cpp

   ## helper ##
   helper/abstract_state_machine.h
   helper/concurrent_queue.h
   helper/state.h
   helper/fsm.h
   helper/ipv4_address.h
   helper/payload_helper.h
   helper/payload_helper.cpp
   helper/instance_id.h

   ## someip message ##
   someip/someipMsg/someip_message.h
   someip/someipMsg/someip_message.cpp

   ## someip_rpc_message ##
   someip/someipRpcMsg/someip_rpc_message.cpp
   someip/someipRpcMsg/someip_rpc_message.h

   ## rpcs ##
   someip/rpc/rpcs_responser.h
   someip/rpc/rpcs_responser.cpp
   someip/rpc/socket_rpc_server.h
   someip/rpc/socket_rpc_server.cpp
   someip/rpc/rpcs_requester.h
   someip/rpc/rpcs_requester.cpp
   someip/rpc/socket_rpc_client.h
   someip/rpc/socket_rpc_client.cpp

   ## events ##
   someip/events/fsm/notsubscribed_state.h
   someip/events/fsm/service_down_state.h
   someip/events/fsm/subscribed_state.h
   someip/events/event_requester.cpp
   someip/events/event_requester.h
   someip/events/socket_event_client.cpp
   someip/events/socket_event_client.h
   someip/events/socket_event_server.cpp
   someip/events/socket_event_server.h
   someip/events/event_provider.cpp
   someip/events/event_provider.h

   ## someip_sd message ##
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
 
   ## DiscoveryProcess ##
   someip/DiscoveryProcess/Process.h
   someip/DiscoveryProcess/Process.cpp   

   ## service which is an event with rpcs ##
   someip/service/event_rpcs/provider.h
   someip/service/event_rpcs/provider.cpp
   someip/service/event_rpcs/requester.h
   someip/service/event_rpcs/requester.cpp
   someip/service/event_rpcs/methods.h
 
   ## service which is an event only ##
   someip/service/event_Only/event_service_provider.h
   someip/service/event_Only/event_service_provider.cpp
   someip/service/event_Only/event_service_requester.h
   someip/service/event_Only/event_service_requester.cpp

   ## config ##
   config.h

   ## proxy_skeleton ##
   Proxy_Skeleton/Proxy.h
   Proxy_Skeleton/Proxy.cpp
   Proxy_Skeleton/Skeleton.h
   Proxy_Skeleton/Skeleton.cpp

   ## sm_interfaces ##
   sm_interfaces/TriggerInProxy.h
   sm_interfaces/TriggerInProxy.cpp
   sm_interfaces/TriggerInSkeleton.h
   sm_interfaces/TriggerInSkeleton.cpp
   sm_interfaces/TriggerOutProxy.h
   sm_interfaces/TriggerOutProxy.cpp
   sm_interfaces/TriggerOutSkeleton.h
   sm_interfaces/TriggerOutSkeleton.cpp
   sm_interfaces/TriggerInOutProxy.h
   sm_interfaces/TriggerInOutProxy.cpp
   sm_interfaces/TriggerInOutSkeleton.h
   sm_interfaces/TriggerInOutSkeleton.cpp
)



################################################################################
##                                execuatables                                ##
################################################################################

#### execuatables for rpc ####
#add_executable(server_rpc samples/demo_rpc_server.cpp)
#target_link_libraries(server_rpc araCom)
#add_executable(client_rpc samples/demo_rpc_client.cpp)
#target_link_libraries(client_rpc araCom)


#### executables for events ####
#add_executable(client_event samples/demo_event_client.cpp)
#target_link_libraries(client_event araCom)
#add_executable(server_event samples/demo_event_server.cpp)
#target_link_libraries(server_event araCom)


#### execuatables for ServiceDiscoveryProcess ####
add_executable(process samples/demo_process.cpp)
target_link_libraries(process araCom)


#### execuatables for service provider ####
#add_executable(provider samples/demo_service_provider.cpp)
#target_link_libraries(provider araCom)


#### execuatables for service requester ####
#add_executable(requester samples/demo_service_requester.cpp)
#target_link_libraries(requester araCom)


#### executables for proxy ####
add_executable(proxy samples/demo_proxy.cpp)
target_link_libraries(proxy araCom)

#### executables for skeleton ####
add_executable(skeleton samples/demo_skeleton.cpp)
target_link_libraries(skeleton araCom)


#####################################################################################
##                                  SM excutables                                  ##
#####################################################################################

#### executables for TriggerInSkeleton ####
add_executable(TriggerInSkeleton sm_interfaces_demos/demo_TriggerInSkeleton.cpp)
target_link_libraries(TriggerInSkeleton araCom)

#### executables for TriggerInProxy ####
add_executable(TriggerInProxy sm_interfaces_demos/demo_TriggerInProxy.cpp)
target_link_libraries(TriggerInProxy araCom)

#### executables for TriggerOutSkeleton ####
add_executable(TriggerOutSkeleton sm_interfaces_demos/demo_TriggerOutSkeleton.cpp)
target_link_libraries(TriggerOutSkeleton araCom)

#### executables for TriggerOutProxy ####
add_executable(TriggerOutProxy sm_interfaces_demos/demo_TriggerOutProxy.cpp)
target_link_libraries(TriggerOutProxy araCom)


add_executable(tes samples/demo_someipRpc.cpp)
target_link_libraries(tes araCom)
