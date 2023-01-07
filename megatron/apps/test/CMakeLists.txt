#cmake_minimum_required (VERSION 2.6)

add_executable(mt-test-${PORT}.exe 
mainTest.cpp
)
include_directories(../../main)
if(!ENABLE_QT_GUI)
IF(FreeBSD)
target_link_libraries(mt-test-${PORT}.exe megatron-main zsingle megatron-core ssl crypto  z  pthread sqlitecpp sqlite3 event event_pthreads )
ELSE()
target_link_libraries(mt-test-${PORT}.exe megatron-main zsingle megatron-core ssl crypto  z  pthread sqlitecpp sqlite3 dl event event_pthreads )
ENDIF()
else()
IF(FreeBSD)
target_link_libraries(mt-test-${PORT}.exe megatron-main zsingle megatron-core ssl crypto  z  pthread sqlitecpp sqlite3 ${Qt5Widgets_LIBRARIES} event event_pthreads )
ELSE()
target_link_libraries(mt-test-${PORT}.exe megatron-main zsingle megatron-core ssl crypto  z  pthread sqlitecpp sqlite3 dl ${Qt5Widgets_LIBRARIES} event event_pthreads )
ENDIF()
endif()


install(TARGETS mt-test-${PORT}.exe DESTINATION ${BIN_TARGET_DIR})

