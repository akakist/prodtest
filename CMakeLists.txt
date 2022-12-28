
cmake_minimum_required (VERSION 3.0)

project (megatron)

if(UNIX AND NOT APPLE)
SET(LINUX true)
endif()

SET(ENABLE_MYSQL true)
#SET(FreeBSD true)
SET(DFS true)
if(LINUX)
    set(CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.7/gcc_64)
    set(CMAKE_SHARED_LINKER_FLAGS "-Xlinker --export-dynamic -Xlinker -zdefs")
else()
    set(CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.14.1/clang_64)
endif()


OPTION(MT_CORE_SERVICES_SINGLE_PLUGING "Build apps/core-plugins - all core plugins in one file" ON)
include_directories(megatron/corelib 
			megatron/main 
			megatron/local  
			megatron/lib/jsoncpp-0.5.0/include 
			/usr/local/Cellar/openssl@3/3.0.7/include

		#	/usr/local/include 
			megatron  .  svs megatron/lib megatron/lib/sqlite3
			)

include_directories(system /usr/local/include)
#target_include_directories(foo PRIVATE /usr/local/include)
#INTERFACE_SYSTEM_INCLUDE_DIRECTORIES(/usr/local/include)

#add_definitions(-std=gnu++0x)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fPIC ")
# -g -ggdb3 -pg
link_directories( /usr/local/lib)
#link_directories(~/bin/lib /usr/local/lib)
IF(APPLE)
    SET(CMAKE_MACOSX_RPATH 1)
ENDIF()
#SET(ENABLE_QT_GUI on)
SET(DISABLE_UPNP on)
#SET(ENABLE_GTEST on)

IF(ENABLE_QT_GUI)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies (QtGui and QtCore).
find_package(Qt5Widgets REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS})

#find_package(Qt5Multimedia REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
add_definitions(${Qt5PrintSupport_DEFINITIONS})

find_package(Qt5Svg REQUIRED)
include_directories(${Qt5Svg_INCLUDE_DIRS})
add_definitions(${Qt5Svg_DEFINITIONS})

#find_package(Qt5WebEngine REQUIRED)
#include_directories(${Qt5WebEngine_INCLUDE_DIRS})
#add_definitions(${Qt5WebEngine_DEFINITIONS})

if (APPLE) 
      set(CMAKE_MACOSX_RPATH ON) 
      SET(CMAKE_SKIP_BUILD_RPATH TRUE) 
      SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 
      SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) 
endif () 

ENDIF()


SET(CAPS_IP "")
#SET(CAPS_IP "127.0.0.1")
SET(PORT 5500)
SET(CLIENTPORT 4401)

IF(ENABLE_QT_GUI)
    add_definitions(  -DQT5 -D__DDRAW)
    message(qt5)
ENDIF()

IF(LINUX)
SET(DL "dl")
ENDIF()

SET(PROJECT_BASE_DIR ~/prodtest-bin-${PORT})
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
add_definitions(  -D__WITH_ZLIB  -DUSE_JSONASSERT  )
# -DMUTEX_INSPECTOR_DEBUG -DDEBUG -DMUTEX_INSPECTOR_DEBUG   -DDEBUG -DMUTEX_INSPECTOR_DEBUG
IF(DISABLE_UPNP)
    add_definitions(  -DWITHOUT_UPNP )
ENDIF()
#-DMUTEX_INSPECTOR_DEBUG  -DWITH_SLOG -DMUTEX_INSPECTOR_DEBUG
#add_definitions(-O3)     -DWITH_ZXING -DWITH_ZBAR  -DWITHOUT_UPNP  
#-DDEBUG  -DMUTEX_INSPECTOR_DEBUG -DDEBUG

#find_library(GTest REQUIRED)

ADD_SUBDIRECTORY(megatron)


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/prodtest  ${CMAKE_CURRENT_BINARY_DIR}/prodtest)
