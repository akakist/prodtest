#include "IInstance.h"
#include "colorOutput.h"
#include "configObj.h"
#include "version_mega.h"
#include "CUtils.h"
void registerRPCService(const char* pn);
void registerSocketModule(const char* pn);
void registerTimerService(const char* pn);
void registerSSL(const char* pn);
void registerOscarModule(const char* pn);
void registerOscarSecureModule(const char* pn);
void registertestServerWebService(const char* pn);
void registerHTTPModule(const char* pn);
void registerTestServerMetricsService(const char* pn);
void registerMetricsHTTPProviderService(const char* pn);

int mainTestMetrics(int argc, char** argv )
{
printf("@@ %s\n",__PRETTY_FUNCTION__);
    try {
        iUtils=new CUtils(argc, argv, "httpTest");

        registerSocketModule(NULL);
        registerTimerService(NULL);
        registerSSL(NULL);
        registerHTTPModule(NULL);
        registerTestServerMetricsService(NULL);
        registerMetricsHTTPProviderService(NULL);

        printf(GREEN("RUN TEST %s"),__PRETTY_FUNCTION__);

        {
            IInstance *instance1=iUtils->createNewInstance("testMetrics");
            ConfigObj *cnf1=new ConfigObj("testMetrics",
            "\nStart=TestMetrics"
"\n# MetricsHTTPProvider listen address"
"\nMetricsHTTPProvider.bindAddr=0.0.0.0:8088"
"\nHTTP.max_post=1000000"
"\nHTTP.doc_urls=/pics,/html,/css"
"\nHTTP.document_root=./www"
"\nSocketIO.epoll_timeout_millisec=10"
"\nSocketIO.listen_backlog=128"

"\n# socket poll thread count"
"\nSocketIO.n_workers=4"
            ""
                                         );
            instance1->setConfig(cnf1);
            instance1->initServices();
        }

        usleep(100);
        system("ab -n 100000 -k -c 100  http://localhost:8088/");

        delete iUtils;
        return 0;

    } catch (CommonError& e)
    {
        printf("CommonError %s\n",e.what());
    }
    return 1;
}
