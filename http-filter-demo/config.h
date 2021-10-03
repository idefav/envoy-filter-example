#pragma once


#include "http-filter-demo/demo_filter.pb.h"
#include "http-filter-demo/demo_filter.pb.validate.h"
#include "demo_filter.h"
#include "envoy/server/factory_context.h"
#include "source/extensions/filters/http/common/factory_base.h"

namespace Envoy{
namespace Server{
namespace Configuration{
namespace DemoFilter{
class DemoFilterConfig: public Envoy::Extensions::HttpFilters::Common::FactoryBase<demo::DemoProp>{

public:
DemoFilterConfig(): FactoryBase("demo"){}
private:
  Http::FilterFactoryCb createFilterFactoryFromProtoTyped(
      const demo::DemoProp& proto_config,
      const std::string& stats_prefix, Server::Configuration::FactoryContext& context) override;
//   Router::RouteSpecificFilterConfigConstSharedPtr createRouteSpecificFilterConfigTyped(
//       const demo::DemoProp& proto_config,
//       Server::Configuration::ServerFactoryContext&, ProtobufMessage::ValidationVisitor&) override;
};
} // namespace DemoFilter
} // namespace Envoy
} // namespace Configuration
} // namespace Server