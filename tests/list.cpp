#include "List.hpp"
#include <gtest/gtest.h>

TEST(ListTestSuite, Constructor1) {
  IR::List<int> l;
  ASSERT_EQ(l.size(), 0);
}

TEST(ListTestSuite, PushBack) {
  IR::List<int> l;
  for (int i = 0; i < 10; ++i)
    l.push_back(i);

  ASSERT_EQ(l.size(), 10);
  for (size_t i = 0; auto &el : l)
    ASSERT_EQ(i++, el);
}

TEST(ListTestSuite, Constructor2) {
  IR::List<int> l;
  for (int i = 0; i < 10; ++i)
    l.push_back(i);
  IR::List<int> l2(l);

  ASSERT_TRUE(l.size() == l2.size() && l.size() == 10);
  for (auto it = std::begin(l), it2 = std::begin(l2); it != std::end(l);
       ++it, ++it2)
    ASSERT_EQ(*it, *it2);
}

TEST(ListTestSuite, Constructor3) {
  IR::List<int> l;
  for (int i = 0; i < 10; ++i)
    l.push_back(i);
  IR::List<int> l2(std::move(l));

  ASSERT_EQ(l2.size(), 10);
  for (size_t i = 0; auto &el : l2)
    ASSERT_EQ(i++, el);
}

TEST(ListTestSuite, EmplaceBack) {
  IR::List<int> l;
  l.emplace_back(1);
  l.push_back(2);
  l.emplace_back(3);

  ASSERT_EQ(l.size(), 3);
  for (int i = 1; auto &el : l)
    ASSERT_EQ(i++, el);
}

TEST(ListTestSuite, Insert) {
  IR::List<int> l;
  for (int i = 0; i < 100; ++i)
    l.insert(l.begin(), i);

  ASSERT_EQ(l.size(), 100);
  for (int i = 0; auto &el : l)
    ASSERT_EQ(100 - (++i), el);
}

struct Person {
  std::string name;
  int age;
  bool operator==(const Person &) const = default;
};

static Person p[5]{
    {"Sane", 24}, {"Sane2", 25}, {"Sane3", 25}, {"Sane4", 27}, {"Sane5", 28},
};

TEST(ListTestSuite, EmplaceComplex) {
  IR::List<Person> l;
  for (int i = 0; i < 5; ++i)
    l.emplace(l.begin(), p[i].name, p[i].age);

  ASSERT_EQ(l.size(), 5);
  for (int i = 0; auto &el : l)
    ASSERT_EQ(p[5 - (++i)], el);
}

TEST(ListTestSuite, Remove1) {
  IR::List<Person> l;
  for (int i = 0; i < 5; ++i)
    l.emplace_back(p[i].name, p[i].age);

  auto l2(l);
  Person first = *l2.begin();
  l2.emplace(l2.remove(l2.begin()), first);

  ASSERT_EQ(l2.size(), l.size());
  for (auto it = std::begin(l), it2 = std::begin(l2); it != std::end(l);
       ++it, ++it2) 
    ASSERT_EQ(*it, *it2);
}

TEST(ListTestSuite, Remove2) {
  IR::List<Person> l;
  for (int i = 0; i < 5; ++i)
    l.emplace_back(p[i].name, p[i].age);
	for(int i = 0; i < 2; ++i) 
		l.remove(l.begin());

	for(size_t i = 2;auto &el : l) 
		ASSERT_EQ(p[i++], el);
}
