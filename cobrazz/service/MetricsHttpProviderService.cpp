#include "MetricsHttpProviderService.h"
#include <stdarg.h>
#include "unknownCastDef.h"

#include "IRPC.h"
#include "version_mega.h"
#include "VERSION_id.h"
#include "tools_mt.h"
#include "../Event/MetricsHTTPProvider/__MetricsHTTPProvider.h"
#include "../Event/MetricsHTTPProvider/AddProvider.h"
#include "Events/System/Net/http/DoListen.h"
#include "events_metricsProvider.hpp"

MetricsHTTPProvider::Service::Service(const SERVICE_id& id, const std::string& nm, IInstance *ifa):
    UnknownBase(nm),
    ListenerBuffered1Thread(this,nm,ifa->getConfig(),id,ifa),Broadcaster(ifa), providers_(new CompositeMetricsProvider), iInstance(ifa)

{

            auto ba=ifa->getConfig()->get_tcpaddr("bindAddr","0.0.0.0:8088","MetricsHTTPProvider listen address");
            if(ba.size()==0)
                throw CommonError("if(ba.size()==0)");
            bindAddr=*ba.begin();

}

MetricsHTTPProvider::Service::~Service()
{
}
bool MetricsHTTPProvider::Service::on_startService(const systemEvent::startService* e)
{
    if(!e) throw CommonError("!e");
    return true;
}

void registerMetricsHTTPProviderService(const char* pn)
{

logErr2("@@ %s",__PRETTY_FUNCTION__);
    XTRY;
    if(pn)
    {

        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::MetricsHTTPProviderService,"MetricsHTTPProvider",getEvents_metricsProvider());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::MetricsHTTPProviderService,MetricsHTTPProvider::Service::construct,"MetricsHTTPProvider");
        regEvents_metricsProvider();
    }
    XPASS;
}


bool MetricsHTTPProvider::Service::on_IncomingOnAcceptor(const rpcEvent::IncomingOnAcceptor*ev)
{
    MUTEX_INSPECTOR;
    XTRY;
    if(!ev) throw CommonError("!ev");
    auto &IDA=ev->e->id;

    logErr2("MetricsHTTPProvider: unhandled event %s %s %d",ev->id.dump().c_str(),__FILE__,__LINE__);
    XPASS;
    return false;
}
bool MetricsHTTPProvider::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    MUTEX_INSPECTOR;
    XTRY;
    auto &ID=e->id;

    if(ID==systemEventEnum::startService)
    {
        sendEvent(ServiceEnum::HTTP, new httpEvent::DoListen(bindAddr,ListenerBase::serviceId));
        return true;
    }
    if(ID==MetricsProviderEventEnum::AddProvider)
    {
        return AddProvider((MetricsProviderEvent::AddProvider*)e.operator->());
    }
    else if(ID==httpEventEnum::RequestIncoming)
    {
        return RequestIncoming((httpEvent::RequestIncoming*)e.operator->());
    }

    logErr2("MetricsHTTPProvider: unhandled event %s %s %d",e->id.dump().c_str(),__FILE__,__LINE__);
    XPASS;
    return false;
}

bool MetricsHTTPProvider::Service::AddProvider(MetricsProviderEvent::AddProvider* e)
{
    providers_->add_provider(e->provider.operator->());
    return true;
}

bool MetricsHTTPProvider::Service::RequestIncoming(httpEvent::RequestIncoming* e)
{

    HTTP::Response resp(iInstance);

   auto vals=providers_->getStringValues();//provider
   Json::Value j;
   for(auto&[k,v]: vals)
     {
         j[k]=v;
     }
    bool keepAlive=e->req->headers["CONNECTION"]=="Keep-Alive";
    if(keepAlive)
    {
        resp.http_header_out["Connection"]="Keep-Alive";
        resp.http_header_out["Keep-Alive"]="timeout=5, max=100000";
    }
    resp.content=j.toStyledString();
    if(keepAlive)
        resp.makeResponsePersistent(e->esi);
    else
        resp.makeResponse(e->esi);

    return true;
}
