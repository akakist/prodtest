#include <version_mega.h>
#include "oscarService.h"

#include "bufferVerify.h"
#include "event.h"
#include <Events/System/Net/socket/AddToConnectTCP.h>
#include <Events/System/Net/socket/AddToListenTCP.h>
#include <Events/System/Net/oscar/Accepted.h>
#include <Events/System/Net/oscar/ConnectFailed.h>
#include <Events/System/Net/oscar/Disaccepted.h>
#include <Events/System/Net/oscar/Disconnected.h>
#include <Events/System/Net/oscar/Connected.h>
#include <Events/System/Net/oscar/NotifyOutBufferEmpty.h>
#include <Events/System/Net/oscar/NotifyBindAddress.h>
#include <mutexInspector.h>
#include "Events/System/Net/oscar/PacketOnConnector.h"
#include "Events/System/Net/oscar/PacketOnAcceptor.h"
#include "Events/Tools/errorDispatcher/SendMessage.h"
#include "events_oscar.hpp"
#include "colorOutput.h"

Oscar::Service::Service(const SERVICE_id &svs, const std::string&  nm,IInstance* ifa):
    UnknownBase(nm),
    ListenerBuffered(nm,ifa->getConfig(),svs,ifa),
    Broadcaster(ifa),
    m_maxPacketSize(32*1024*1024),__m_users(new __users)
{
    try
    {
        XTRY;
        m_maxPacketSize= static_cast<size_t>(ifa->getConfig()->get_int64_t("maxPacketSize",
                                             32 * 1024 * 1024, ""));
        XPASS;
    }
    catch (std::exception &e)
    {
        logErr2("exception: %s %s %d",e.what(),__FILE__,__LINE__);
        throw;
    }

}

Oscar::Service::~Service()
{
    __m_users->clear();
}

