#include "Map.hpp"
#include "Vector.hpp"
#include "algo.hpp"
#include <algorithm>
#include <cstring>
#include <ios>
#include <iostream>

int main(int argc, const char *argw[]) {
	IR::Map<int, bool> map;
	int n;
	std::cin >> n;
	while(n != 0) {
		map[n] = true;
		for(const auto &[key, _] : map) 
			std::cout << key << ' ';
		std::cout << '\n';

		std::cin >> n;
	}

	IR::Vector<int> sorted;
	for(const auto &[key, _] : map) 
		sorted.push_back(key), sorted.push_back(key);
	n = 1;
	while(n != 0) {
		for(const auto &val : sorted) 
			std::cout << val << ' ';
		std::cout << '\n';

		std::cout << "Input value to search(0 to break): ";
		std::cin >> n;


		auto it = IR::algo::lowerBound(sorted.begin(), sorted.end(), n), it2 = std::lower_bound(std::begin(sorted), std::end(sorted), n);
		bool found = IR::algo::binarySearch(std::begin(sorted), std::end(sorted), n);

		std::cout << "Lower bound index = " << it - sorted.begin() << "(stl version = "<< it2 - sorted.begin() <<"); value = " << *it << '\n';
		std::cout << "Found by bin search = " << std::boolalpha << found << '\n';
	}


	

  return 0;
}
