#ifndef ___CloseEvent_H
#define ___CloseEvent_H
#include "_________socketEvent.h"
namespace socketEvent {
    class Close: public Event::NoPacked
    {
    public:
        Close(const REF_getter<epoll_socket_info> &__S, const route_t& r):
            NoPacked(socketEventEnum::Close,r), esi(__S)
        {
        }
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        const REF_getter<epoll_socket_info> esi;
        void jdump(Json::Value &) const
        {
        }
    };
}

#endif
