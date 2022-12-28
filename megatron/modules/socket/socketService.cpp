#ifndef _WIN32
#include <sys/ioctl.h>
#ifdef __linux__
#include <sys/epoll.h>
#endif
#include <sys/socket.h>
#include <arpa/inet.h>
#else
#include "compat_win32.h"
#endif
#include <sys/types.h>
#include "socketService.h"
#include <unistd.h>

#include <unistd.h>
#ifdef __MACH__
#include <sys/select.h>
#include <sys/event.h>
#endif
#include "st_malloc.h"
#ifndef _WIN32
#include <sys/ioctl.h>
#endif
#include <colorOutput.h>
#include <logging.h>
#include <tools_mt.h>
#include <Events/System/Net/socket/Accepted.h>
#include <Events/System/Net/socket/StreamRead.h>
#include <Events/System/Net/socket/Connected.h>
#include <Events/System/Net/socket/NotifyOutBufferEmpty.h>

//#endif
#include "version_mega.h"
#include "Events/Tools/webHandler/RegisterHandler.h"
#include "Events/System/Net/socket/Connected.h"
#include "Events/System/Net/socket/NotifyBindAddress.h"
#include "Events/System/Net/socket/ConnectFailed.h"
#include "Events/System/Net/socket/Disaccepted.h"
#include "Events/System/Net/socket/Disconnected.h"
#include "events_socket.hpp"
#include "colorOutput.h"
#include <uv.h>
#ifdef HAVE_EPOLL
#include <sys/epoll.h>
#endif
#include <assert.h>

SocketIO::Service::~Service()
{
    m_isTerminating=true;
    int err=pthread_join(m_pthread_id_worker,NULL);
    if(err)
    {
        printf(RED("%s pthread_join: %s"),__PRETTY_FUNCTION__,strerror(errno));
    }


}

SocketIO::Service::Service(const SERVICE_id& id, const std::string& nm, IInstance* ifa):
    UnknownBase(nm),Broadcaster(ifa),
    ListenerSimple(nm,ifa->getConfig(),id),
    iInstance(ifa),m_isTerminating(false)
{
    try {

        if(pthread_create(&m_pthread_id_worker,NULL,worker__,this))
        {
            throw CommonError("pthread_create: errno %d",errno);
        }
    }
    catch(...)
    {
        logErr2("failed init service socket");
    }

}

void *SocketIO::Service::worker__(void*p)
{
    auto* obj=(SocketIO::Service*)p;
    obj->worker();
    return nullptr;
}

bool  SocketIO::Service::on_startService(const systemEvent::startService*)
{
    XTRY;



    XPASS;
    return true;
}
bool SocketIO::Service::on_TickTimer(const timerEvent::TickTimer*e)
{
    return true;
}


UnknownBase* SocketIO::Service::construct(const SERVICE_id& id, const std::string&  nm, IInstance* ifa)
{
    XTRY;
    return new Service(id,nm,ifa);
    XPASS;
}





bool  SocketIO::Service::handleEvent(const REF_getter<Event::Base>&e)
{
    MUTEX_INSPECTOR;
    XTRY;
    auto &ID=e->id;
    if(timerEventEnum::TickTimer==ID)
        return on_TickTimer((const timerEvent::TickTimer*)e.operator->());
    if(socketEventEnum::AddToListenTCP==ID)
        return on_forward_to_thread(e);
    if(socketEventEnum::AddToConnectTCP==ID)
        return on_forward_to_thread(e);
    if(socketEventEnum::Close==ID)
        return on_forward_to_thread(e);
    if(socketEventEnum::Write==ID)
        return on_forward_to_thread(e);
    if(webHandlerEventEnum::RequestIncoming==ID)
        return on_RequestIncoming((const webHandlerEvent::RequestIncoming*)e.operator->());
    if(systemEventEnum::startService==ID)
        return on_startService((const systemEvent::startService*)e.operator->());

    XPASS;
    return false;
}

void registerSocketModule(const char* pn)
{
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::Socket,"SocketIO",getEvents_socket());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::Socket,SocketIO::Service::construct,"SocketIO");
        regEvents_socket();
    }


}

bool  SocketIO::Service::on_forward_to_thread(const REF_getter<Event::Base> &ev)
{
    MUTEX_INSPECTOR;
    {
        WLocker lk(mx.lock);
        mx.workerEvents.push_back(ev);
        mx.dequeSize++;
    }
    return true;
}


