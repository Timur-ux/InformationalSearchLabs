#include "exceptions/Int.hpp"
#include <format>
#include <userver/server/handlers/exceptions.hpp>

namespace SERVICE_NAMESPACE::exception {
IdOutOfRange::IdOutOfRange(const std::string &id)
    : userver::server::handlers::RequestParseError(
          userver::server::handlers::ExternalBody{
              std::format("Given id: {} out of uint32_t range!", id)}) {}
IdInvalid::IdInvalid(const std::string &id)
    : userver::server::handlers::RequestParseError(
          userver::server::handlers::ExternalBody{std::format(
              "Given id: {} invalid (not a uint32_t number)!", id)}) {}

} // namespace SERVICE_NAMESPACE::exception
