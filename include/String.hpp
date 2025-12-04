#ifndef STRING_HPP_
#define STRING_HPP_
#include "Vector.hpp"
#pragma once
namespace IR {
using String = Vector<wchar_t>;
using StringView = VectorView<wchar_t>;
} // namespace IR
#endif // !STRING_HPP_
