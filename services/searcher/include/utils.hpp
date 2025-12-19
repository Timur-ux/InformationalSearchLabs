#ifndef SEARCH_UTILS_HPP_
#define SEARCH_UTILS_HPP_
#include "Map.hpp"
#include "Vector.hpp"
#include "concepts.hpp"
#include <concepts>
#include <cstdint>
#include <functional>
#include <vector>
#pragma once

inline std::vector<std::uint32_t>
getUnique(const std::vector<std::uint32_t> &data) {
  IR::Map<std::uint32_t, bool> entries;
  for (auto &item : data)
    entries[item] = true;

  std::vector<std::uint32_t> uniques(entries.size());
  for (size_t i = 0; auto [item, _] : entries)
    uniques[i++] = item;

  return uniques;
}

template <typename T>
  requires(!std::same_as<T, std::uint32_t>)
std::vector<std::uint32_t>
getUnique(const std::vector<T> &data,
          std::function<std::vector<std::uint32_t>(const T &)> extractor) {
	std::vector<std::uint32_t> uniques{};
	for(const T & elem : data) { 
		auto elemUniques = getUnique(extractor(elem));
		for(auto &elemUnique : elemUniques) 
			uniques.emplace_back(elemUnique);
	}

	return getUnique(uniques);
}

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