bool SocketIO::Service::on_RequestIncoming(const webHandlerEvent::RequestIncoming* e)
{
    MUTEX_INSPECTOR;

    HTTP::Response cc(iInstance);
    cc.content+="<h1>Socket report</h1><p>";

    Json::Value v=jdump();
    Json::StyledWriter w;
    cc.content+="<pre>\n"+w.write(v)+"\n</pre>";

    cc.makeResponse(e->esi);
    return true;
}




void SocketIO::Service::after_close(uv_handle_t* handle)
{
    MUTEX_INSPECTOR;

//    printf("after_close\n");
    m_opaque* op=(m_opaque*)handle->data;
    op->esi->_closed=true;
    if(op->esi->STREAMTYPE_ACCEPTED)
        op->service->passEvent(new socketEvent::Disaccepted(op->esi,"uv close",poppedFrontRoute(op->esi->m_route)));
    if(op->esi->STREAMTYPE_CONNECTED)
        op->service->passEvent(new socketEvent::Disconnected(op->esi,"uv close",poppedFrontRoute(op->esi->m_route)));
    delete(op);
}

void SocketIO::Service::after_shutdown(uv_shutdown_t* req, int status)
{
    MUTEX_INSPECTOR;


    uv_close((uv_handle_t*)req->handle, after_close);
    delete (req);
}
void SocketIO::Service::after_write(uv_write_t* req1, int status)
{
    MUTEX_INSPECTOR;
    uv_buf_t* buft=(uv_buf_t*)req1->data;

    auto handle=req1->handle;

    if(buft!=NULL)
    {
        if (buft->base != NULL)
            free(buft->base);
        delete buft;
    }

    delete req1;

    m_opaque *op=(m_opaque *)handle->data;
    if(op->esi->markedToDestroyOnSend)
    {
        uv_shutdown_t* req = new uv_shutdown_t;
        assert(req != NULL);

        int r = uv_shutdown(req, (uv_stream_t*) op->esi->m_stream, after_shutdown);
        if(r)
            printf("err: %s\n",uv_strerror(r));

        assert(r == 0);

//        uv_close((uv_handle_t*)handle, after_close);
        return;

    }
    if (status == 0)
    {
        return;
    }

    fprintf(stderr, "uv_write error: %s\n", uv_strerror(status));

    if (status == UV_ECANCELED)
    {
        return;
    }

    assert(status == UV_EPIPE);
    uv_close((uv_handle_t*)handle, after_close);
}


void SocketIO::Service::after_read(uv_stream_t* handle,
        ssize_t nread,
        const uv_buf_t* buf)
{
    MUTEX_INSPECTOR;
    int r;

    if (nread <= 0 && buf->base != NULL)
        free(buf->base);

    if (nread == 0)
        return;

    if (nread < 0) {
//        fprintf(stderr, "err: %s\n", uv_strerror(nread));


        uv_shutdown_t* req = new uv_shutdown_t;
        assert(req != NULL);
        r = uv_shutdown(req, handle, after_shutdown);
        if(r)
            printf("err: %s\n",uv_strerror(r));

        assert(r == 0);

        return;
    }
    m_opaque* op=(m_opaque*)handle->data;


    op->esi->m_inBuffer.append(buf->base, nread);
    std::string recv(buf->base, nread);
    op->service->passEvent(new socketEvent::StreamRead(op->esi,poppedFrontRoute(op->esi->m_route)));
    free(buf->base);
}

void SocketIO::Service::worker_alloc_cb(uv_handle_t* handle,
                                        size_t suggested_size,
                                        uv_buf_t* buf) {
    MUTEX_INSPECTOR;
    buf->base = (char*)malloc(suggested_size);
    assert(buf->base != NULL);
    buf->len = suggested_size;
}


