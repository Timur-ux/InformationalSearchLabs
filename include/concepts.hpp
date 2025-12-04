#ifndef CONCEPTS_HPP_
#define CONCEPTS_HPP_
#include <compare>
#include <concepts>
#include <stdexcept>
#include <type_traits>
#pragma once

template <typename T>
concept Comparable = requires (const T & a, const T & b) {
	{a < b} -> std::convertible_to<bool>;
	{a == b} -> std::convertible_to<bool>;
};

template <typename T>
concept Defaulted = std::is_default_constructible_v<T>;

template <typename Base, typename Derived>
concept base_of = std::derived_from<Derived, Base>;

namespace IR::serde {
	template <typename T>
	struct Serializer {
		static constexpr bool initialized = false;
	};
};

template <typename T>
concept Serializable = IR::serde::Serializer<T>::initialized;

#endif // !CONCEPTS_HPP_
