#cmake_minimum_required (VERSION 2.6)

include_directories(../../lib/miniupnp-master)

add_executable(test-metrics 
mainTestMetrics.cpp
main_.cpp
testMetricsService.cpp

)

target_link_libraries(test-metrics megatron-main Metrics  zsingle megatron-core crypto  z pthread dl)


#install(TARGETS test-metrics DESTINATION ${PLUGIN_TARGET_DIR} )
add_test(NAME test-metrics
         COMMAND test-metrics)


