#ifndef INSERT_HANDLER_HPP_
#define INSERT_HANDLER_HPP_
#pragma once
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
#pragma once
#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>

namespace dbService {
using namespace userver;
class InsertHandler final : public server::handlers::HttpHandlerBase {
	public:
		static constexpr std::string_view kName = "insert-handler";

		using server::handlers::HttpHandlerBase::HttpHandlerBase;

		std::string HandleRequest(server::http::HttpRequest &request, server::request::RequestContext &context) const override;
};
} // namespace dbService
#endif // !INSERT_HANDLER_HPP_
