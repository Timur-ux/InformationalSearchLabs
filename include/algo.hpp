#ifndef ALGO_HPP_
#define ALGO_HPP_
#include "concepts.hpp"
#include <iterator>
#pragma once

namespace IR::algo {
template <RandomIterator TIt, ConvertibleFromIterator<TIt> TVal>
  requires Comparable<TVal>
TIt lowerBound(TIt first, TIt last, const TVal &value) {
	using Distance = std::iterator_traits<TIt>::difference_type;

	Distance len = last - first;
	while(len > 0) {
		Distance half = len >> 1; // len / 2
		TIt mid = first;
		mid = mid + half;
		if(*mid < value) {
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
bool binarySearch (TIt first, TIt last, const TVal &value) {
	TIt res = lowerBound(first, last, value);

	return res != last && *res == value;
}
} // namespace IR::algo
#endif // !ALGO_HPP_
