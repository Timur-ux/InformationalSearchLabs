#include <cstring>
#include <gtest/gtest.h>
#include "algo.hpp"

TEST(lltostringTestSuite, Zero) {
	long n = 0;

	const char * s = IR::algo::lltostring(n);

	ASSERT_EQ(strcmp(s, "0"), 0);
}

TEST(lltostringTestSuite, Small) {
	long n = 123;

	const char * s = IR::algo::lltostring(n);

	ASSERT_EQ(strcmp(s, "123"), 0);
}

TEST(lltostringTestSuite, Big) {
	long n = 123123123123123;

	const char * s = IR::algo::lltostring(n);

	ASSERT_EQ(strcmp(s, "123123123123123"), 0);
}

TEST(lltostringTestSuite, Negative) {
	long n = -100;

	const char * s = IR::algo::lltostring(n);

	ASSERT_EQ((void*)s, (void*)NULL);
}

