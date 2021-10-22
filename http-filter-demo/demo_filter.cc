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
    : cluster_(proto_config.cluster()), enable_(proto_config.enable()), local_info_(local_info) {
  node_ = make_shared<StasticNode>();
}

// DemoFilter
void DemoFilter::onDestroy() {}

void DemoFilter::complete(){
  decoder_callbacks_->sendLocalReply(Http::Code::InternalServerError, "limited", nullptr,
                                 absl::nullopt, "limited err");
  decoder_callbacks_->streamInfo().setResponseFlag(StreamInfo::ResponseFlag::RateLimitServiceError);
}

// decode stream filter
FilterHeadersStatus DemoFilter::decodeHeaders(RequestHeaderMap&, bool) {
  ENVOY_LOG(trace,
            "Cluster: " + cluster().get() + "enable: " + (this->enable() ? "true" : "false"));
  // decoder_callbacks_->sendLocalReply(Http::Code::TooManyRequests,"demo filter
  // resp",nullptr,absl::nullopt,"to many req");
  std::cout << static_cast<void*>(this) << endl;
  start_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
  ENVOY_LOG(info, string("minRt:") + to_string(config_->node()->minRt()) + string(" avgRt:") +
                      to_string(long(config_->node()->avgRt())) + string(" passQps:") +
                      to_string(config_->node()->passQps())+string(" currThreadNum:"+to_string(config_->node()->curThreadNum())));
  ENVOY_LOG(trace, "start time:" + std::to_string(start_time));
  // if (config_->node()->passQps() > 1) {
  //   ENVOY_LOG(warn, "blocked! passQps:" + to_string(config_->node()->passQps()));
  //   this->complete();
  //   return FilterHeadersStatus::StopIteration;
  // }
  config_->node()->increaseThreadNum();
  config_->node()->addPassRequest(1);
  node_.increaseThreadNum();
  node_.addPassRequest(1);

  if (config_->node()->curThreadNum() > 1 &&
      config_->node()->curThreadNum() > (config_->node()->maxSuccessQps() * config_->node()->minRt() / 1000)) {
    ENVOY_LOG(warn, string("flow blocked, maxSuccessQps:") + to_string(config_->node()->maxSuccessQps()) +
                        string(" currentThreadNum:") + to_string(config_->node()->curThreadNum()) +
                        string(" minRt:") + to_string(config_->node()->minRt()));
    this->complete();
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
  config_->node()->addRtAndSuccess(rt, 1);
  config_->node()->decreaseThreadNum();
  node_.addRtAndSuccess(rt, 1);
  node_.decreaseThreadNum();
  ENVOY_LOG(info, string("minRt:") + to_string(config_->node()->minRt()) + string(" avgRt:") +
                      to_string(long(config_->node()->avgRt())) + string(" passQps:") +
                      to_string(config_->node()->passQps()));
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