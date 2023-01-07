cmake_minimum_required (VERSION 3.0)

add_library(prodtestServer SHARED
    prodtestServer.cpp
    prodtestServerService.cpp

)

target_link_libraries(prodtestServer   megatron-core pthread event)

install(TARGETS prodtestServer DESTINATION ${PLUGIN_TARGET_DIR})

