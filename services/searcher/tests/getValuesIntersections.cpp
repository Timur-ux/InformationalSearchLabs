#include <cstdint>
#include <gtest/gtest.h>
#include <vector>

#include "handlers/boolean.hpp"

TEST(BooleanSearchTestSuite, getValuesIntersectionTest) {
	std::vector<std::pair<std::uint32_t, std::vector<std::uint32_t>>> kv{
		{1, {1, 2, 3, 11, 31, 45, 173}},
		{2, {2, 31, 54, 101}}
	};
	std::vector<std::uint32_t> expected{2, 31};

	std::vector<uint32_t> intersection = searchService::BooleanSearchHandler::getValuesIntersection(kv);

	ASSERT_EQ(intersection.size(), expected.size());
	for(size_t i = 0; i < expected.size(); ++i) 
		ASSERT_EQ(intersection[i], expected[i]);
}
