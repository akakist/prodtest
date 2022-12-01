#ifndef _________dfsReferrerEvent_h19Z1
#define _________dfsReferrerEvent_h19Z1
#include "___prodtestEvent.h"
#include <vector>


namespace prodtestEvent {




    class AddTaskREQ: public Event::Base
    {
        enum {channel=CHANNEL_100};


    public:
        static Base* construct(const route_t &r)
        {
            return new AddTaskREQ(r);
        }
        AddTaskREQ(const std::string& _session,
                const std::string& _sampleString,
                const route_t&r)
            :Base(prodtestEventEnum::AddTaskREQ,channel,r),
              session(_session),
              sampleString(_sampleString)
        {}
        AddTaskREQ(const route_t&r)
            :Base(prodtestEventEnum::AddTaskREQ,channel,r) {}
        std::string session;
        std::string sampleString;
        void unpack(inBuffer& o)
        {
            o>>session>>sampleString;
        }
        void pack(outBuffer&o) const
        {
            o<<session<<sampleString;
        }
        void jdump(Json::Value &) const
        {
        }

    };

    class AddTaskRSP: public Event::Base
    {
        enum {channel=CHANNEL_100};


    public:
        static Base* construct(const route_t &r)
        {
            return new AddTaskRSP(r);
        }
        AddTaskRSP(const std::string& _session,
                const std::string& _sampleString,
                const route_t&r)
            :Base(prodtestEventEnum::AddTaskRSP,channel,r),
              session(_session),
              sampleString(_sampleString)
        {}
        AddTaskRSP(const route_t&r)
            :Base(prodtestEventEnum::AddTaskRSP,channel,r) {}
        std::string session;
        std::string sampleString;
        void unpack(inBuffer& o)
        {
            o>>session>>sampleString;
        }
        void pack(outBuffer&o) const
        {
            o<<session<<sampleString;
        }
        void jdump(Json::Value &) const
        {
        }

    };







}
#endif
