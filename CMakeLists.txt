
#cmake_minimum_required (VERSION 2.6)

project (megatron)

if(UNIX AND NOT APPLE)
SET(LINUX true)
endif()


#set(M1 true)


#SET(ENABLE_MYSQL true)
#SET(FreeBSD true)
SET(DFS true)
if(LINUX)
    set(CMAKE_SHARED_LINKER_FLAGS "-Xlinker --export-dynamic -Xlinker -zdefs")
else()
endif()

    include(CTest)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)




OPTION(MT_CORE_SERVICES_SINGLE_PLUGING "Build apps/core-plugins - all core plugins in one file" ON)


find_package(OpenSSL REQUIRED)

include_directories(megatron/corelib 
			megatron/main 
			megatron/local  
			megatron/lib/jsoncpp-0.5.0/include 
#			${OPENSSL_INCLUDE_DIR}
			/usr/local/Cellar/openssl@3/3.0.7/include
			/opt/homebrew/Cellar/mysql-client/8.0.32/include
			/opt/homebrew/Cellar/libharu/2.4.3/include
			megatron  .  svs megatron/lib megatron/lib/sqlite3
			)

if(M1)
    link_directories(
	/opt/homebrew/Cellar/mysql-client/8.0.32/lib
	/opt/homebrew/Cellar/openssl@3/3.0.8/lib
    /opt/homebrew/Cellar/libharu/2.4.3/lib
    )
endif()
include_directories(system /usr/local/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fPIC ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC ")
# -g -ggdb3 -pg
link_directories( /usr/local/lib)
#link_directories(~/bin/lib /usr/local/lib)
IF(APPLE)
    SET(CMAKE_MACOSX_RPATH 1)
ENDIF()
SET(DISABLE_UPNP on)
#SET(ENABLE_GTEST on)


SET(CAPS_IP "test.com")
#SET(CAPS_IP "127.0.0.1")
SET(PORT 10100)
SET(CLIENTPORT 10101)


IF(LINUX)
SET(DL "dl")
ENDIF()

SET(PROJECT_BASE_DIR ~/bin-${PORT})
SET(PLUGIN_TARGET_DIR ${PROJECT_BASE_DIR}/plugin)
SET(BIN_TARGET_DIR ${PROJECT_BASE_DIR}/bin)
SET(LIB_TARGET_DIR ${PROJECT_BASE_DIR}/lib)
SET(LOG_TARGET_DIR ${PROJECT_BASE_DIR}/log)
SET(DATA_TARGET_DIR ${PROJECT_BASE_DIR}/data)
SET(CONFIG_TARGET_DIR ${PROJECT_BASE_DIR}/conf)
SET(CACHE_TARGET_DIR ${PROJECT_BASE_DIR}/cache)


CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/megatron/megatron_config.h.in"
                "${PROJECT_SOURCE_DIR}/megatron/megatron_config.h"
                NEWLINE_STYLE UNIX )


set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE )
add_definitions(  -D__WITH_ZLIB  -DUSE_JSONASSERT   )
#-DMUTEX_INSPECTOR_DEBUG -DDEBUG
IF(DISABLE_UPNP)
#    add_definitions(  -DWITHOUT_UPNP  -DMUTEX_INSPECTOR_DEBUG )
ENDIF()
#-DMUTEX_INSPECTOR_DEBUG  -DWITH_SLOG -DMUTEX_INSPECTOR_DEBUG
#add_definitions(-O3)     -DWITH_ZXING -DWITH_ZBAR  -DWITHOUT_UPNP  
#-DDEBUG  -DMUTEX_INSPECTOR_DEBUG -DDEBUG

#find_library(GTest REQUIRED)

ADD_SUBDIRECTORY(megatron)

    

#ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/../utils2)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/megatron/lib/miniupnp/miniupnpc  ${CMAKE_CURRENT_BINARY_DIR}/miniupnpc)






add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cobrazz  ${CMAKE_CURRENT_BINARY_DIR}/cobrazz)

