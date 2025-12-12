#include "Vector.hpp"
#include <gtest/gtest.h>

TEST(VectorTestSuite, Constructors1) {
	IR::Vector<int> v;
	ASSERT_EQ(v.size(), 0);
}

TEST(VectorTestSuite, Constructors2) {
	IR::Vector<int> v(5);
	ASSERT_EQ(v.size(), 5);
	for(size_t i = 0; i < 5; ++i) 
		ASSERT_EQ(v[i], 0);
}

TEST(VectorTestSuite, Constructors3) {
	IR::Vector<int> v(5, 7);
	ASSERT_EQ(v.size(), 5);
	for(size_t i = 0; i < 5; ++i) 
		ASSERT_EQ(v[i], 7);
}

TEST(VectorTestSuite, Constructors4AndGetting) {
	int data[5] = {1, 2, 3, 4, 5};
	IR::Vector<int> v(data, 5);

	ASSERT_EQ(v.size(), 5);

	for(size_t i = 0; i < 5; ++i) 
		ASSERT_EQ(data[i], v[i]);
}

TEST(VectorTestSuite, Constructors5) {
	int data[5] = {1, 2, 3, 4, 5};
	IR::Vector<int> v(data, 5);

	IR::Vector<int> v2(std::move(v));
	ASSERT_EQ(v2.size(), 5);

	for(size_t i = 0; i < 5; ++i) 
		ASSERT_EQ(data[i], v2[i]);
}

TEST(VectorTestSuite, Constructors6) {
	int data[5] = {1, 2, 3, 4, 5};
	IR::Vector<int> v(data, 5);

	IR::Vector<int> v2(v);
	ASSERT_EQ(v2.size(), v.size());

	for(size_t i = 0; i < 5; ++i) 
		ASSERT_EQ(v[i], v2[i]);
}

TEST(VectorTestSuite, Inserting1) {
	int data[5] = {1, 2, 3, 4, 5};
	IR::Vector<int> v;
	for(size_t i = 0; i < 5; ++i) 
		v.insert(v.end(), data[i]);

	ASSERT_EQ(v.size(), 5);

	for(size_t i = 0; i < 5; ++i) 
		ASSERT_EQ(v[i], data[i]);
}

TEST(VectorTestSuite, Inserting2) {
	int data[5] = {1, 2, 3, 4, 5};
	IR::Vector<int> v;
	for(size_t i = 0; i < 5; ++i) 
		v.insert(v.begin(), data[i]);

	ASSERT_EQ(v.size(), 5);

	for(size_t i = 0; i < 5; ++i) 
		ASSERT_EQ(v[i], data[4 - i]);
}

TEST(VectorTestSuite, Iterating) {
	int data[5] = {1, 2, 3, 4, 5};
	IR::Vector<int> v(data, 5);

	for(size_t i = 0;auto & el : v) 
		ASSERT_EQ(el, data[i++]);
}

struct Person {
	std::string name;
	int age;
	bool operator==(const Person &) const = default;
};

static Person p[5] {
	{"Sane", 24},
	{"Sane2", 25},
	{"Sane3", 25},
	{"Sane4", 27},
	{"Sane5", 28},
};

TEST(VectorTestSuite, ComplexType) {
	IR::Vector<Person> v(p, 5);

	for(size_t i = 0; i < 5; ++i) 
		ASSERT_EQ(p[i], v[i]);
}
