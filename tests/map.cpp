#include "Map.hpp"
#include <gtest/gtest.h>

TEST(MapTestSuite, InsertEraseTest) {
	std::map<int, int> m1;
	IR::Map<int, int> m2;
	size_t n = 10000;

	for(size_t i = 0; i < n; ++i) {
		int val = rand();
		m1[i] = val, m2[i] = val;
	}

	ASSERT_EQ(m1.size(), m2.size());

	// m2.print(std::cerr);
	for(size_t i = 0; i < n/2; ++i) {
		int val = rand() % n;
		// std::cout << "------"<< val << "-----\n";
		if(m1.contains(val))
			m1.erase(val), m2.remove(val);
		// m2.print(std::cerr);
	}

	ASSERT_EQ(m1.size(), m2.size());
	auto it1 = m1.begin();
	auto it2 = m2.begin();
	for(size_t i = 0; i < m1.size(); ++i) {
		ASSERT_EQ(it1->first, it2->key);
		ASSERT_EQ(it1->second, it2->value);
		++it1, ++it2;
	}
}

