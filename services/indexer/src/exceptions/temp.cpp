#include "exceptions/temp.hpp"
#include <format>
#include <userver/server/handlers/exceptions.hpp>

namespace SERVICE_NAMESPACE::exception {
	TempException::TempException()
		: userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{ "error message here, will be returned to user" },
				userver::server::handlers::InternalMessage{ "internal message for logging" }
				) {}

} // namespace SERVICE_NAMESPACE::exception
