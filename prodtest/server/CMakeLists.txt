cmake_minimum_required (VERSION 2.6)

add_library(prodtestServer SHARED
    prodtestServer.cpp
    prodtestServerService.cpp

)

target_link_libraries(prodtestServer   megatron-core pthread)

install(TARGETS prodtestServer DESTINATION ${PLUGIN_TARGET_DIR})

