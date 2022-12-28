#ifndef __________EPOLL_SOCKET_INFO_____H
#define __________EPOLL_SOCKET_INFO_____H

#include "networkMultiplexor.h"
//#include "epoll_socket_info.h"
#include <deque>
#include "REF.h"
#include "SOCKET_fd.h"
#include "webDumpable.h"
#include "SOCKET_id.h"
#include "route_t.h"
#include "uv.h"
#include "IInstance.h"
#include "st_rsa.h"

/**
* Wrapper for socket selector
*/

namespace SocketIO {
class Service;

}
//struct m_uv_tcp_t{
//    uv_tcp_t tcp;
//    SocketIO::Service *service;
//    REF_getter<epoll_socket_info> esi;
//    m_uv_tcp_t(SocketIO::Service* s):
//        service(s),
//        esi(NULL)
//    {

//    }
//};
//struct m_uv_write_t {
//  uv_write_t req;
//  uv_buf_t buf;
////  SocketIO::Service *service;
////  REF_getter<epoll_socket_info> esi;
//  m_uv_write_t(){
//  }
//};

struct m_opaque
{
    SocketIO::Service *service;
    REF_getter<epoll_socket_info> esi;
    m_opaque(SocketIO::Service *s, REF_getter<epoll_socket_info> &e)
        :service(s),esi(e)
    {}

};

struct P_msockaddr_in: public Refcountable
{
    msockaddr_in addr;
    P_msockaddr_in(const msockaddr_in& sa)
        : addr(sa)
    {

    }
};
class epoll_socket_info;
/**
* Socket wrapper
*/
//struct NetworkMultiplexor;

class epoll_socket_info:public Refcountable, public WebDumpable
{

public:
    IInstance* iInstance;
    const int m_socketType; /// SOCK_STREAM, SOCK_DGRAM
    enum STREAMTYPE
    {
        STREAMTYPE_ACCEPTED,
        STREAMTYPE_CONNECTED,
        STREAMTYPE_LISTENING,
        STREAMTYPE_UDP_ASSOC
    };

    /// socket type   _ACCEPTED,  _CONNECTED,  _LISTENING,
    const STREAMTYPE m_streamType;

    /// unique ID
    SOCKET_id m_id;

    uv_tcp_t* m_stream;
private:
    /// filedescriptor

public:

    bool closed();

    /// routing
    const route_t m_route;

    /// if true socket must be closed after flush data
    bool markedToDestroyOnSend;



    /// out buffer
//    socketBuffersOut m_outBuffer;

    /// in buffer
    struct _inBuffer: public Mutexable
    {
        std::string _mx_data;
        void append(const char* data, size_t size);
        size_t size();
        std::string extract_all();
    };
    _inBuffer m_inBuffer;

    /// socket in ::connect state
    bool inConnection;

    /// remote peer name
    REF_getter<P_msockaddr_in> remote_name;

    /// local peer name
    REF_getter<P_msockaddr_in> local_name;

    /// local peer name
    REF_getter<P_msockaddr_in> request_for_connect;

    /// m_outBuffer max size
//    const unsigned int maxOutBufferSize;

    /// any text of socket for debugging
    const char* socketDescription;

    ///  used to check buffer available for processing
    bool (*bufferVerify)(const std::string& s);

//    REF_getter<NetworkMultiplexor> multiplexor;


    epoll_socket_info(IInstance*ins, const int& socketType, const STREAMTYPE &_streamtype,const SOCKET_id& _id, uv_tcp_t* _fd, const route_t& _route,
                      const char* _socketDescription,         bool (*__bufferVerify)(const std::string&)
                      );

    virtual ~epoll_socket_info();

    /// implementation of webDumpable (display in http)
    Json::Value wdump();
    std::string wname() {
        return "epoll_socket_info";
    }
    /// write buffer
#ifdef KALL
    void write_(const char *s, const size_t &sz);
#endif
    void write_(const std::string&s);
    void close(const std::string & reason);

    Json::Value __jdump();
    bool _closed;
    st_rsa rsa;
    st_AES aes;

};


#endif
