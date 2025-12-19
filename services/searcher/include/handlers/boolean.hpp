#ifndef TOKEN_HANDLER_HPP_
#define TOKEN_HANDLER_HPP_

#include "Vector.hpp"
#include <chrono>
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
#include <vector>

namespace SERVICE_NAMESPACE {
using namespace userver;
class BooleanSearchHandler final : public server::handlers::HttpHandlerJsonBase {
  storages::mongo::PoolPtr pool_;
  clients::http::Client &httpClient_;

	static constexpr size_t retries_ = 1;

	const char * tokenizeServiceAddr_;
	const char * dbServiceAddr_;

  void indexPage(std::uint32_t pageId) const;
  std::vector<std::uint32_t> tokenize(std::string data) const;
  void insertTokensToDB(std::uint32_t pageId, std::vector<std::uint32_t> tokens) const;

public:
  static constexpr std::string_view kName = "index-handler";

  using server::handlers::HttpHandlerJsonBase::HttpHandlerJsonBase;
  using Value = formats::json::Value;
  using HttpRequest = server::http::HttpRequest;
  using RequestContext = server::request::RequestContext;

  BooleanSearchHandler(const components::ComponentConfig &config,
               const components::ComponentContext &context);
  Value HandleRequestJsonThrow(const HttpRequest &request,
                               const Value &requestJson,
                               RequestContext &context) const override;
};
} // namespace SERVICE_NAMESPACE

#endif // !TOKEN_HANDLER_HPP_
