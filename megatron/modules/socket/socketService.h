#ifndef ________TCP_LISTENER_____H
#define ________TCP_LISTENER_____H


#include <REF.h>
#include <SOCKET_id.h>
#include <epoll_socket_info.h>
#include <json/value.h>
#include <unknown.h>
#include <listenerSimple.h>
#include <broadcaster.h>
#include "event_mt.h"
#include <Events/System/Net/socket/AddToListenTCP.h>
#include <Events/System/Net/socket/AddToConnectTCP.h>
#include <Events/System/Run/startService.h>
#include <Events/System/timer/TickTimer.h>
#include <Events/Tools/webHandler/RequestIncoming.h>
#include "Events/System/Net/socket/UdpAssoc.h"
#include "Events/System/Net/socket/Write.h"
#include "Events/System/Net/socket/Close.h"
#include <uv.h>
namespace SocketIO
{

    class Service:
        public UnknownBase,
        public Broadcaster,
        public ListenerSimple

    {
    private:
        Json::Value jdump()
        {
            Json::Value v;

            return v;
        }
//        int64_t m_total_recv, m_total_send,m_total_accepted;

#ifdef WITH_TCP_STATS
        __stats m_stats;
#endif
        pthread_t m_pthread_id_worker;

        bool  on_forward_to_thread(const REF_getter<Event::Base>& ev);


        bool on_startService(const systemEvent::startService*);
        bool on_TickTimer(const timerEvent::TickTimer*);
        bool on_RequestIncoming(const webHandlerEvent::RequestIncoming*);



        bool  handleEvent(const REF_getter<Event::Base>&);

        IInstance* iInstance;
    public:
        bool m_isTerminating;

    public:
        static UnknownBase *construct(const SERVICE_id& id, const std::string&  nm, IInstance* ifa);
        Service(const SERVICE_id& id, const std::string& nm,  IInstance *ifa);
        ~Service();
        void deinit()
        {
//            ListenerSimple::deinit();
        }

    protected:
        static void *worker__(void*);
        void worker();

        struct _MX {
            std::deque<REF_getter<Event::Base > > workerEvents;
            std::atomic<int> dequeSize;
            RWLock lock;
            _MX() {
                dequeSize=0;
            }
        };
        _MX mx;
    public:
        void WorkerHandleEvents();
        void worker_AddToListenTCP(const socketEvent::AddToListenTCP*e);
        void worker_AddToConnectTCP(const socketEvent::AddToConnectTCP*e);
        void worker_Write(const socketEvent::Write*e);
        void worker_Close(const socketEvent::Close*e);

        static void after_read(uv_stream_t* handle,
                                      ssize_t nread,
                                      const uv_buf_t* buf);

        static void worker_alloc_cb(uv_handle_t* handle,
                                    size_t suggested_size,
                                    uv_buf_t* buf);
        static void after_listen(uv_stream_t* server, int status);
        static void after_write(uv_write_t* req, int status);
        static void after_close(uv_handle_t* handle);
        static void after_shutdown(uv_shutdown_t* req, int status);
        static void after_connect(uv_connect_t *req, int status);





    };


};

#endif
