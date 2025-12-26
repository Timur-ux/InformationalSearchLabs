#ifndef SEARCH_UTILS_HPP_
#define SEARCH_UTILS_HPP_
#include "Map.hpp"
#include "Vector.hpp"
#include "concepts.hpp"
#include <concepts>
#include <cstdint>
#include <functional>
#include <vector>
#include "algo.hpp"
#pragma once

using namespace IR::algo;
template<Comparable T>
IR::Vector<std::uint32_t> zString(const IR::Vector<T> &s) {
  int64_t l = 0, r = 0;
	IR::Vector<std::uint32_t> z(s.size(), 0);

  for(int64_t i = 1; i < s.size(); ++i) {
    z[i] = std::max((int64_t)0, std::min(r - i, (int64_t)z[i - l]));
    while(i + z[i] < s.size() && s[z[i]] == s[i + z[i]])
      ++z[i];

    if(i + z[i] > r) {
      r = i + z[i];
      l = i;
    }
  }

  return z;
}

template <Comparable T>
bool zStringContains(IR::VectorView<T> s, IR::VectorView<T> sub) {
	IR::Vector<T> concatenated{sub.data(), sub.size()};
	concatenated.emplace_back(-1);
	for(const T & el: s) 
		concatenated.emplace_back(el);
	
  auto z = zString(concatenated);

  for(size_t i = 0; i < z.size(); ++i) {
    if(z[i] == sub.size()) 
			return true;
  }
  
  return false;
}
#endif // !SEARCH_UTILS_HPP_
