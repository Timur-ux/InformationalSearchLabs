#include "handlers/insert.hpp"
#include "components/DataBase.hpp"
#include "exceptions/KeyNotFound.hpp"
#include "schemas/insert.hpp"
#include <sys/types.h>
#include <userver/components/component_context.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/content_type.hpp>
#include <userver/server/handlers/exceptions.hpp>

namespace SERVICE_NAMESPACE {
	InsertHandler::InsertHandler(const components::ComponentConfig &config,
                                 const components::ComponentContext &context)
		: HttpHandlerJsonBase(config, context), db_(context.FindComponent<DataBaseComponent>().GetDataBase()) {}

InsertHandler::Value
InsertHandler::HandleRequestJsonThrow(const HttpRequest &request,
                                      const Value &requestJson,
                                      RequestContext &context) const {
  request.GetHttpResponse().SetContentType(
      http::content_type::kApplicationJson);

  auto requestBody = requestJson.As<insert::InsertRequestBody>();

  if (!requestBody.id.has_value())
    throw exception::KeyNotFound("id");
  if (!requestBody.tokens.has_value())
    throw exception::KeyNotFound("tokens");

  db_.insert(*requestBody.id, IR::VectorView<std::uint32_t>{
                                                   requestBody.tokens->data(),
                                                   requestBody.tokens->size()});

  insert::InsertResponseBody responseBody{"success"};

  auto responseJson = formats::json::ValueBuilder{responseBody}.ExtractValue();

  return responseJson;
}
} // namespace dbService
