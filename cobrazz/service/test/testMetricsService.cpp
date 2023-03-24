#include "corelib/mutexInspector.h"
#include <time.h>
#include <time.h>
#include "ioBuffer.h"
#include "tools_mt.h"
#include "st_FILE.h"
#include <map>
#include "js_utils.h"
#include "configDB.h"
#include "testMetricsService.h"
//#include "___testEvent.h"
#include "serviceEnum.h"



bool testMetrics::Service::on_startService(const systemEvent::startService*)
{
    MUTEX_INSPECTOR;

//    sendEvent(ServiceEnum::HTTP,new httpEvent::DoListen(bindAddr,ListenerBase::serviceId));
    sendEvent(ServiceEnum::MetricsHTTPProviderService,new MetricsProviderEvent::AddProvider("iaia", provider_,ListenerBase::serviceId));
    sendEvent(ServiceEnum::Timer,new timerEvent::SetTimer(1,NULL,NULL,0.1,ListenerBase::serviceId));

    return true;
}


bool testMetrics::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    try {
        MUTEX_INSPECTOR;
        auto& ID=e->id;
        if(ID==timerEventEnum::TickTimer)
        {
            static long n=0;
            provider_->add_value("ximikall",n++);
            return true;
        }
        if(httpEventEnum::RequestIncoming==ID)
            return on_RequestIncoming((const httpEvent::RequestIncoming*)e.operator->());
        if(systemEventEnum::startService==ID)
            return on_startService((const systemEvent::startService*)e.operator->());



        if(rpcEventEnum::IncomingOnConnector==ID)
        {
            rpcEvent::IncomingOnConnector *E=(rpcEvent::IncomingOnConnector *) e.operator->();
            auto& IDC=E->e->id;


            return false;
        }

        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            rpcEvent::IncomingOnAcceptor *E=(rpcEvent::IncomingOnAcceptor *) e.operator->();
            auto& IDA=E->e->id;


            return false;
        }


    }
    catch(CommonError& e)
    {
        logErr2("testMetrics CommonError  %s",e.what());

    }
    catch(std::exception &e)
    {
        logErr2("testMetrics std::exception  %s",e.what());
    }
    XPASS;
    return false;
}

testMetrics::Service::~Service()
{

}


testMetrics::Service::Service(const SERVICE_id& id, const std::string& nm,IInstance* ins):
    UnknownBase(nm),
    ListenerBuffered1Thread(this,nm,ins->getConfig(),id,ins),
    Broadcaster(ins),provider_(new MetricsProvider)
{

}

void registerTestServerMetricsService(const char* pn)
{
    MUTEX_INSPECTOR;

    XTRY;
    if(pn)
    {
        std::set<EVENT_id> es;
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::TestMetrics,"TestMetrics",es);
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::TestMetrics,testMetrics::Service::construct,"TestMetrics");
//        regEvents_prodtestMetrics();
    }
    XPASS;
}







bool testMetrics::Service::on_RequestIncoming(const httpEvent::RequestIncoming*e)
{

    HTTP::Response resp(getIInstance());

    {
        bool keepAlive=iUtils->strupper(e->req->headers["CONNECTION"])=="KEEP-ALIVE";
//        printf("keepalive %d\n",keepAlive);
//        keepAlive=true;

        if(keepAlive)
        {
            resp.http_header_out["Connection"]="Keep-Alive";
            resp.http_header_out["Keep-Alive"]="timeout=15, max=1000000";
        }
        resp.content="<div>received response </div>";
        if(keepAlive)
            resp.makeResponsePersistent(e->esi);
        else
            resp.makeResponse(e->esi);

    }

    return true;
}


