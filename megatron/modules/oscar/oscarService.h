#ifndef ___________PP__SERVER__H
#define ___________PP__SERVER__H

#include <REF.h>
#include <SOCKET_id.h>
#include <epoll_socket_info.h>
#include <unknown.h>
#include <listenerBuffered.h>
#include <broadcaster.h>
#include "event.h"
#include <Events/System/Net/oscar/Connect.h>
#include <Events/System/Net/oscar/SendPacket.h>
#include <Events/System/Net/oscar/AddToListenTCP.h>
#include <Events/System/Net/socket/Accepted.h>
#include <Events/System/Net/socket/StreamRead.h>
#include <Events/System/Net/socket/Connected.h>
#include <Events/System/Net/socket/NotifyOutBufferEmpty.h>
#include <Events/System/Net/socket/ConnectFailed.h>
#include <Events/System/Net/socket/Disaccepted.h>
#include <Events/System/Net/socket/Disconnected.h>
#include <Events/System/Net/socket/NotifyBindAddress.h>
#include <Events/System/Run/startService.h>

namespace Oscar
{
    enum StartByte
    {
        SB_HELLO		=0x00,
        SB_SINGLEPACKET	=0x01,
        SB_ACK			=0x02,
        SB_MULTIPACKET	=0x03,
    };
    namespace Packet
    {
    }
}

namespace Oscar
{

    class __users: public Refcountable
//            : public SocketsContainerBase
    {
    private:
        Mutex m_lock;
        std::map<SOCKET_id,bool > m_isServers;
        std::map<SOCKET_id,REF_getter<epoll_socket_info> > m_users;
    public:
        __users()
//            : SocketsContainerBase("oscarServiceUsers")
        {}
        REF_getter<epoll_socket_info> getOrNull(const SOCKET_id& socketId)
        {
            M_LOCK(m_lock);
            auto i=m_users.find(socketId);
            if(i==m_users.end())
                return NULL;
            return i->second;
        }
        void add(const REF_getter<epoll_socket_info>& esi)
        {
            M_LOCK(m_lock);
            m_users.insert(std::make_pair(esi->m_id,esi));
        }
        void user_insert(const REF_getter<epoll_socket_info>& bi, const bool& isServer);

        void on_delete(const REF_getter<epoll_socket_info>&esi, const std::string& reason);
        bool isServer(const SOCKET_id&);

        Json::Value jdump();
        void clear()
        {
//            SocketsContainerBase::clear();
            {
                M_LOCK(m_lock);
                m_isServers.clear();
            }
        }
    };

    enum
    {
        TIMER_SEND=1
    };
    class Service:
        public UnknownBase,
        public ListenerBuffered,
        public Broadcaster
    {
        size_t m_maxPacketSize;

        REF_getter<__users> __m_users;

        bool on_Connect(const oscarEvent::Connect* e);
        bool on_SendPacket(const oscarEvent::SendPacket* e);
        bool on_AddToListenTCP(const oscarEvent::AddToListenTCP* e);


        bool on_Accepted(const socketEvent::Accepted*);
        bool on_StreamRead(const socketEvent::StreamRead*);
        bool on_Connected(const socketEvent::Connected*);
        bool on_NotifyBindAddress(const socketEvent::NotifyBindAddress*);
        bool on_NotifyOutBufferEmpty(const socketEvent::NotifyOutBufferEmpty*);
        bool on_ConnectFailed(const socketEvent::ConnectFailed*e);
        bool on_Disaccepted(const socketEvent::Disaccepted*e);
        bool on_Disconnected(const socketEvent::Disconnected*e);


        bool on_startService(const systemEvent::startService*);
        bool handleEvent(const REF_getter<Event::Base>& e);

    protected:
        void sendPacketPlain(const Oscar::StartByte& startByte, const REF_getter<epoll_socket_info>& esi, const outBuffer &o);
        void sendPacketPlain(const Oscar::StartByte& startByte, const REF_getter<epoll_socket_info>& esi, const REF_getter<refbuffer> &o);

        void processRequest(const SOCKET_id&  socketId, const std::string& buf,const route_t & route);
        Json::Value jdump()
        {
            Json::Value v=__m_users->jdump();
            return v;
        }
    public:

    public:
        void deinit()
        {
	    ListenerBuffered::deinit();
        }

        Service(const SERVICE_id &svs, const std::string&  nm,IInstance* ifa);
        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa);
        ~Service();
    };
}
#endif
