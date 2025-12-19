// ------------------------------
// |  File for some hand tests  |
// ------------------------------
#include "BPlusTree.hpp"
#include "Trie.hpp"
#include "factory/BPlusTreeFactory.hpp"
#include <clocale>
#include <cstring>
#include <iostream>
static inline size_t wstrlen(const wchar_t * s) {
	size_t result = 0;
	while(*s != 0) 
		++result, ++s;
	return result;
}

int main() {
	setlocale(LC_ALL, "");
	static constexpr size_t nStrings = 6;
	static constexpr const wchar_t *strings[nStrings] = {
		L"Some",
		L"Текст",
		L"Длинная дорога",
		L"Me",
		L"I",
		L"Docker desctop"
	};
	IR::Trie<wchar_t, unsigned int> *trie;
		trie = new IR::Trie<wchar_t, unsigned int>;
		for(size_t i = 0; i < nStrings; ++i) 
			trie->insert({strings[i], wstrlen(strings[i])}, i);
	trie->print(std::wcout);
	std::cout << "----------------" << '\n';
}
