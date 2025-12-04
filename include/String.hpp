#ifndef STRING_HPP_
#define STRING_HPP_
#include "Vector.hpp"
#pragma once
namespace IR {
using StringView = VectorView<wchar_t>;
class String : public Vector<wchar_t> {
	using parent = Vector<wchar_t>;
public:
	using parent::parent;

	String & operator+(const String &other) {
		reserve(size() + other.size());

		for(size_t i = 0; i < other.size(); ++i) 
			data_[size_ + i] = other.data_[i];

		return *this;
	}

	StringView view() const {
		return StringView{data_, size_};
	}
};

} // namespace IR
#endif // !STRING_HPP_
