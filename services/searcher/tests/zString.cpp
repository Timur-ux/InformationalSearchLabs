#include <cstdint>
#include <gtest/gtest.h>

#include "utils.hpp"

TEST(ZstringTestSuite, test1) {
	static std::uint32_t s[] = {1, 2, 3, 4, 5, 6}, sub[]= {3, 4, 5};

	ASSERT_TRUE(zStringContains<std::uint32_t>({s, 6}, {sub, 3}));
}

TEST(ZstringTestSuite, test2) {
	static std::uint32_t s[] = {1, 2, 3, 4, 5, 6}, sub[]= {3};

	ASSERT_TRUE(zStringContains<std::uint32_t>({s, 6}, {sub, 1}));
}

TEST(ZstringTestSuite, test3) {
	static std::uint32_t s[] = {1, 2, 3, 4, 5, 6}, sub[]= {3, 5, 4};

	ASSERT_FALSE(zStringContains<std::uint32_t>({s, 6}, {sub, 3}));
}