void SocketIO::Service::after_listen(uv_stream_t* server, int status)
{

    MUTEX_INSPECTOR;
    int r;
    m_opaque* server_op=(m_opaque* )server->data;
    assert(status == 0);

    uv_tcp_t* stream = new uv_tcp_t();

    assert(stream != NULL);

    r = uv_tcp_init(uv_default_loop(), stream);
    if(r)
        printf("err: %s\n",uv_strerror(r));
    assert(r == 0);


    r = uv_accept(server, (uv_stream_t*)stream);
    if(r)
        printf("err: %s\n",uv_strerror(r));
    assert(r == 0);
    SOCKET_id _sid=iUtils->getSocketId();
    REF_getter<epoll_socket_info> nesi=new epoll_socket_info(server_op->service->iInstance,
            SOCK_STREAM,
            epoll_socket_info::STREAMTYPE_ACCEPTED,
            _sid,stream,server_op->esi->m_route,
            server_op->esi->socketDescription,server_op->esi->bufferVerify);
    {
        msockaddr_in sa;
        int len=sizeof(sa);
        uv_tcp_getsockname(stream,(sockaddr*)&sa, &len);
        nesi->local_name=new P_msockaddr_in(sa);
    }
    {
        msockaddr_in sa;
        int len=sizeof(sa);
        uv_tcp_getpeername(stream,(sockaddr*)&sa, &len);
        nesi->remote_name=new P_msockaddr_in(sa);
    }
    stream->data=new m_opaque(server_op->service,nesi);

    server_op->service->passEvent(new socketEvent::Accepted(nesi,poppedFrontRoute(server_op->esi->m_route)));

    r = uv_read_start((uv_stream_t*)stream, worker_alloc_cb, after_read);
    if(r)
        printf("err: %s\n",uv_strerror(r));
    assert(r == 0);
}
void SocketIO::Service::after_connect(uv_connect_t *req1, int status)
{
    MUTEX_INSPECTOR;
    auto handle=req1->handle;
    m_opaque *op=(m_opaque *)handle->data;
    delete  req1;
    if(status==0)
    {
        {
            msockaddr_in sa;
            int len=sizeof(sa);
            uv_tcp_getsockname((uv_tcp_t*)handle,(sockaddr*)&sa, &len);
            op->esi->local_name=new P_msockaddr_in(sa);
        }
        {
            msockaddr_in sa;
            int len=sizeof(sa);
            uv_tcp_getpeername((uv_tcp_t*)handle,(sockaddr*)&sa, &len);
            op->esi->remote_name=new P_msockaddr_in(sa);
        }
        op->service->passEvent(new socketEvent::Connected(op->esi,poppedFrontRoute(op->esi->m_route)));
    }
    else
    {
        op->service->passEvent(new socketEvent::ConnectFailed(op->esi,
                                                                   op->esi->request_for_connect->addr,
                                                                   status,
                                                                   poppedFrontRoute(op->esi->m_route)));
    }
      if (status == -1) {
        fprintf(stderr, "error on_write_end");
        return;
      }

      int r = uv_read_start((uv_stream_t*)handle, worker_alloc_cb, after_read);
      if(r)
          printf("err: %s\n",uv_strerror(r));
      assert(r == 0);

}

