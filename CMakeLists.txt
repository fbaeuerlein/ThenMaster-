#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_device_twin_and_methods_sample
cmake_minimum_required(VERSION 3.13)

#compileAsC99()
add_compile_options(-std=c++14)

set(SOURCES
    main.cpp
)

# set(HEADERS
#     IoTHubConnection.cxx
# )

include_directories(
    ./include
    )

add_executable(test ${SOURCES} ${HEADERS})

target_link_libraries(test
    pthread
)





# target_link_libraries(test PUBLIC 

# )    

# link_libraries("-static")

# target_link_libraries(iothub_client_device_twin_and_methods_sample
#     aziotsharedutil
#     iothub_client
#     iothub_client_amqp_transport
#     iothub_client_amqp_ws_transport
#     iothub_client_http_transport
#     iothub_client_mqtt_transport
#     iothub_client_mqtt_ws_transport
#     uamqp
#     uhttp
#     umqtt
#     prov_auth_client
#     hsm_security_client
#     serializer
#     parson
# )
