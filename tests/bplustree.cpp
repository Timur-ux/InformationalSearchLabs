#include "factory/BPlusTreeFactory.hpp"
#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <iostream>
#include <random>

typedef enum { InMemory, FileBased } TreeType;

#include <gtest/gtest.h>

class BPlusTreeTestSuite
    : public testing::TestWithParam<std::tuple<TreeType, size_t>> {
  using key_type = int;
  using value_type = int;
  using same_key_ordering_type = IR::bplustree::SameKeyOrdering;

public:
  std::shared_ptr<IR::bplustree::BPlusTree<key_type, value_type,
                                           same_key_ordering_type::AsInserted>>
      tree;
  void SetUp() {
    auto [treeType, nodeCapacity] = GetParam();
    if (treeType == InMemory)
      tree = IR::bplustree::InMemoryBPlusTreeFactory<
                 key_type, value_type, same_key_ordering_type::AsInserted>(
                 nodeCapacity)
                 .createTree();
    else
      tree = IR::bplustree::FileBasedBPlusTreeFactory<
                 key_type, value_type, same_key_ordering_type::AsInserted>(
                 nodeCapacity)
                 .createTree();
  }
  void TearDown() {}
};

INSTANTIATE_TEST_SUITE_P(BPlusTreeGroup, BPlusTreeTestSuite,
                         testing::Combine(testing::Values(InMemory, FileBased),
                                          testing::Values(4, 100, 1024)));

TEST_P(BPlusTreeTestSuite, FindAll) {
  for (int i = 0; i < 2000; ++i)
    tree->insert(i, 2 * i);

  auto cursor = tree->find(-5, 10000);

  int i;
  for (i = 0; auto [key, value] : cursor) {
    ASSERT_EQ(key, i);
    ASSERT_EQ(value, 2 * i);
    ++i;
  }
  ASSERT_EQ(i, 2000);
}

TEST_P(BPlusTreeTestSuite, FindPart) {
  for (int i = 0; i < 2000; ++i)
    tree->insert(i, 2 * i);

  auto cursor = tree->find(100, 1000);

  auto it = cursor.begin(), end = cursor.end();
  for (int i = 100; i <= 1000; ++i) {
    ASSERT_NE(it, end);
    ASSERT_EQ(it->first, i);
    ASSERT_EQ(it->second, 2 * i);
    ++it;
  }
}

TEST(FileBasedBPlusTreeTestSuite, ConsistencyCheck) {
  using TreeFactory = IR::bplustree::FileBasedBPlusTreeFactory<
      int, int, IR::bplustree::SameKeyOrdering::AsInserted>;
  std::random_device device;

  const char *storagePath = "/tmp/";
  char storageName[31];
  for (size_t i = 0; i < 30; ++i)
    storageName[i] = device() % 26 + 'A';
  storageName[30] = 0;

  {
    auto tree = TreeFactory(50, storagePath, storageName).createTree();
    for (int i = 0; i <= 1000; ++i)
      tree->insert(i, i * 2);
  }

  auto tree = TreeFactory(50, storagePath, storageName).createTree();
  auto cursor = tree->find(0, 1000);

  auto it = cursor.begin(), end = cursor.end();
  for (int i = 0; i <= 1000; ++i) {
    ASSERT_NE(it, end);
    ASSERT_EQ(it->first, i);
    ASSERT_EQ(it->second, 2 * i);
    ++it;
  }
}
