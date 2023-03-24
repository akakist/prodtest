#ifndef _____5_ADD_PROVODER__H
#define _____5_ADD_PROVODER__H

#include "__MetricsHTTPProvider.h"
#include "MetricsProvider.hpp"
#include "event_mt.h"
namespace MetricsProviderEvent
{
    class AddProvider: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        AddProvider(const std::string& n,const REF_getter<MetricsProvider>& __R, const route_t & r)
            :NoPacked(MetricsProviderEventEnum::AddProvider,r),name(n),
             provider(__R) {}
        const std::string name;
        const REF_getter<MetricsProvider> provider;
        void jdump(Json::Value &) const
        {
        }

    };


}

#endif
