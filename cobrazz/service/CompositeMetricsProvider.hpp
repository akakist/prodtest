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
    std::vector<REF_getter<MetricsProvider> > providersMX;
    RWLock mx;
public:
  void
  add_provider(MetricsProvider* p);


  MetricsProvider::MetricArray get_valuesMX();
  std::map<std::string,std::string> getStringValues();
  

};
#endif

