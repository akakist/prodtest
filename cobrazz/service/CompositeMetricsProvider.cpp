#include "CompositeMetricsProvider.hpp"
class to_string_visitor : public boost::static_visitor<>
{
public:
    std::string str;
    void operator()(long i)
    {
        str=std::to_string(i);
    }
    void operator()(double i)
    {
        str=std::to_string(i);
    }
    void operator()(const std::string i)
    {
        str=i;
    }

};


void CompositeMetricsProvider::add_provider(MetricsProvider* p)
{
    WLocker lk(mx);
    providersMX.push_back(p);
}


MetricsProvider::MetricArray CompositeMetricsProvider::get_valuesMX()
{

    std::vector<REF_getter<MetricsProvider > > provs;
    {
        RLocker lk(mx);
        provs=providersMX;
    }

    MetricsProvider::MetricArray ret;

    for(auto& z:provs)
    {
      auto arr=z->get_values();
      for(auto &x: arr)
      {
          ret.push_back(x);
      }
    }
    return ret;

}

std::map<std::string,std::string> CompositeMetricsProvider::getStringValues()
{

    std::map<std::string,std::string> ret;
    auto arr=get_valuesMX();
    for(auto &x:arr )
    {
        for(auto& z:arr)
        {
              auto& key=z.first;
              auto &v=z.second;
              to_string_visitor vis;
              boost::apply_visitor(vis,v);
              ret[key]=vis.str;
        }
    }

    return ret;
}
