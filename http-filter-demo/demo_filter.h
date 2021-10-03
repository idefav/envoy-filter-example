#pragma once
#include <string>

#include "http-filter-demo/demo_filter.pb.h"
#include "source/extensions/filters/http/common/pass_through_filter.h"
#include "source/common/common/logger.h"
#include "envoy/local_info/local_info.h"

namespace Envoy {
namespace Http {

class DemoConfig {
public:
  DemoConfig(const demo::DemoProp& proto_config, const LocalInfo::LocalInfo& local_info);

  const std::string& getCluster() const { return cluster_; }

  bool getEnable() const { return enable_; }

  const LocalInfo::LocalInfo& localInfo() const {return local_info_;}

private:
  const std::string cluster_;
  const bool enable_;
  const LocalInfo::LocalInfo& local_info_;
};

using DemoFilterShardConfigPtr = std::shared_ptr<DemoConfig>;

class DemoFilter : public StreamFilter,
                   Envoy::Http::PassThroughDecoderFilter,
                   Envoy::Http::PassThroughEncoderFilter,
                   Logger::Loggable<Logger::Id::filter> {
public:
  DemoFilter(DemoFilterShardConfigPtr);
  ~DemoFilter();

  // Http::StreamFilterBase
  void onDestroy() override;

  // Http::StreamDecoderFilter
  FilterHeadersStatus decodeHeaders(RequestHeaderMap&, bool) override;
  FilterDataStatus decodeData(Buffer::Instance&, bool) override;
  void setDecoderFilterCallbacks(StreamDecoderFilterCallbacks& callbacks) override;

  Http::FilterHeadersStatus encodeHeaders(Http::ResponseHeaderMap&, bool) override;
  Http::FilterDataStatus encodeData(Buffer::Instance&, bool) override;
  void setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks& callbacks) override;

private:
  const DemoFilterShardConfigPtr config_;
  StreamDecoderFilterCallbacks* decoder_callbacks_;
  StreamEncoderFilterCallbacks* encoder_callbacks_;

  LowerCaseString cluster() const;
  bool enable() const;
  DemoConfig demoConfig() const;
};
} // namespace Http
} // namespace Envoy