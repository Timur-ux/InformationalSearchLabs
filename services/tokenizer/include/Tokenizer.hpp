#ifndef TOKENIZER_HPP_
#define TOKENIZER_HPP_
#include "Trie.hpp"
#include "Vector.hpp"
#include <cstdint>
#include <filesystem>
#include <string>
#pragma once
namespace SERVICE_NAMESPACE {
class Tokenizer {
	static constexpr const wchar_t* wpatternText = L"(([A-Z]|[a-z]|[А-Я]|[а-я])+)|([[:punct:]])|([0-9])";
	static constexpr const char* patternText = "(([A-Z]|[a-z]|[А-Я]|[а-я])+)|([[:punct:]])|([0-9])";
	Tokenizer();
	~Tokenizer();
	std::filesystem::path storagePath_;
	IR::Trie<wchar_t, std::uint32_t> forward_;
	IR::Vector<std::wstring> backward_;

	void load();
	void save();
public:
	static Tokenizer & instance();
	IR::Vector<std::uint32_t> tokenize(const std::wstring &s);
	std::wstring detokenize(const IR::Vector<std::uint32_t> &tokens);
};
} // namespace SERVICE_NAMESPACE
#endif // !TOKENIZER_HPP_
