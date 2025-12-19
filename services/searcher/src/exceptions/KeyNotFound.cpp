#include "exceptions/KeyNotFound.hpp"
#include <format>
#include <userver/server/handlers/exceptions.hpp>

namespace SERVICE_NAMESPACE::exception {
KeyNotFoundException::KeyNotFoundException(const char *key)
    : ush::RequestParseError(ush::ExternalBody{
          std::format("Required key: \"{}\" not found", key)}) {}

} // namespace SERVICE_NAMESPACE::exception
