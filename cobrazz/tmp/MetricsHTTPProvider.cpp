#include <string>
//#include <crypto/openssl.hpp>
//#include <utils/jemalloc.hpp>
#include <regex>

#include "MetricsHTTPProvider.hpp"
#include "CompositeMetricsProvider.hpp"
#include "REF.h"
//namespace UServerUtils
//{
  void copy_json_to_tmp();
    ReferenceCounting::SmartPtr<MetricsProvider> MetricsHTTPProvider::container;

    void* MetricsHTTPProvider::worker(MetricsHTTPProvider* _this)
    {
      const components::ComponentList component_list = components::MinimalServerComponentList()
        .Append<ConfigDistributor>();

      // crypto::impl::Openssl::Init();

      auto conf_replaced = std::regex_replace(config_z_yaml,std::regex("~port~"), std::to_string(_this->listen_port_));
      conf_replaced = std::regex_replace(conf_replaced,std::regex("~uri~"), std::string(_this->uri_));
      auto conf_prepared = std::make_unique<components::ManagerConfig>(components::ManagerConfig::FromString(conf_replaced, {}, {}));
      std::optional<components::Manager> manager;

      

      try
      {
      
        manager.emplace(std::move(conf_prepared), component_list);
        _this->state_=AS_ACTIVE;
      
      }
      catch (const std::exception& ex)
      {
        LOG_ERROR() << "Loading failed: " << ex;
      }
      



      while(true)
      {
        if(_this->stopped_)
        {
          return NULL;
        }

        sleep(1);
      }

      return NULL;
    }

    void
    MetricsHTTPProvider::activate_object()
    {
      

      copy_json_to_tmp();
      thread_ = std::thread(worker,this);
    }

    void
    MetricsHTTPProvider::deactivate_object()
    {

      state_=AS_DEACTIVATING;
      stopped_=true;
    }
    void
    MetricsHTTPProvider::wait_object()
    {
      
      thread_.join();
      state_=AS_NOT_ACTIVE;
    
    }

    bool
    MetricsHTTPProvider::active()
    {
      return state_==AS_ACTIVE;
    }
//     AS_ACTIVE,
//      AS_DEACTIVATING,
//      AS_NOT_ACTIVE



    MetricsHTTPProvider::MetricsHTTPProvider(MetricsProvider *mProv,unsigned int _listen_port, std::string_view _uri)
      : listen_port_(_listen_port),uri_(_uri)//, metricsProvider_(mProv)
    {

      
	container=mProv;
	state_=AS_NOT_ACTIVE;
    }

    MetricsHTTPProvider::~MetricsHTTPProvider()
    {
      
      if(state_!=AS_NOT_ACTIVE)
      {
        LOG_ERROR() << "Try to destruct active object MetricsHTTPProviderImpl";
      }
    }


//}
