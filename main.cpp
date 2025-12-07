#include "BPlusTree.hpp"
#include <cstring>
#include <ios>
#include <iostream>

int main(int argc, const char *argw[]) {
	IR::BPlusTree<int, int> storage(4);


	for(size_t i = 0; i < 30; ++i) {
		storage.insert(i, i / 2);

		std::cout << "Insertion = " << i<< '\n';
		storage.print(std::cout);
		std::cout << "----------------"<< '\n';
	}
	std::cout << storage.size() << '\n';
	storage.print(std::cout);

  return 0;
}
