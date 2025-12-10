#ifndef CONCEPTS_HPP_
#define CONCEPTS_HPP_
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

template <typename T>
concept RandomIterator =
    std::is_same_v<typename std::iterator_traits<T>::iterator_category,
                   std::random_access_iterator_tag>;

template <typename TVal, typename TIt>
concept ConvertibleFromIterator = requires (TIt iter) {
	{*iter} -> std::convertible_to<TVal>;
};

template <typename Base, typename Derived>
concept base_of = std::derived_from<Derived, Base>;

template <typename T, typename U>
concept SameAs = std::is_same_v<std::remove_reference_t<std::remove_const_t<T>>, std::remove_reference_t<std::remove_const_t<U>>>;

#endif // !CONCEPTS_HPP_
