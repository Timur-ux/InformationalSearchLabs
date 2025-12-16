#include "exceptions/KeyNotFound.hpp"
#include <format>

namespace SERVICE_NAMESPACE::exception {
	KeyNotFound::KeyNotFound(const char * key)
		: userver::server::handlers::ResourceNotFound(
				userver::server::handlers::ExternalBody{std::format("No key: \"{}\" found!", key)}
				) {}
} // namespace dbService::exception
