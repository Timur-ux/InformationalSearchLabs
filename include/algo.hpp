#ifndef ALGO_HPP_
#define ALGO_HPP_
#include "concepts.hpp"
#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <string>
#pragma once

namespace IR::algo {
template <RandomIterator TIt, ConvertibleFromIterator<TIt> TVal>
  requires Comparable<TVal>
TIt lowerBound(
    TIt first, TIt last, const TVal &value,
    std::function<bool(const TVal &, const TVal &)> cmp =
        [](const TVal &a, const TVal &b) { return a < b; }) {
  using Distance = std::iterator_traits<TIt>::difference_type;

  Distance len = last - first;
  while (len > 0) {
    Distance half = len >> 1; // len / 2
    TIt mid = first;
    mid = mid + half;
    if (cmp(*mid, value)) {
      first = mid;
      ++first;
      len -= half + 1;
    } else
      len = half;
  }

  return first;
}

template <RandomIterator TIt, ConvertibleFromIterator<TIt> TVal>
  requires Comparable<TVal>
bool binarySearch(TIt first, TIt last, const TVal &value) {
  TIt res = lowerBound(first, last, value);

  return res != last && *res == value;
}

/**
 * @brief Convert non negative long integer to C-string view
 * @param n nuber to convert (n >= 0)
 *
 * @return pointer to stringified n or NULL if n < 0
 */
const char * lltostring(long n);

template <typename CharT>
bool contains(CharT c, const CharT *s) {
	for(const CharT * _c = s; *_c != 0; ++_c) 
		if(*_c == c) 
			return true;
	return false;
}
} // namespace IR::algo
#endif // !ALGO_HPP_
