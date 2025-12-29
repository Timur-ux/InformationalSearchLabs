#include "handlers/token.hpp"
#include "Vector.hpp"
#include "components/DataBase.hpp"
#include "exceptions/Int.hpp"
#include "schemas/token.hpp"
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <userver/components/component_context.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/serialize_container.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/logging/log.hpp>

namespace SERVICE_NAMESPACE {
TokenHandler::TokenHandler(const components::ComponentConfig &config,
                           const components::ComponentContext &context)
    : HttpHandlerBase(config, context),
      db_(context.FindComponent<DataBaseComponent>().GetDataBase()) {}

std::string TokenHandler::HandleRequest(HttpRequest &request,
                                        RequestContext &context) const {
  request.GetHttpResponse().SetContentType(
      userver::http::content_type::kApplicationJson);
  auto ids = formats::json::FromString(request.RequestBody())
                 .As<token::TokenRequestBody>();

  token::TokenResponseBody responseBody;
  for (const auto &id : ids) {
    IR::Vector<std::uint32_t> values = db_.findTokensByDocumentId(id);
    responseBody.emplace_back(token::TokenResponseBodyA{
        id, std::vector<std::uint32_t>{std::begin(values), std::end(values)}});
    LOG_DEBUG() << "For key with id: " << id << " found " << values.size()
                << " values";
  }

  auto responseJson = formats::json::ValueBuilder{responseBody}.ExtractValue();
  return formats::json::ToString(responseJson);
}
} // namespace SERVICE_NAMESPACE
