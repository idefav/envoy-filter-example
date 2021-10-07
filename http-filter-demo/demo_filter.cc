#include <string>
#include <chrono>
#include <sstream>

#include "demo_filter.h"

using std::ostringstream;
using std::string;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;

using Envoy::Http::DemoConfig;

namespace Envoy {
namespace Http {

// constructor for DemoConfig
DemoConfig::DemoConfig(const demo::DemoProp& proto_config, const LocalInfo::LocalInfo& local_info)
    : cluster_(proto_config.cluster()), enable_(proto_config.enable()), local_info_(local_info) {
      
    }

// DemoFilter
void DemoFilter::onDestroy() {}

// decode stream filter
FilterHeadersStatus DemoFilter::decodeHeaders(RequestHeaderMap&, bool) {
  ENVOY_LOG(trace,
            "Cluster: " + cluster().get() + "enable: " + (this->enable() ? "true" : "false"));
  // decoder_callbacks_->sendLocalReply(Http::Code::TooManyRequests,"demo filter
  // resp",nullptr,absl::nullopt,"to many req");
  start_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
  ENVOY_LOG(trace, "start time:" + std::to_string(start_time));
  return FilterHeadersStatus::Continue;
};
FilterDataStatus DemoFilter::decodeData(Buffer::Instance&, bool) {
  return FilterDataStatus::Continue;
}
void DemoFilter::setDecoderFilterCallbacks(StreamDecoderFilterCallbacks& callbacks) {
  decoder_callbacks_ = &callbacks;
}

// encode stream filter
Http::FilterHeadersStatus DemoFilter::encodeHeaders(Http::ResponseHeaderMap&, bool) {
  auto end_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
  auto rt = end_time - start_time;

  ostringstream result;
  result << "start time:" << std::to_string(start_time) << ","
         << "end time:" << end_time << ","
         << "rt:" << std::to_string(rt);
  ENVOY_LOG(trace, result.str());
  return FilterHeadersStatus::Continue;
}
Http::FilterDataStatus DemoFilter::encodeData(Buffer::Instance&, bool) {
  return FilterDataStatus::Continue;
};
void DemoFilter::setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks& callbacks) {
  encoder_callbacks_ = &callbacks;
};

LowerCaseString DemoFilter::cluster() const { return LowerCaseString(config_->getCluster()); }
bool DemoFilter::enable() const { return config_->getEnable(); }

DemoConfig DemoFilter::demoConfig() const { return *config_.get(); }

} // namespace Http
} // namespace Envoy