#ifndef ALGO_HPP_
#define ALGO_HPP_
#include "concepts.hpp"
#include <algorithm>
#pragma once

 //    {
 //      typedef typename iterator_traits<_ForwardIterator>::difference_type
	// _DistanceType;
	//
 //      _DistanceType __len = std::distance(__first, __last);
	//
 //      while (__len > 0)
	// {
	//   _DistanceType __half = __len >> 1;
	//   _ForwardIterator __middle = __first;
	//   std::advance(__middle, __half);
	//   if (__comp(__middle, __val))
	//     {
	//       __first = __middle;
	//       ++__first;
	//       __len = __len - __half - 1;
	//     }
	//   else
	//     __len = __half;
	// }
 //      return __first;
 //    }
namespace IR {
template <RandomIterator TIt, ConvertibleFromIterator<TIt> TVal>
  requires ComparableIterator<TIt> && Comparable<TVal>
TIt lowrBound(TIt first, TIt last, const TVal &value) {
	std::lower_bound()
	TIt mid;
	while(last - first >= 0) {
		mid = first + (last - first) / 2;
		if(*mid < value) 
			first = (++mid);
		else if(*mid > value)
			last = (--mid);
		else
			return mid;
	}
	return last;
}
} // namespace IR
#endif // !ALGO_HPP_
