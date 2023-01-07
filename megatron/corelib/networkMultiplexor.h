#ifndef _______________NETWORKMULTIPLEXOR_H
#define _______________NETWORKMULTIPLEXOR_H

#include "e_poll.h"
#include "unistd.h"
#include <vector>
#ifdef __FreeBSD__
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#endif
#ifdef __linux__
#include <sys/epoll.h>
#endif

#ifdef __MACH__
#include <sys/select.h>
#include <sys/event.h>
#endif
#include "REF.h"
class epoll_socket_info;

struct NetworkMultiplexor: public Refcountable, public Mutexable
{
#ifdef HAVE_EPOLL
    e_poll m_epoll;
#endif
private:
#ifdef HAVE_KQUEUE
    int m_kqueue;
//    std::vector<struct kevent> evSet;
//    struct kevent evSt[1024];
//    int evIdx;

public:
    NetworkMultiplexor()
    {
        m_kqueue=kqueue();
        if(m_kqueue==-1)
            throw CommonError("kqueue(): errno %d",errno);
    }

#endif
public:
#ifdef HAVE_KQUEUE
//    void clear();
//    void addEvent(const struct kevent& k);
//    size_t size();
//    std::vector<struct kevent> extractEvents();
    int getKqueue();
#endif

    ~NetworkMultiplexor();

    void sockAddReadOnNew(epoll_socket_info *esi);
    void sockStartWrite(epoll_socket_info* esi);
    void sockStopWrite(epoll_socket_info* esi);
    void sockAddRWOnNew(epoll_socket_info* esi);

//    int add_counter=0;

};


#endif // NETWORKMULTIPLEXOR_H
