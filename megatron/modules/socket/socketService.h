#ifndef ________TCP_LISTENER_____H
#define ________TCP_LISTENER_____H


#include <REF.h>
#include <SOCKET_id.h>
#include <epoll_socket_info.h>
#include <json/value.h>
#include <unknown.h>
#include <listenerSimple.h>
#include <broadcaster.h>
#include "event.h"
#include <Events/System/Net/socket/AddToListenTCP.h>
#include <Events/System/Net/socket/AddToConnectTCP.h>
#include <Events/System/Run/startService.h>
#include <Events/System/timer/TickTimer.h>
#include <Events/Tools/webHandler/RequestIncoming.h>
#include "Events/System/Net/socket/Write.h"

namespace SocketIO
{
struct SocketsContainerForSocketIO
    :public Refcountable,
     public Mutexable
{

private:
    std::map<SOCKET_id,REF_getter<epoll_socket_info> > m_container;
public:
    REF_getter<NetworkMultiplexor> multiplexor;

    SocketsContainerForSocketIO(): multiplexor(new NetworkMultiplexor)
    {}
    ~SocketsContainerForSocketIO();
    void remove(const SOCKET_id& sid)
    {
        M_LOCK(this);
        m_container.erase(sid);

    }
    std::map<SOCKET_id,REF_getter<epoll_socket_info> > getContainer()
    {
        M_LOCK(this);
        return m_container;
    }
    size_t count(const SOCKET_id& sid)
    {
        M_LOCK(this);
        return m_container.count(sid);
    }
    REF_getter<epoll_socket_info> getOrNull(const SOCKET_id& sid)
    {
        M_LOCK(this);
        auto z=m_container.find(sid);
        if(z==m_container.end())
            return NULL;
        return z->second;
    }
    Json::Value jdump();

    void add(const REF_getter<epoll_socket_info>& esi)
    {
//        multiplexor->sockAddReadOnNew(esi.operator ->());

        {
            M_LOCK(this);
            m_container.insert(std::make_pair(esi->m_id,esi));
        }

    }

    void clear()
    {
        multiplexor=NULL;
    }

private:
};

class Service:
    public UnknownBase,
    public Broadcaster,
    public ListenerSimple

{
private:
    Json::Value jdump()
    {
        Json::Value v;
        v["total_recv"]=std::to_string(m_total_recv);
        v["total_send"]=std::to_string(m_total_send);
        v["total_accepted"]=std::to_string(m_total_accepted);
        v["listen_backlog"]=(int)m_listen_backlog;
//        v["sockets"]=m_socks->jdump();
        v["maxOutBufferSize"]=std::to_string(maxOutBufferSize);
        v["epoll_size"]=std::to_string(epoll_size);
        v["epoll_timeout_millisec"]=std::to_string(epoll_timeout_millisec);
//        v["sockets"]=m_socks->jdump();

        return v;
    }
    int64_t m_total_recv, m_total_send,m_total_accepted;

    struct PTHSOCKS
    {
        RWLock lock;
        std::map<pthread_t, REF_getter<SocketsContainerForSocketIO> > m_socks_pollers;
    };
    PTHSOCKS m_io_socks_pollers;
#ifdef WITH_TCP_STATS
    __stats m_stats;
#endif
#define N_WORKERS 4
    std::vector<pthread_t> m_pthread_id_worker;
//    pthread_t m_pthread_id_acceptor;
    int64_t m_listen_backlog;
    const int64_t maxOutBufferSize;
#ifdef HAVE_EPOLL
    int epoll_size;
    int epoll_timeout_millisec;
#endif
#ifdef HAVE_KQUEUE
    /// конфигурационный параметр
    int epoll_size;
    int epoll_timeout_millisec;

//    struct kev_list
//    {
//        struct kevent kev[1024];
//        std::atomic<int> kevidx;
//        kev_list()
//        {
//            kevidx.store(0);
//        }
//    };
//    kev_list klist0;
//    kev_list klist1;
//    kev_list *default_kev;

#endif

    std::deque<std::pair<SOCKET_fd,REF_getter<epoll_socket_info> > > accepteds_MX_container;
    Mutex accepted_MX_lock;



    void closeSocket(const REF_getter<epoll_socket_info>&esi, const std::string& reason, int errNo, const REF_getter<SocketsContainerForSocketIO> &MS);
    bool on_AddToListenTCP(const socketEvent::AddToListenTCP*, const REF_getter<SocketsContainerForSocketIO> &MS);
    bool on_AddToConnectTCP(const socketEvent::AddToConnectTCP*, const REF_getter<SocketsContainerForSocketIO> &MS);
    bool on_startService(const systemEvent::startService*);
    bool on_TickTimer(const timerEvent::TickTimer*);
//    bool on_Write(const socketEvent::Write*);
    bool on_RequestIncoming(const webHandlerEvent::RequestIncoming*);
    void onEPOLLIN(const REF_getter<epoll_socket_info>& esi, const REF_getter<SocketsContainerForSocketIO> &MS);
    void onEPOLLIN_STREAMTYPE_LISTENING(const REF_getter<epoll_socket_info>&esi, const REF_getter<SocketsContainerForSocketIO> &MS);
    void onEPOLLIN_STREAMTYPE_CONNECTED_or_STREAMTYPE_ACCEPTED(const REF_getter<epoll_socket_info>&esi, const REF_getter<SocketsContainerForSocketIO> &MS);



    void onEPOLLOUT(const REF_getter<epoll_socket_info>& esi, const REF_getter<SocketsContainerForSocketIO> &MS);
    void onEPOLLERR(const REF_getter<epoll_socket_info>& esi, const REF_getter<SocketsContainerForSocketIO> &MS);
    bool  handleEvent(const REF_getter<Event::Base>&);
    void handle_accepted(const REF_getter<SocketsContainerForSocketIO>& MS);

    IInstance* iInstance;
    bool m_isTerminating;

public:
    static UnknownBase *construct(const SERVICE_id& id, const std::string&  nm, IInstance* ifa);
    Service(const SERVICE_id& id, const std::string& nm,  IInstance *ifa);
    ~Service();
    void deinit()
    {
        ListenerSimple::deinit();
    }

protected:
    static void *worker__(void*);
    void worker();



};


};

#endif
