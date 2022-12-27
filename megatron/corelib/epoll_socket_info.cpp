#ifdef _WIN32
#include <Winsock2.h>
#endif
#include "epoll_socket_info.h"
#include "IInstance.h"
#include "Events/System/Net/socket/Write.h"
#include "Events/System/Net/socket/Close.h"
#include "tools_mt.h"
#ifdef __linux__
#include <sys/epoll.h>
#endif
#ifdef __APPLE__

#endif
#include "json/json.h"
#ifdef _WIN32
#include "compat_win32.h"
#include <winsock2.h>
#endif
#include "mutexInspector.h"
#include "colorOutput.h"
Json::Value epoll_socket_info::__jdump()
{
    Json::Value v;
    XTRY;

    v["id"]=(int)CONTAINER(m_id);
//    v["fd"]=CONTAINER(m_fd);
    switch(m_streamType)
    {
    case STREAMTYPE_ACCEPTED:
        v["streamType"]="ACCEPTED";
        break;
    case STREAMTYPE_CONNECTED:
        v["streamType"]="CONNECTED";
        break;
    case STREAMTYPE_LISTENING:
        v["streamType"]="LISTENING";
        break;
    default:
        throw CommonError("invalid streamtype %s %d",__FILE__,__LINE__);
    }
    {
//        v["outBufferSize"]=(int)m_outBuffer.size();
        {
            M_LOCK(m_inBuffer);
            v["inBufferSize"]=(int)m_inBuffer._mx_data.size();
        }
    }
    v["in connection"]=inConnection;
    if(remote_name.valid())
        v["peer name"]=remote_name->addr.dump();
    if(local_name.valid())
    v["local name"]=local_name->addr.dump();
    v["dst route"]=m_route.dump();
    v["closed"]=closed();
//    v["outbuffer size"]=(int)m_outBuffer.size();
    v["inbuffer size"]=(int)m_inBuffer.size();
    v["socketDescription"]=socketDescription;
    XPASS;
    return v;

}

Json::Value epoll_socket_info::wdump()
{
    return __jdump();
}

void epoll_socket_info::write_(const std::string&s)
{
    iInstance->sendEvent(ServiceEnum::Socket, new socketEvent::Write(this,s));

}
void epoll_socket_info::close(const std::string & reason)
{
    MUTEX_INSPECTOR;
    iInstance->sendEvent(ServiceEnum::Socket,new socketEvent::Close(this,route_t()));
}

epoll_socket_info::~epoll_socket_info()
{
}

bool epoll_socket_info::closed()
{
    return _closed;
}

epoll_socket_info::epoll_socket_info(IInstance* ins, const int &_socketType, const STREAMTYPE &_streamtype, const SOCKET_id& _id,  uv_tcp_t *_fd,
                                     const route_t& _route, const char *_socketDescription, bool (*__bufferVerify)(const std::string&)):
    iInstance(ins),
    m_socketType(_socketType),
    m_streamType(_streamtype),
    m_id(_id),
    m_stream(_fd),
    m_route(_route),
    markedToDestroyOnSend(false),
    inConnection(false),
    remote_name(NULL),
    local_name(NULL),
    request_for_connect(NULL),
    socketDescription(_socketDescription),
    bufferVerify(__bufferVerify),
    _closed(false)
{
}

void epoll_socket_info::_inBuffer::append(const char* data, size_t size)
{
    M_LOCK(this);
    _mx_data.append(data,size);
}
size_t epoll_socket_info::_inBuffer::size()
{
    MUTEX_INSPECTOR;
    M_LOCK(this);
    return _mx_data.size();
}
std::string epoll_socket_info::_inBuffer::extract_all()
{
    MUTEX_INSPECTOR;
    M_LOCK(this);
    std::string ret=std::move(_mx_data);
    _mx_data.clear();
    return ret;
}
