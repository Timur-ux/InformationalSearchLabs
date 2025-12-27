// ------------------------------
// |  File for some hand tests  |
// ------------------------------
#include "factory/BPlusTreeFactory.hpp"
#include <clocale>
#include <cstring>
#include <iostream>

int main() {

  using TreeFactory = IR::bplustree::FileBasedBPlusTreeFactory<
      int, int, IR::bplustree::SameKeyOrdering::AsInserted>;
  std::random_device device;

  const char *storagePath = "/tmp/";
  char storageName[31];
  for (size_t i = 0; i < 30; ++i)
    storageName[i] = device() % 26 + 'A';
  storageName[30] = 0;

  auto tree = TreeFactory(4, storagePath, storageName).createTree();
  for (int i = 0; i < 2500; ++i) {
    tree->insert(i, i * 2);
  }

	auto cursor = tree->find(-5, 1000000);
	std::cerr << "Found items:\n";
	for(size_t i = 0; auto [key, value] : cursor) {
		std::cerr << i++ << ") Key = " << key << "; value = " << value << '\n';
	} 
		
	
}
