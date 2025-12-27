#include <filesystem>
#include <gtest/gtest.h>
#include "bplustree/INodeCache.hpp"
#include "bplustree/Node.hpp"
using namespace IR::bplustree;
static impl::Node<int, int, SameKeyOrdering::AsInserted> nodes[] {
	{.id = 0, .parentId = 0},
	{.id = 1, .parentId = 1},
	{.id = 2, .parentId = 2},
	{.id = 3, .parentId = 3},
	{.id = 4, .parentId = 4},
};

TEST(CacheTestSuite, Insert) {
	DefaultNodeCache<int, int, SameKeyOrdering::AsInserted> cache("/tmp/storage1");
	
	for(size_t i = 0; i < 5; ++i) 
		cache.insert(nodes[i]);

	for(size_t i = 0; i < 5; ++i) 
		EXPECT_TRUE(cache.contains(nodes[i].id));

	EXPECT_FALSE(cache.contains(-1));
	EXPECT_FALSE(cache.contains(100));
}

TEST(CacheTestSuite, Get) {
	std::filesystem::path p = "/tmp/storage1";
	std::filesystem::create_directories(p);
	DefaultNodeCache<int, int, SameKeyOrdering::AsInserted> cache(p, 1);
	
	for(size_t i = 0; i < 5; ++i) 
		cache.insert(nodes[i]);

	for(size_t i = 0; i < 4; ++i) 
		EXPECT_FALSE(cache.contains(nodes[i].id));
	EXPECT_TRUE(cache.contains(nodes[4].id));
	auto node = cache.get(nodes[4].id);

	ASSERT_EQ(node.id, nodes[4].id);
	ASSERT_EQ(node.parentId, nodes[4].parentId);
}
