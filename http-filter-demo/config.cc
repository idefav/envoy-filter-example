#include "http-filter-demo/config.h"

#include <string>

#include "envoy/registry/registry.h"

#include "source/common/protobuf/utility.h"
#include "http-filter-demo/demo_filter.pb.h"
#include "http-filter-demo/demo_filter.pb.validate.h"

namespace Envoy{
namespace Server{
namespace Configuration{
namespace DemoFilter{

Http::FilterFactoryCb DemoFilterConfig::createFilterFactoryFromProtoTyped(
    const demo::DemoProp& proto_config,
    const std::string&, Server::Configuration::FactoryContext& context) {
  Envoy::Http::DemoFilterShardConfigPtr filter_config = std::make_shared<Envoy::Http::DemoConfig>(
      proto_config, context.localInfo());
  return [filter_config](Envoy::Http::FilterChainFactoryCallbacks& callbacks) -> void {
    callbacks.addStreamFilter(std::make_shared<Envoy::Http::DemoFilter>(filter_config));
  };
}

// Router::RouteSpecificFilterConfigConstSharedPtr
// DemoFilter::createRouteSpecificFilterConfigTyped(
//     const demo::DemoProp& proto_config,
//     Server::Configuration::ServerFactoryContext& context, ProtobufMessage::ValidationVisitor&) {
//   return std::make_shared<const Envoy::Http::DemoConfig>(proto_config, context.localInfo(),
//                                               context.mainThreadDispatcher(), context.scope(),
//                                               context.runtime(), true);
// }

/**
 * Static registration for the demo filter. @see RegisterFactory.
 */
REGISTER_FACTORY(DemoFilterConfig,
                 Server::Configuration::NamedHttpFilterConfigFactory){"envoy.demo"};
} // DemoFilter
} // Configuration
} // Server
} // namespace Envoy