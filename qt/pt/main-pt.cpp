#include "IUtils.h"
#include "CUtils.h"
#include <stdio.h>
#include <cstdio>
#include <string>
#include "../common/xor.h"
#include "IConfigDB.h"
#include "configObj.h"
#include "Events/System/Net/http/___httpEvent.h"
#include "Events/System/Net/oscar/___oscarEvent.h"
#include "Events/System/Net/rpc/___rpcEvent.h"
#include "Events/System/Net/socket/_________socketEvent.h"
#include "Events/System/timer/___timerEvent.h"
#include "../Event/Ping.h"
void registerOscarModule(const char*);
void registerRPCService(const char*);
void registerSocketModule(const char*);
void registerTimerService(const char*);
void registerprodtestServerService(const char* pn);
void registerprodtestServerWebService(const char* pn);
void registerHTTPModule(const char* pn);
void registerModules()
{
    registerOscarModule(NULL);
    registerOscarModule(NULL);
    registerRPCService(NULL);
    registerSocketModule(NULL);
    registerTimerService(NULL);
    registerprodtestServerService(NULL);
    registerprodtestServerWebService(NULL);
    registerHTTPModule(NULL);


}
bool ex=false;
bool is_term=false;
static time_t exTime=0;

void onterm(int signum)
{
    if(is_term) return;
    is_term=true;
    static std::string ss;
    char s[200];
    snprintf(s,sizeof(s),RED("Terminating on SIGNAL %d"),signum);
    if(ss!=s)
    {
        logErr2("%s",s);
        ss=s;
    }

    //printf("ex %d \n",ex);
    try
    {
        exTime=time(NULL);
        ex=true;

        if (!ex || time(NULL)-exTime>2)
        {
//    	    exTime=time(NULL);
//#ifndef WIN32

            if(!iUtils)
                return;
            auto tc=iUtils->getIThreadNameController();
            if(!tc)
                return;
#ifndef WIN32
            if(signum==SIGHUP)
            {
                tc->print_term(signum);
                return;
            }
#endif

            exTime=time(NULL);
            ex=true;
            iUtils->setTerminate();

            tc->print_term(signum);
//            if(iUtils)
//                delete iUtils;


//            mega.stop();


            std::cout << RED("Terminating on SIGNAL ") << signum << std::endl;
//            exit(1);
        }
    }
    catch(CommonError &e)
    {
//        delete cFactory;
        std::cerr<<"onterm exception: "<<e.what()<<std::endl;
    }
    catch(...)
    {
//        delete cFactory;
        std::cerr<<"--Error: unknow error in on_term()"<<std::endl;
    }
}

void help()
{
    printf(R"ZXC(
                    Options:

                    )ZXC");
}


static bool win32Initialized=false;
int main(int argc, char *argv[])
{

#ifdef _WIN32
    if(!win32Initialized)
    {
        win32Initialized=true;
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;
        wVersionRequested = MAKEWORD(2, 2);
        err = WSAStartup(wVersionRequested, &wsaData);
        if (err != 0)
        {
            logErr2("WSAStartup failed\n");
            return 1;
        }
    }
#endif
    XTRY;
#ifndef _WIN32
    signal(SIGPIPE,SIG_IGN);
#endif
    signal(SIGABRT,onterm);
    signal(SIGTERM,onterm);
    signal(SIGINT,onterm);
    signal(SIGSEGV,onterm);
    signal(SIGFPE,onterm);
    signal(SIGILL,onterm);
#ifndef _WIN32
    signal(SIGQUIT,onterm);
    signal(SIGBUS,onterm);
    signal(SIGHUP,onterm);
#endif
    signal(10,onterm);

    srand(time(NULL));

    try {
        int opt;

        iUtils=new CUtils(argc,argv,argv[0]);
        iUtils->setFilesDir(".");
        IInstance *instance=iUtils->createNewInstance(argv[0]);

        while ((opt = getopt(argc, argv, "c:m:?")) != -1)
        {
            switch (opt)
            {
            case '?':help();exit(1);

            default:
            {
                std::cout<<"invalid option "<<opt<<std::endl;
                help();
                return -1;
            }

            }
        }
        printf("-? - help\n");



        registerModules();
        {
            ConfigObj *cnf1=new ConfigObj("pt.conf");
            instance->setConfig(cnf1);
            instance->getServiceOrCreate(ServiceEnum::prodtestServer);
            instance->getServiceOrCreate(ServiceEnum::prodtestServerWeb);
        }


        while(!ex)
        {
            usleep(100000);

        }
        delete iUtils;
    }
    catch(CommonError& e)
    {
        std::cerr<<"catch: "<<e.what()<<std::endl;
    }
    XPASS;
    return 0;
}
