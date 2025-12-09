#include "BPlusTree.hpp"
#include <cstring>
#include <ios>
#include <iostream>
#include <memory>

int main(int argc, const char *argw[]) {
	IR::bplustree::BPlusTree<int, int> storage(std::make_unique<IR::bplustree::InMemoryNodeManager<int, int>>(), 20);

  for (size_t i = 0; i < 50; ++i) {
    storage.insert(i, i / 2);
  }

  storage.print(std::cout);

  while (true) {
    std::cout << "Input left border to find: ";
    int left, right;
    if (!(std::cin >> left))
      break;
    std::cout << "Input right border to find: ";
    if (!(std::cin >> right))
      break;

    auto result = storage.find(left, right);
    for (auto it = std::begin(result); it != std::end(result); ++it) {
      std::cout << "{ " << it->first << ' ' << it->second << "} ";
    }
    std::cout << '\n';
  }

  return 0;
}
