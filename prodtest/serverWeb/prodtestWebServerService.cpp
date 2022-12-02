#include "corelib/mutexInspector.h"
#include <time.h>
#include <time.h>
#include "ioBuffer.h"
#include "tools_mt.h"
#include "st_FILE.h"
#include "../common/xor.h"
#include <map>
#include "js_utils.h"
#include "configDB.h"
#include "prodtestWebServerService.h"
#include "../Event/Ping.h"
#include "serviceEnum.h"
#include "events_prodtestWebServer.hpp"



bool prodtestWebServer::Service::on_startService(const systemEvent::startService*)
{
    MUTEX_INSPECTOR;

    sendEvent(ServiceEnum::HTTP,new httpEvent::DoListen(bindAddr,ListenerBase::serviceId));
    sendEvent(ServiceEnum::Timer,new timerEvent::SetTimer(TIMER_PUSH_NOOP,NULL,NULL,1.5,ListenerBase::serviceId));
    return true;
}


bool prodtestWebServer::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    try {
        MUTEX_INSPECTOR;
        auto& ID=e->id;
        if(timerEventEnum::TickTimer==ID)
        {
            const timerEvent::TickTimer*ev=static_cast<const timerEvent::TickTimer*>(e.operator ->());
            if(ev->tid==TIMER_PUSH_NOOP)
            {
                for(auto &z: sessions)
                {
                    auto esi=z.second->esi;
                    if(esi.valid())
                    {
                        if(!esi->closed())
                        {
                            esi->write_("   ");
                        }
                    }
                    else logErr2("!if(esi.valid())");
                }
            }
            return true;
        }
        if(httpEventEnum::RequestIncoming==ID)
            return on_RequestIncoming((const httpEvent::RequestIncoming*)e.operator->());
        if(systemEventEnum::startService==ID)
            return on_startService((const systemEvent::startService*)e.operator->());


        if(prodtestEventEnum::AddTaskRSP==ID)
            return on_AddTaskRSP((const prodtestEvent::AddTaskRSP*)e.operator->());

        if(rpcEventEnum::IncomingOnConnector==ID)
        {
            rpcEvent::IncomingOnConnector *E=(rpcEvent::IncomingOnConnector *) e.operator->();
            auto& IDC=E->e->id;
            if(prodtestEventEnum::AddTaskRSP==IDC)
                return on_AddTaskRSP((const prodtestEvent::AddTaskRSP*)E->e.operator->());


            return false;
        }

        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            rpcEvent::IncomingOnAcceptor *E=(rpcEvent::IncomingOnAcceptor *) e.operator->();
            auto& IDA=E->e->id;
            if(prodtestEventEnum::AddTaskRSP==IDA)
                return on_AddTaskRSP((const prodtestEvent::AddTaskRSP*)E->e.operator->());


            return false;
        }


    }
    catch(CommonError& e)
    {
        logErr2("prodtestWebServer CommonError  %s",e.what());

    }
    catch(std::exception &e)
    {
        logErr2("prodtestWebServer std::exception  %s",e.what());
    }
    XPASS;
    return false;
}

prodtestWebServer::Service::~Service()
{
}


prodtestWebServer::Service::Service(const SERVICE_id& id, const std::string& nm,IInstance* ins):
    UnknownBase(nm),
    ListenerBuffered(nm,ins->getConfig(),id,ins),
    Broadcaster(ins)
{
    auto ba=ins->getConfig()->get_tcpaddr("bindAddr","0.0.0.0:8088","http listen address");
    if(ba.size()==0)
        throw CommonError("if(ba.size()==0)");

    bindAddr=*ba.begin();
    prodtestServerAddr=ins->getConfig()->get_string("prodtestServerAddr","local","server prodtest address");

}

void registerprodtestServerWebService(const char* pn)
{
    MUTEX_INSPECTOR;

    XTRY;
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::prodtestServerWeb,"prodtestServerWeb",getEvents_prodtestWebServer());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::prodtestServerWeb,prodtestWebServer::Service::construct,"prodtestServerWeb");
        regEvents_prodtestWebServer();
    }
    XPASS;
}






std::string index_html=R"ZXC(
<!DOCTYPE html>
<html>
<head>
<title>prodtest</title>
</head>

<body>

    <div>
     starting handling request
    </div>
</body>
</html>
)ZXC";

bool prodtestWebServer::Service::on_RequestIncoming(const httpEvent::RequestIncoming*e)
{

    HTTP::Response resp;
    auto S=check_session(e->req,resp);
    S->esi=e->esi;

    if(1){

        std::string query_string=e->req->params["query_string"];

//        for(int i=0;i<10;i++)
        {
            sendEvent(prodtestServerAddr,ServiceEnum::prodtestServer,new prodtestEvent::AddTaskREQ(S->sessionId,query_string,ListenerBase::serviceId));
        }
        return true;
    }
    if(0)
    {
        resp.content="<div>received response </div>";
        resp.makeResponse(e->esi);

    }

    return true;
}



REF_getter<prodtestWebServer::Session> prodtestWebServer::Service::check_session( const REF_getter<HTTP::Request>& req, HTTP::Response& resp)
{

    std::string session_id;
    if(!req->in_cookies.count(SESSION_ID))
    {
            for(int i=0;i<20;i++)
            {
                session_id+=char(rand());
            }
        resp.out_cookies[SESSION_ID]=session_id;
//        logErr2("insert session cookie %s",iUtils->bin2hex(session_id).c_str());
    }
    else
    {

        session_id=req->in_cookies[SESSION_ID];
    }
    REF_getter<Session> S(nullptr);
    auto it=sessions.find(session_id);
    if(it!=sessions.end())
        S=it->second;
    else
    {
        S=new Session(session_id);
//        logErr2("insert into sessions %s",iUtils->bin2hex(session_id).c_str());
        sessions.insert(std::make_pair(session_id,S));
    }
    S->lastTimeSessionHit=time(nullptr);
    return S;

}
REF_getter<prodtestWebServer::Session> prodtestWebServer::Service::get_session( const std::string& session_id)
{

    REF_getter<Session> S(nullptr);
    auto it=sessions.find(session_id);
    if(it!=sessions.end())
        S=it->second;
    else
    {
        throw CommonError("session not found %s",session_id.c_str());
    }
    S->lastTimeSessionHit=time(nullptr);
    return S;

}

bool prodtestWebServer::Service::on_AddTaskRSP(const prodtestEvent::AddTaskRSP*e)
{
    HTTP::Response resp;
    auto S=get_session(e->session);
    resp.content="<div>received response "+e->sampleString+"</div>";
    resp.makeResponse(S->esi);

    return true;
}