bool Oscar::Service::on_Connect(const oscarEvent::Connect* e)
{
    MUTEX_INSPECTOR;
    sendEvent(ServiceEnum::Socket,new socketEvent::AddToConnectTCP(e->socketId,e->addr,e->socketDescription,bufferVerify,e->route));
    return true;
}
bool Oscar::Service::on_SendPacket(const oscarEvent::SendPacket* e)
{
    MUTEX_INSPECTOR;
    REF_getter<epoll_socket_info> esi=__m_users->getOrNull(e->socketId);
    if(esi.valid())
    {
        sendPacketPlain(Oscar::SB_SINGLEPACKET,esi,e->buf);
    }
    else
    {
        throw CommonError("!if(esi.valid()) %s %d",__FILE__,__LINE__);
        logErr2("!esi valid %s %d",__FILE__,__LINE__);
    }
    return true;
}
bool Oscar::Service::on_AddToListenTCP(const oscarEvent::AddToListenTCP* e)
{
    MUTEX_INSPECTOR;
    sendEvent(ServiceEnum::Socket,new socketEvent::AddToListenTCP(e->socketId,e->addr,e->socketDescription,false,bufferVerify, e->route));
    return true;
}
bool Oscar::Service::on_Accepted(const socketEvent::Accepted*e)
{
    MUTEX_INSPECTOR;
    __m_users->user_insert(e->esi,true);

    outBuffer o;
    o<<"HELLO"<<COREVERSION;
    sendPacketPlain(SB_HELLO,e->esi,o);
    passEvent(new oscarEvent::Accepted(e->esi,e->route));
    return true;
}
bool Oscar::Service::on_StreamRead(const socketEvent::StreamRead* evt)
{
    MUTEX_INSPECTOR;
    try
    {
        while (1)
        {
            std::string req;
            bool recvd=false;
            bool success=false;
            unsigned char start_byte;
            bool need_disconnect=false;
            {
                {
                    XTRY;
                    {
                        M_LOCK(evt->esi->m_inBuffer);
                        inBuffer b(evt->esi->m_inBuffer._mx_data.data(), evt->esi->m_inBuffer._mx_data.size());

                        start_byte=b.get_8_nothrow(success);


                        if (!success) return true;

                        {
                            XTRY;
                            auto len= static_cast<size_t>(b.get_PN_nothrow(success));
                            if (!success) return true;
                            if (len>m_maxPacketSize)
                            {
                                need_disconnect=true;
                            }
                            else
                            {
                                XTRY;
                                if(b.remains()<len)
                                {
                                    return true;
                                }
                                else if(b.remains()==len)
                                {
                                    DBG(logErr2("b.remains()==len %d %d",b.remains(),len));
                                }
                                else if(b.remains()>len)
                                {
                                    DBG(logErr2("b.remains()>len %d %d",b.remains(),len));
                                }
                                b.unpack_nothrow(req,len,success);
                                if (!success)
                                {
                                    logErr2("---------_ERROR if (!success) %s %d",__FILE__,__LINE__);
                                    return true;
                                }
                                evt->esi->m_inBuffer._mx_data.erase(0,evt->esi->m_inBuffer._mx_data.size()-b.remains());
                                recvd=true;
                                XPASS;
                            }
                            XPASS;
                        }

                    }
                    XPASS;
                }
            }
            if(need_disconnect)
            {
                XTRY;
                evt->esi->close("oscar buffer broken");
                return true;
                XPASS;
            }
            if(!recvd) return true;
            if (recvd)
            {
                XTRY;
                switch(start_byte)
                {
                case Oscar::SB_SINGLEPACKET:
                {
                    REF_getter<epoll_socket_info> u=__m_users->getOrNull(evt->esi->m_id);
                    if(u.valid())
                    {
                        if(!__m_users->isServer(evt->esi->m_id))
                        {
                            passEvent(new oscarEvent::PacketOnConnector(evt->esi,toRef(req),evt->route));
                        }
                        else
                        {
                            passEvent(new oscarEvent::PacketOnAcceptor(evt->esi,toRef(req),evt->route));
                        }
                    }
                    continue;
                }
                break;
                case Oscar::SB_HELLO:
                {
                    inBuffer in(req);
                    std::string hello=in.get_PSTR();
                    if(hello!="HELLO")
                        throw CommonError("!HELLO");
                    int64_t ver=in.get_PN();
                    if(ver>>8 != COREVERSION>>8)
                    {
                        if(COREVERSION>ver)
                            sendEvent(ServiceEnum::ErrorDispatcher,new errorDispatcherEvent::SendMessage("ED_VERSION_WRONG","Packet version wrong for peer. You running newest version, please wait"));
                        else
                        {
                            sendEvent(ServiceEnum::ErrorDispatcher,new errorDispatcherEvent::SendMessage("ED_VERSION_WRONG","Packet version wrong for peer in oscar. Your need upgrade software"));
                        }

                        evt->esi->close("oscar: invalid peer version");
                        return true;
                    }
                }
                break;
                default:
                    logErr2("Invalid start byte %d",start_byte);
                    evt->esi->close("oscar: invalid start byte");

                    return true;
                }
                XPASS;
            }
            break;
        }
    }
    catch (std::exception &e)
    {
        logErr2("exception: %s (%s %d)",e.what(),__FILE__,__LINE__);
        evt->esi->close(e.what());
    }
    catch (...)
    {
        logErr2("exception: unknown (%s %d)",__FILE__,__LINE__);
        evt->esi->close("exeption ...");
    }
    return true;
}
bool Oscar::Service::on_Connected(const socketEvent::Connected* e)
{
    MUTEX_INSPECTOR;
    __m_users->user_insert(e->esi,false);

    passEvent(new oscarEvent::Connected(e->esi,e->route));
    return true;
}
UnknownBase* Oscar::Service::construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
{
    XTRY;
    return new Service(id,nm,ifa);
    XPASS;
}
bool Oscar::Service::on_NotifyOutBufferEmpty(const socketEvent::NotifyOutBufferEmpty* e)
{
    MUTEX_INSPECTOR;
    passEvent(new oscarEvent::NotifyOutBufferEmpty(e->esi,e->route));
    return true;
}
bool Oscar::Service::on_NotifyBindAddress(const socketEvent::NotifyBindAddress*e)
{
    MUTEX_INSPECTOR;
    passEvent(new oscarEvent::NotifyBindAddress(e->esi,e->socketDescription,e->rebind,e->route));
    return true;
}
void Oscar::Service::sendPacketPlain(const Oscar::StartByte& startByte, const REF_getter<epoll_socket_info>& esi, const outBuffer &o)
{
    MUTEX_INSPECTOR;
    XTRY;
    outBuffer O2;
    O2.put_8(startByte);
    O2<<o.asString();
    esi->write_(O2.asString()->asString());
    XPASS;
}
void Oscar::Service::sendPacketPlain(const Oscar::StartByte& startByte, const REF_getter<epoll_socket_info>& esi, const REF_getter<refbuffer> &o)
{
    MUTEX_INSPECTOR;
    XTRY;
    outBuffer O2;
    O2.put_8(startByte);
    O2<<o;
    esi->write_(O2.asString()->asString());
    XPASS;
}
bool Oscar::Service::on_startService(const systemEvent::startService*)
{
    return true;
}

