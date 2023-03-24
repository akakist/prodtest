#ifndef ____SEVICE_ERRDTIMERR___H___METR
#define ____SEVICE_ERRDTIMERR___H___METR
#include "mutexable.h"
#include "unknown.h"
#include "SERVICE_id.h"
#ifndef _MSC_VER
#include <sys/time.h>
#endif
#include "IInstance.h"
#include "listenerBuffered1Thread.h"
#include "listenerSimple.h"
#include "broadcaster.h"
#include <ostream>

#include "logging.h"

#include "Events/System/Run/startService.h"
#include "Events/System/Net/rpc/IncomingOnAcceptor.h"
#include "../Event/MetricsHTTPProvider/AddProvider.h"
#include "CompositeMetricsProvider.hpp"
#include "Events/System/Net/http/RequestIncoming.h"

namespace MetricsHTTPProvider
{

    class Service:
        public UnknownBase,
        public ListenerBuffered1Thread,
        public Broadcaster

    {
        bool on_startService(const systemEvent::startService*);
        bool handleEvent(const REF_getter<Event::Base>& e);
        bool on_IncomingOnAcceptor(const rpcEvent::IncomingOnAcceptor*);

        bool AddProvider(MetricsProviderEvent::AddProvider*);
        bool RequestIncoming(httpEvent::RequestIncoming*e);


        Service(const SERVICE_id&, const std::string&  nm, IInstance *ifa);
        ~Service();


        REF_getter<CompositeMetricsProvider> providers_;
        IInstance *iInstance;
        msockaddr_in bindAddr;
    public:
        void deinit()
        {
            ListenerBuffered1Thread::deinit();
        }
        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
        {
            XTRY;
            return new Service(id,nm,ifa);
            XPASS;
        }

    private:

    };
};
#endif
