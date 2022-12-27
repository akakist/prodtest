#ifndef ______E_POLL_H
#define ______E_POLL_H
#include "pconfig.h"
#if defined(WIN32)
#define HAVE_SELECT
#elif defined (__MACH__) || defined(__IOS__) || defined(__FreeBSD__)
#define HAVE_KQUEUE
#else
#define HAVE_EPOLL
#endif

#ifdef HAVE_EPOLL
struct e_poll
{
    e_poll()
        :
        m_epollFd(-1)
    {
    }
    int m_epollFd;

};
#endif


#endif // E_POLL_H