void SocketIO::Service::worker_AddToListenTCP(const socketEvent::AddToListenTCP*ev)
{
    MUTEX_INSPECTOR;
    uv_tcp_t* tcp_server;
    int r;


    tcp_server = new uv_tcp_t();
    assert(tcp_server != NULL);

    r = uv_tcp_init(uv_default_loop(), tcp_server);
    if(r)
        printf("err: %s\n",uv_strerror(r));
    assert(r == 0);

    r = uv_tcp_bind(tcp_server, (const struct sockaddr*)&ev->addr, 0);
    if(r)
    {
        printf("err: %s\n",uv_strerror(r));
        return;
    }
    assert(r == 0);

    msockaddr_in sa;
    int len=sizeof(sa);
    uv_tcp_getsockname(tcp_server,(sockaddr*)&sa, &len);
    logErr2("binded to %s (%s)",sa.dump().c_str(),ev->route.dump().c_str());


    REF_getter<epoll_socket_info> nesi=new epoll_socket_info(iInstance,SOCK_STREAM,epoll_socket_info::STREAMTYPE_LISTENING,ev->socketId,tcp_server,ev->route,
            ev->socketDescription,ev->bufferVerify);
    nesi->local_name=new P_msockaddr_in(sa);
    tcp_server->data=new m_opaque(this,nesi);

    passEvent(new socketEvent::NotifyBindAddress(sa,ev->socketDescription,ev->rebind,poppedFrontRoute(ev->route)));
//    {
//        M_LOCK(mx.lock);
//        mx.esies.emplace(nesi->m_stream,nesi);
//    }

    r = uv_listen((uv_stream_t*)tcp_server, SOMAXCONN, after_listen);
    if(r)
        printf("err: %s\n",uv_strerror(r));

    assert(r == 0);

}
void SocketIO::Service::worker_AddToConnectTCP(const socketEvent::AddToConnectTCP*e)
{
    MUTEX_INSPECTOR;
        uv_connect_t *connect_req=new uv_connect_t;
        uv_tcp_t *client=new uv_tcp_t();
//        SOCKET_id sid(e->socketId);
        REF_getter<epoll_socket_info> nesi=new epoll_socket_info(iInstance,SOCK_STREAM,epoll_socket_info::STREAMTYPE_CONNECTED,e->socketId,client,e->route,e->socketDescription,e->bufferVerify);
        nesi->request_for_connect=new P_msockaddr_in(e->addr);
        m_opaque *client_op=new m_opaque(this,nesi);

        uv_tcp_init(uv_default_loop(), client);
        client->data=client_op;
        uv_tcp_connect(connect_req, client, (const sockaddr*)&e->addr, after_connect);
}
void SocketIO::Service::worker_Write(const socketEvent::Write*e)
{
    MUTEX_INSPECTOR;
    uv_write_t* wr = new uv_write_t;
    assert(wr != NULL);

    char *buf=(char*)malloc(e->buf.size());
    memcpy(buf,e->buf.data(),e->buf.size());
    uv_buf_t* buft=new uv_buf_t;

    *buft = uv_buf_init((char*)buf, e->buf.size());
    wr->data=buft;
//    logErr2("uv_write size %d",buft->len);
    int r = uv_write(wr, (uv_stream_t*)e->esi->m_stream, buft, 1, after_write);
    if(r)
        printf("err: %s\n",uv_strerror(r));

    assert(r == 0);

}
void SocketIO::Service::worker_Close(const socketEvent::Close*e)
{

    MUTEX_INSPECTOR;

    if(!e->esi->closed())
    {
        e->esi->_closed=true;

        uv_shutdown_t* req = new uv_shutdown_t;
        assert(req != NULL);

        int r = uv_shutdown(req, (uv_stream_t*) e->esi->m_stream, after_shutdown);
        if(r)
            printf("err: %s\n",uv_strerror(r));

        assert(r == 0);

    }
}

void SocketIO::Service::WorkerHandleEvents()
{
    MUTEX_INSPECTOR;
    std::deque<REF_getter<Event::Base> > evs;
    if(mx.dequeSize!=0)
    {
        WLocker lk(mx.lock);
        evs=mx.workerEvents;
        mx.workerEvents.clear();
        mx.dequeSize=0;
    }
    for(auto& e:evs)
    {
        auto &ID=e->id;
        if(socketEventEnum::AddToListenTCP==ID)
            worker_AddToListenTCP((const socketEvent::AddToListenTCP*)e.operator->());
        else if(socketEventEnum::AddToConnectTCP==ID)
            worker_AddToConnectTCP((const socketEvent::AddToConnectTCP*)e.operator->());
        else if(socketEventEnum::Write==ID)
            worker_Write((const socketEvent::Write*)e.operator->());
        else if(socketEventEnum::Close==ID)
            worker_Close((const socketEvent::Close*)e.operator->());
        else
            logErr2("unhandled event %s",e->dump().toStyledString().c_str());

    }
}
void timer_cb1 (uv_timer_t* timer, int status) {
    SocketIO::Service* service=(SocketIO::Service*)timer->data;
    service->WorkerHandleEvents();
    if(service->m_isTerminating)
    {
        uv_timer_stop(timer);
        uv_stop(uv_default_loop());
    }
}
void SocketIO::Service::worker()
{
    MUTEX_INSPECTOR;
#if defined __MACH__
    pthread_setname_np("SocketIO");
#else
#if !defined _WIN32 && !defined __FreeBSD__
    pthread_setname_np(pthread_self(),"SocketIO");
#endif
#endif
    sleep(1);
    {
        uv_timer_t timer1;
        uv_timer_init(uv_default_loop(), &timer1);
        timer1.data=this;
        uv_timer_start(&timer1, (uv_timer_cb) &timer_cb1, 1, 1);
        int r = uv_run(uv_default_loop(), UV_RUN_DEFAULT);
        if(r!=0 && r!=1)
            printf("err: %s\n",uv_strerror(r));
    }
}
