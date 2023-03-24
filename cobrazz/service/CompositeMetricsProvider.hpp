#ifndef ___CompositeMetricsProvider__H
#define ___CompositeMetricsProvider__H
#include <map>
#include <set>
#include <map>
#include <mutex>
#include "../Event/MetricsHTTPProvider/MetricsProvider.hpp"
#include "mutexable.h"
class CompositeMetricsProvider: public Refcountable
{
    std::map<std::string, REF_getter<MetricsProvider> > providersMX;
    Mutex mx;
public:
  void
  add_provider(const std::string &name, MetricsProvider* p);


  MetricsProvider::MetricArray get_values();
  std::map<std::string,std::string> getStringValues();
  

};
#endif

