#include "exceptions/UndefinedToken.hpp"
#include <cstdint>
#include <format>

namespace SERVICE_NAMESPACE::exception {
UndefinedTokenException::UndefinedTokenException(std::uint32_t token)
    : ush::ClientError(ush::ExternalBody{
          std::format("Given token {} first met in detokenization, so it's "
                      "string value is unknown",
                      token)}) {}
} // namespace SERVICE_NAMESPACE::exception
