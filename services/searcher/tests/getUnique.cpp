#include "utils.hpp"
#include <algorithm>
#include <cstdint>
#include <gtest/gtest.h>
#include <vector>

TEST(GetUniqueTestSuite, baseCase) {
  std::vector<std::uint32_t> values{1, 2, 3, 1, 2, 3, 0, 3, 3, 2, 1},
      expected{0, 1, 2, 3};

  std::vector<std::uint32_t> uniques = getUnique(values);
  std::sort(std::begin(uniques), std::end(uniques));

  ASSERT_EQ(uniques.size(), expected.size());
  for (size_t i = 0; i < uniques.size(); ++i)
    ASSERT_EQ(uniques[i], expected[i]);
}

TEST(GetUniqueTestSuite, complexCase) {
  std::vector<std::vector<std::uint32_t>> values{
      {0, 1, 2, 3, 3, 3}, {1, 2, 3, 1}, {37, 2, 3, 2}};
  std::vector<std::uint32_t> expected{0, 1, 2, 3, 37};

  std::vector<std::uint32_t> uniques = getUnique<std::vector<std::uint32_t>>(
      values, [](const std::vector<std::uint32_t> &el) { return el; });
  std::sort(std::begin(uniques), std::end(uniques));

  ASSERT_EQ(uniques.size(), expected.size());
  for (size_t i = 0; i < uniques.size(); ++i)
    ASSERT_EQ(uniques[i], expected[i]);
}