void registerOscarModule(const char* pn)
{
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::Oscar,"Oscar",getEvents_oscar());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::Oscar,Oscar::Service::construct,"Oscar");
        regEvents_oscar();
    }

}
void Oscar::__users::user_insert(const REF_getter<epoll_socket_info>& esi, const bool &isServer)
{
    MUTEX_INSPECTOR;

    add(esi);
    {
        M_LOCK(m_lock);
        if(m_isServers.count(esi->m_id)) throw CommonError(" 1 if(m_isServers.count(esi->m_id)) "+_DMI());
//        logErr2("m_isServers[esi->m_id]=isServer; %s",_DMI().c_str());
        m_isServers[esi->m_id]=isServer;
    }
}
Json::Value Oscar::__users::jdump()
{
    MUTEX_INSPECTOR;
    Json::Value v;
//    v["SocketsContainerBase"]=SocketsContainerBase::jdump();
    std::map<SOCKET_id,bool > m;
    {
        M_LOCK(m_lock);
        m=m_isServers;
    }
    for(auto i=m.begin(); i!=m.end(); i++)
    {
        v["isServer"][std::to_string(CONTAINER(i->first))]=i->second;
    }
    return v;
}

void Oscar::__users::on_delete(const REF_getter<epoll_socket_info>&esi, const std::string& )
{
    MUTEX_INSPECTOR;
    M_LOCK(m_lock);
    if(!m_isServers.count(esi->m_id)) throw CommonError("2 if(m_isServers.count(esi->m_id)) "+_DMI());
    m_isServers.erase(esi->m_id);
}

bool Oscar::__users::isServer(const SOCKET_id &sid)
{
    MUTEX_INSPECTOR;
    M_LOCK(m_lock);
    if(!m_isServers.count(sid)) throw CommonError("3 if(m_isServers.count(esi->m_id)) %s %d",__FILE__,__LINE__);
    return m_isServers[sid];

}


bool Oscar::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    auto &ID=e->id;
    if( socketEventEnum::Accepted==ID)
        return on_Accepted((const socketEvent::Accepted*)e.operator->());
    if( socketEventEnum::StreamRead==ID)
        return on_StreamRead((const socketEvent::StreamRead*)e.operator->());
    if( socketEventEnum::Connected==ID)
        return on_Connected((const socketEvent::Connected*)e.operator->());
    if( socketEventEnum::NotifyBindAddress==ID)
        return on_NotifyBindAddress((const socketEvent::NotifyBindAddress*)e.operator->());
    if( socketEventEnum::NotifyOutBufferEmpty==ID)
        return on_NotifyOutBufferEmpty((const socketEvent::NotifyOutBufferEmpty*)e.operator->());
    if( socketEventEnum::ConnectFailed==ID)
        return on_ConnectFailed((const socketEvent::ConnectFailed*)e.operator->());
    if( oscarEventEnum::SendPacket==ID)
        return(this->on_SendPacket((const oscarEvent::SendPacket*)e.operator->()));

    if( oscarEventEnum::AddToListenTCP==ID)
        return(this->on_AddToListenTCP((const oscarEvent::AddToListenTCP*)e.operator->()));

    if( oscarEventEnum::Connect==ID)
        return(this->on_Connect((const oscarEvent::Connect*)e.operator->()));

    if(systemEventEnum::startService==ID)
        return on_startService((const systemEvent::startService*)e.operator->());


    if( socketEventEnum::Disaccepted==ID)
        return on_Disaccepted((const socketEvent::Disaccepted*)e.operator->());
    if( socketEventEnum::Disconnected==ID)
        return on_Disconnected((const socketEvent::Disconnected*)e.operator->());

    XPASS;
    return false;

}
bool Oscar::Service::on_ConnectFailed(const socketEvent::ConnectFailed*e)
{
    passEvent(new oscarEvent::ConnectFailed(e->esi,e->addr,e->route));
    return true;
}
bool Oscar::Service::on_Disaccepted(const socketEvent::Disaccepted*e)
{
    passEvent(new oscarEvent::Disaccepted(e->esi,e->reason,e->route));
    return true;
}
bool Oscar::Service::on_Disconnected(const socketEvent::Disconnected*e)
{
    passEvent(new oscarEvent::Disconnected(e->esi,e->reason,e->route));
    return true;
}
