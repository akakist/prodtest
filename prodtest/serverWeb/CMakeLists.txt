cmake_minimum_required (VERSION 3.0)

add_library(prodtestWebServer SHARED
prodtestWebServer.cpp
prodtestWebServerService.cpp
)

target_link_libraries(prodtestWebServer   megatron-core pthread)

install(TARGETS prodtestWebServer DESTINATION ${PLUGIN_TARGET_DIR})

