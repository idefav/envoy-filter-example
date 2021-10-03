#include <string>

#include "demo_filter.h"

namespace Envoy{
namespace Http{


// constructor for DemoConfig
DemoConfig::DemoConfig(const demo::DemoProp& proto_config,const LocalInfo::LocalInfo& local_info): 
    cluster_(proto_config.cluster()),
    enable_(proto_config.enable()),
    local_info_(local_info){}

// DemoFilter
DemoFilter::DemoFilter(DemoFilterShardConfigPtr config):config_(config){}
DemoFilter::~DemoFilter(){}
void DemoFilter::onDestroy(){}

// decode stream filter
FilterHeadersStatus DemoFilter::decodeHeaders(RequestHeaderMap&, bool) {
    ENVOY_LOG(trace,"Cluster: "+ cluster().get() + "enable: "+ (this->enable()?"ture":"false"));
    return FilterHeadersStatus::Continue;
};
FilterDataStatus DemoFilter::decodeData(Buffer::Instance&, bool) {
    return FilterDataStatus::Continue;
}
void DemoFilter::setDecoderFilterCallbacks(StreamDecoderFilterCallbacks& callbacks) {
    decoder_callbacks_=&callbacks;
}

// encode stream filter
Http::FilterHeadersStatus DemoFilter::encodeHeaders(Http::ResponseHeaderMap&, bool){
    return FilterHeadersStatus::Continue;
}
Http::FilterDataStatus DemoFilter::encodeData(Buffer::Instance&, bool) {
    return FilterDataStatus::Continue;
};
void DemoFilter::setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks& callbacks) {
    encoder_callbacks_=&callbacks;
};

  LowerCaseString DemoFilter::cluster() const{
      return LowerCaseString(config_->getCluster());
  }
  bool DemoFilter::enable() const{
      return config_->getEnable();
  }

  DemoConfig DemoFilter::demoConfig()const{
      return *config_.get();
  }

}
}