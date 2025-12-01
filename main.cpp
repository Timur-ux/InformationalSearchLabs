#include <iostream>
#include "Map.hpp"

int main(int argc, const char * argw[]) {
	IR::Map<float, int> map;
	for(size_t i = 0; i < 10; ++i) 
		map[float(i) / 2] = i;

	std::cout << map << '\n';

	auto it = map.begin();
	std::cout << "first = " << it->key << '\n';
	
	return 0;
}

