#include "Trie.hpp"
#include <gtest/gtest.h>
#include <type_traits>
#include <wchar.h>
static inline size_t wstrlen(const wchar_t * s) {
	size_t result = 0;
	while(*s != 0) 
		++result, ++s;
	return result;
}
class TrieTestSuite : public testing::Test {
	public:
	static constexpr size_t nStrings = 6;
	static constexpr const wchar_t *strings[nStrings] = {
		L"Some",
		L"Текст",
		L"Длинная дорога",
		L"Me",
		L"I",
		L"Docker desctop"
	};
	IR::Trie<wchar_t, unsigned int> *trie;
	void SetUp() {
		trie = new IR::Trie<wchar_t, unsigned int>;
		for(size_t i = 0; i < nStrings; ++i) 
			trie->insert({strings[i], wstrlen(strings[i])}, i);
	}
	void TearDown() {
		delete trie;
	}
};

TEST_F(TrieTestSuite, CheckIn) {
	for(size_t i = 0; i < nStrings; ++i) 
		ASSERT_EQ(trie->get(strings[i], wstrlen(strings[i])), i);
}

TEST_F(TrieTestSuite, SerDeserialization) {
	constexpr const char * pathToFile = "/tmp/temporary-trie-data-file";
	trie->serialize(pathToFile);
	auto trie2 = new std::remove_cvref_t<decltype(*trie)>;
	trie2->deserialize(pathToFile);

	ASSERT_EQ(trie->size(), trie2->size());
	for(size_t i = 0; i < nStrings; ++i) 
		ASSERT_EQ(trie2->get(strings[i], wstrlen(strings[i])), i);
}
