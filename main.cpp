// ------------------------------
// |  File for some hand tests  |
// ------------------------------
#include "BPlusTree.hpp"
#include "Trie.hpp"
#include "factory/BPlusTreeFactory.hpp"
#include <clocale>
#include <cstring>
#include <iostream>
#include <utility>
static inline size_t wstrlen(const wchar_t * s) {
	size_t result = 0;
	while(*s != 0) 
		++result, ++s;
	return result;
}

int main() {
	auto db = IR::bplustree::FileBasedBPlusTreeFactory<int, int, IR::bplustree::SameKeyOrdering::AsInserted>(24).createTree();

	for(size_t i = 0; i < 10000; ++i) 
		db->insert(std::forward<int>(i), std::forward<int>(i));

	int n;
	while(std::cin >> n) {
		auto cursor = db->find(n);
		for(auto [key, value] : cursor) { 
			std::cout << key << ' ' << value<< '\n';
		}
		
	}
		
}
