#ifndef ____EVT_SOCKET_OPEN__H___WR
#define ____EVT_SOCKET_OPEN__H___WR

#include "_________socketEvent.h"
#include "event_mt.h"
namespace socketEvent
{
    class Write: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Write(const REF_getter<epoll_socket_info> & _esi,const std::string& _buf)
            :NoPacked(socketEventEnum::Write),
             esi(_esi),buf(_buf)
        {
        }
        void jdump(Json::Value &) const
        {
        }
        const REF_getter<epoll_socket_info> esi;
        const std::string buf;
    };
}


#endif
