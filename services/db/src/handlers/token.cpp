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
  const std::vector<std::string> &ids = request.GetArgVector("id");

  token::TokenResponseBody responseBody;
  for (const auto &id : ids) {
    long _id = -1;
    try {
      _id = std::stol(id);
    } catch (std::invalid_argument &) {
      throw exception::IdInvalid(id);
    } catch (std::out_of_range &) {
      throw exception::IdOutOfRange(id);
    }
    if (_id < 0)
      throw exception::IdInvalid(id);
    if (_id > std::numeric_limits<std::uint32_t>::max())
      throw exception::IdInvalid(id);

    std::uint32_t key{static_cast<uint32_t>(_id)};
    IR::Vector<std::uint32_t> values =
        db_.findTokensByDocumentId(key);
    responseBody.emplace_back(token::TokenResponseBodyA{
        key, std::vector<std::uint32_t>{std::begin(values), std::end(values)}});
    LOG_DEBUG() << "For key with id: " << key << " found " << values.size()
                << " values";
  }

  auto responseJson = formats::json::ValueBuilder{responseBody}.ExtractValue();
  return formats::json::ToString(responseJson);
}
} // namespace SERVICE_NAMESPACE
