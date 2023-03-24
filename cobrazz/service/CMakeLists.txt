#cmake_minimum_required (VERSION 2.6)


add_library(MetricsHttpProviderService  SHARED
CompositeMetricsProvider.cpp
MetricsHttpProvider.cpp
MetricsHttpProviderService.cpp

)
add_library(Metrics  STATIC
CompositeMetricsProvider.cpp
MetricsHttpProvider.cpp
MetricsHttpProviderService.cpp

)


target_link_libraries(MetricsHttpProviderService megatron-core pthread)
install(TARGETS MetricsHttpProviderService DESTINATION ${PLUGIN_TARGET_DIR})

ADD_SUBDIRECTORY(test)
