#ifndef __METRICS_PROVIDER__H
#define __METRICS_PROVIDER__H
#include <vector>
#include <boost/variant.hpp>
#include <string_view>
#include "REF.h"
#include "mutexable.h"
class MetricsProvider : public Refcountable
{
    RWLock mx;
public:
  typedef boost::variant<double, long, std::string> Value;
  typedef std::vector<std::pair<std::string, Value> > MetricArray;

  MetricArray metricArray_;
  MetricArray get_values()
  {
      RLocker lk(mx);
      return metricArray_;
  }
  void add_value(const std::string& n,double v)
  {
      WLocker lk(mx);
      metricArray_.push_back({(std::string)n,v});
  }

  void add_value(const std::string& n,long v)
  {
      WLocker lk(mx);
      metricArray_.push_back({(std::string)n,v});
  }
  void add_value(const std::string& n,const std::string& v)
  {
      WLocker lk(mx);
      metricArray_.push_back(std::make_pair((std::string)n,v));
  }
};

#endif
