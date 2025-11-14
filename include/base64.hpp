#ifndef BASE_64_HPP_
#define BASE_64_HPP_
#include <string>
#include <string_view>
std::string base64_encode(const std::string_view &in);
std::string base64_decode(const std::string_view &in); 

#endif // !BASE_64_HPP_
