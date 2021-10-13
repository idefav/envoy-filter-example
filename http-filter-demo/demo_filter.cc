#include <string>
#include <chrono>
#include <sstream>

#include "demo_filter.h"

using std::ostringstream;
using std::string;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;
using namespace std;

using Envoy::Http::DemoConfig;

namespace Envoy {
namespace Http {

// constructor for DemoConfig
DemoConfig::DemoConfig(const demo::DemoProp& proto_config, const LocalInfo::LocalInfo& local_info)
    : cluster_(proto_config.cluster()), enable_(proto_config.enable()), local_info_(local_info) {}

// DemoFilter
void DemoFilter::onDestroy() {}

// decode stream filter
FilterHeadersStatus DemoFilter::decodeHeaders(RequestHeaderMap&, bool) {
  ENVOY_LOG(trace,
            "Cluster: " + cluster().get() + "enable: " + (this->enable() ? "true" : "false"));
  // decoder_callbacks_->sendLocalReply(Http::Code::TooManyRequests,"demo filter
  // resp",nullptr,absl::nullopt,"to many req");
  std::cout << static_cast<void*>(this) << endl;
  start_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
  ENVOY_LOG(info, string("minRt:") + to_string(node_.minRt()) + string(" avgRt:") +
                      to_string(long(node_.avgRt())) + string(" passQps:") +
                      to_string(node_.passQps()));
  ENVOY_LOG(trace, "start time:" + std::to_string(start_time));
  if (node_.passQps() > 1) {
    ENVOY_LOG(warn, "blocked! passQps:" + to_string(node_.passQps()));
    return FilterHeadersStatus::StopIteration;
  }
  node_.increaseThreadNum();
  node_.addPassRequest(1);
  ENVOY_LOG(info, string("flow blocked, maxSuccessQps:") + to_string(node_.maxSuccessQps()) +
                      string(" currentThreadNum:") + to_string(node_.curThreadNum()) +
                      string(" minRt:") + to_string(node_.minRt()));
  if (node_.curThreadNum() > 1 &&
      node_.curThreadNum() > (node_.maxSuccessQps() * node_.minRt() / 1000)) {
    ENVOY_LOG(warn, string("flow blocked, maxSuccessQps:") + to_string(node_.maxSuccessQps()) +
                        string(" currentThreadNum:") + to_string(node_.curThreadNum()) +
                        string(" minRt:") + to_string(node_.minRt()));
    return FilterHeadersStatus::StopIteration;
  }
  return FilterHeadersStatus::Continue;
};
FilterDataStatus DemoFilter::decodeData(Buffer::Instance&, bool) {
  return FilterDataStatus::Continue;
}
void DemoFilter::setDecoderFilterCallbacks(StreamDecoderFilterCallbacks& callbacks) {
  decoder_callbacks_ = &callbacks;
}

// encode stream filter
Http::FilterHeadersStatus DemoFilter::encodeHeaders(Http::ResponseHeaderMap& respHeaders,
                                                    bool status) {
  auto end_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
  auto rt = end_time - start_time;

  ostringstream result;
  result << "start time:" << std::to_string(start_time) << ","
         << "end time:" << end_time << ","
         << "rt:" << std::to_string(rt) << " resp headers:" << respHeaders << " status:" << status;
  ENVOY_LOG(trace, result.str());
  node_.addRtAndSuccess(rt, 1);
  node_.decreaseThreadNum();
  ENVOY_LOG(info, string("minRt:") + to_string(node_.minRt()) + string(" avgRt:") +
                      to_string(long(node_.avgRt())) + string(" passQps:") +
                      to_string(node_.passQps()));
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