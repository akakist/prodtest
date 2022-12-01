QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../../megatron/corelib ../../megatron/local ../../megatron/lib/jsoncpp-0.5.0/include\
            ../../megatron/main ../../megatron /usr/local/include\
            ../../prodtest/Event/


DEFINES += DEBUG
LIBS += -L/usr/local/lib -lssl -lcrypto

SOURCES += \
        ../../prodtest/server/prodtestServerService.cpp \
        ../../prodtest/serverWeb/prodtestWebServerService.cpp \
        ../../megatron/corelib/DBH.cpp \
        ../../megatron/corelib/Integer.cpp \
        ../../megatron/corelib/Rational.cpp \
        ../../megatron/corelib/_QUERY.cpp \
        ../../megatron/corelib/__crc16.cpp \
        ../../megatron/corelib/__crc32.cpp \
        ../../megatron/corelib/__crc64.cpp \
        ../../megatron/corelib/_bitStream.cpp \
        ../../megatron/corelib/broadcaster.cpp \
        ../../megatron/corelib/commonError.cpp \
        ../../megatron/corelib/configDB.cpp \
        ../../megatron/corelib/epoll_socket_info.cpp \
        ../../megatron/corelib/event.cpp \
        ../../megatron/corelib/eventDeque.cpp \
        ../../megatron/corelib/httpConnection.cpp \
        ../../megatron/corelib/ioBuffer.cpp \
        ../../megatron/corelib/jpge.cpp \
        ../../megatron/corelib/js_utils.cpp \
        ../../megatron/corelib/jsonHandler.cpp \
        ../../megatron/corelib/listenerBase.cpp \
        ../../megatron/corelib/listenerBuffered.cpp \
        ../../megatron/corelib/listenerBuffered1Thread.cpp \
        ../../megatron/corelib/listenerPolled.cpp \
        ../../megatron/corelib/listenerSimple.cpp \
        ../../megatron/corelib/logging.cpp \
        ../../megatron/corelib/msockaddr_in.cpp \
        ../../megatron/corelib/mtimespec.cpp \
        ../../megatron/corelib/mutexInspector.cpp \
        ../../megatron/corelib/mutexable.cpp \
        ../../megatron/corelib/neighbours.cpp \
        ../../megatron/corelib/networkMultiplexor.cpp \
        ../../megatron/corelib/objectHandler.cpp \
        ../../megatron/corelib/route_t.cpp \
        ../../megatron/corelib/socketsContainer.cpp \
        ../../megatron/corelib/trashable.cpp \
        ../../megatron/corelib/unknown.cpp \
        ../../megatron/corelib/url.cpp \
        ../../megatron/corelib/webDumpable.cpp \
        ../../megatron/lib/jsoncpp-0.5.0/src/lib_json/json_reader.cpp \
        ../../megatron/lib/jsoncpp-0.5.0/src/lib_json/json_value.cpp \
        ../../megatron/lib/jsoncpp-0.5.0/src/lib_json/json_writer.cpp \
        ../../megatron/main/CInstance.cpp \
        ../../megatron/main/CUtils.cpp \
        ../../megatron/main/configObj.cpp \
        ../../megatron/main/iutils_decl.cpp \
        ../../megatron/main/megatron.cpp \
        ../../megatron/main/megatronClient.cpp \
        ../../megatron/main/threadNameCtl.cpp \
        ../../megatron/modules/http/httpService.cpp \
        ../../megatron/modules/http/mime_types.cpp \
        ../../megatron/modules/objectProxy/objectProxyService.cpp \
        ../../megatron/modules/oscar/oscarService.cpp \
        ../../megatron/modules/rpc/rpcService.cpp \
        ../../megatron/modules/socket/socketService.cpp \
        ../../megatron/modules/telnet/telnetService.cpp \
        ../../megatron/modules/timer/timerService.cpp \
        ../../megatron/modules/webHandler/webHandlerService.cpp \
        main-pt.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../prodtest/Event/Ping.h \
    ../../prodtest/Event/___prodtestEvent.h \
    ../../prodtest/server/prodtestServerService.h \
    ../../prodtest/serverSql/prodtestWebServerService.h \
    ../../prodtest/serverWeb/prodtestWebServerService.h \
    ../../megatron/corelib/AUTHCOOKIE_id.h \
    ../../megatron/corelib/CHUNK_id.h \
    ../../megatron/corelib/CLASS_id.h \
    ../../megatron/corelib/CONTAINER.h \
    ../../megatron/corelib/CapsId.h \
    ../../megatron/corelib/DATA_id.h \
    ../../megatron/corelib/DBH.h \
    ../../megatron/corelib/DB_POSTFIX.h \
    ../../megatron/corelib/DB_id.h \
    ../../megatron/corelib/EVENT_id.h \
    ../../megatron/corelib/ErrorDispatcherHelper.h \
    ../../megatron/corelib/GlobalCookie_id.h \
    ../../megatron/corelib/IConfigDB.h \
    ../../megatron/corelib/IConfigObj.h \
    ../../megatron/corelib/IGEOIP.h \
    ../../megatron/corelib/IInstance.h \
    ../../megatron/corelib/ILogger.h \
    ../../megatron/corelib/IObjectProxy.h \
    ../../megatron/corelib/IProgress.h \
    ../../megatron/corelib/IRPC.h \
    ../../megatron/corelib/ISSL.h \
    ../../megatron/corelib/ITests.h \
    ../../megatron/corelib/IThreadNameController.h \
    ../../megatron/corelib/IUtils.h \
    ../../megatron/corelib/Integer.h \
    ../../megatron/corelib/JAVACOOKIE_id.h \
    ../../megatron/corelib/REF.h \
    ../../megatron/corelib/RSA_id.h \
    ../../megatron/corelib/Rational.h \
    ../../megatron/corelib/Real.h \
    ../../megatron/corelib/SEQ_id.h \
    ../../megatron/corelib/SERVICE_id.h \
    ../../megatron/corelib/SHA1_id.h \
    ../../megatron/corelib/SOCKET_fd.h \
    ../../megatron/corelib/SOCKET_id.h \
    ../../megatron/corelib/URI.h \
    ../../megatron/corelib/USER_id.h \
    ../../megatron/corelib/VERSION_id.h \
    ../../megatron/corelib/_QUERY.h \
    ../../megatron/corelib/__crc16.h \
    ../../megatron/corelib/__crc32.h \
    ../../megatron/corelib/__crc64.h \
    ../../megatron/corelib/_bitStream.h \
    ../../megatron/corelib/broadcaster.h \
    ../../megatron/corelib/bufferVerify.h \
    ../../megatron/corelib/colorOutput.h \
    ../../megatron/corelib/commonError.h \
    ../../megatron/corelib/compat_win32.h \
    ../../megatron/corelib/configDB.h \
    ../../megatron/corelib/dfsErrors.h \
    ../../megatron/corelib/e_poll.h \
    ../../megatron/corelib/epoll_socket_info.h \
    ../../megatron/corelib/errorDispatcherCodes.h \
    ../../megatron/corelib/event.h \
    ../../megatron/corelib/eventDeque.h \
    ../../megatron/corelib/eventEnum.h \
    ../../megatron/corelib/evfilt_name.h \
    ../../megatron/corelib/gTry.h \
    ../../megatron/corelib/httpConnection.h \
    ../../megatron/corelib/ifaces.h \
    ../../megatron/corelib/ioBuffer.h \
    ../../megatron/corelib/jpeg_decoder.h \
    ../../megatron/corelib/jpge.h \
    ../../megatron/corelib/js_utils.h \
    ../../megatron/corelib/jsonHandler.h \
    ../../megatron/corelib/linkinfo.h \
    ../../megatron/corelib/listenerBase.h \
    ../../megatron/corelib/listenerBuffered.h \
    ../../megatron/corelib/listenerBuffered1Thread.h \
    ../../megatron/corelib/listenerPolled.h \
    ../../megatron/corelib/listenerSimple.h \
    ../../megatron/corelib/logging.h \
    ../../megatron/corelib/msockaddr_in.h \
    ../../megatron/corelib/mtimespec.h \
    ../../megatron/corelib/mutexInspector.h \
    ../../megatron/corelib/mutex_inspector_entry.h \
    ../../megatron/corelib/mutexable.h \
    ../../megatron/corelib/neighbours.h \
    ../../megatron/corelib/networkMultiplexor.h \
    ../../megatron/corelib/objectHandler.h \
    ../../megatron/corelib/pollable.h \
    ../../megatron/corelib/queryResult.h \
    ../../megatron/corelib/refstring.h \
    ../../megatron/corelib/route_t.h \
    ../../megatron/corelib/serviceEnum.h \
    ../../megatron/corelib/socketsContainer.h \
    ../../megatron/corelib/st_FILE.h \
    ../../megatron/corelib/st_malloc.h \
    ../../megatron/corelib/timerHelper.h \
    ../../megatron/corelib/tools_mt.h \
    ../../megatron/corelib/trashable.h \
    ../../megatron/corelib/unknown.h \
    ../../megatron/corelib/unknownCastDef.h \
    ../../megatron/corelib/url.h \
    ../../megatron/corelib/userObjectHandler.h \
    ../../megatron/corelib/version_mega.h \
    ../../megatron/corelib/webDumpable.h \
    ../../megatron/main/CInstance.h \
    ../../megatron/main/CUtils.h \
    ../../megatron/main/configObj.h \
    ../../megatron/main/megatron.h \
    ../../megatron/main/megatronClient.h \
    ../../megatron/main/progressor.h \
    ../../megatron/main/threadNameCtl.h \
    ../../megatron/main/utils_local.h \
    ../../megatron/modules/http/httpService.h \
    ../../megatron/modules/objectProxy/objectProxyService.h \
    ../../megatron/modules/oscar/oscarService.h \
    ../../megatron/modules/oscarSecure/bufferVerify.h \
    ../../megatron/modules/oscarSecure/oscarSecureService.h \
    ../../megatron/modules/oscarSecure/oscarSecureUser.h \
    ../../megatron/modules/rpc/rpcService.h \
    ../../megatron/modules/socket/socketService.h \
    ../../megatron/modules/socket/socketStats.h \
    ../../megatron/modules/telnet/telnet.h \
    ../../megatron/modules/telnet/telnetService.h \
    ../../megatron/modules/telnet/telnet_keys.h \
    ../../megatron/modules/timer/timerService.h \
    ../../megatron/modules/webHandler/webHandlerService.h

DISTFILES += \
    ../../megatron/corelib/CMakeLists.txt
