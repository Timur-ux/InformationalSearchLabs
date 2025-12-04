#include "Trie.hpp"
#include <cstring>
#include <iostream>

int main(int argc, const char *argw[]) {
  IR::Trie<char, int> trie;

	const char *strings[] = {"some", "say",  "you", "don't",
												 "need", "that", "way"};

  size_t n = sizeof(strings) / sizeof(const char *);
  std::cout << "n = " << n << '\n';
  for (size_t i = 0; i < n-1; ++i)
    trie.insert(strings[i], strlen(strings[i]), int(i));

  for (size_t i = 0; i < n; ++i) {
		const auto &val = trie.get(strings[i], strlen(strings[i]));
		if(val) 
			std::cout << strings[i] << " in the trie! Value = " << *val << '\n';
		else
			std::cout << strings[i] << " NOT in the trie!"<< '\n';
			
	}

  return 0;
}
