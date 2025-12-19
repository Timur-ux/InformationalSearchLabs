#ifndef QUOTE_SEARCH_HANDLER_HPP_
#define QUOTE_SEARCH_HANDLER_HPP_

#include <cstdint>
#include <userver/clients/http/client.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/clients/http/request.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_config.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
#include <userver/storages/mongo/component.hpp>
#include <userver/storages/mongo/pool.hpp>

namespace SERVICE_NAMESPACE {
using namespace userver;
class QuoteSearchHandler final : public server::handlers::HttpHandlerJsonBase {
  storages::mongo::PoolPtr pool_;
  clients::http::Client &httpClient_;

public:
  static constexpr std::string_view kName = "quote-search-handler";

  using server::handlers::HttpHandlerJsonBase::HttpHandlerJsonBase;
  using Value = formats::json::Value;
  using HttpRequest = server::http::HttpRequest;
  using RequestContext = server::request::RequestContext;

  QuoteSearchHandler(const components::ComponentConfig &config,
               const components::ComponentContext &context);

  Value HandleRequestJsonThrow(const HttpRequest &request,
                               const Value &requestJson,
                               RequestContext &context) const override;
};
} // namespace SERVICE_NAMESPACE

#endif // !QUOTE_SEARCH_HANDLER_HPP_

