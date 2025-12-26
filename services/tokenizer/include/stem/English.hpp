#ifndef ENGLISH_STEMMER_HPP_
#define ENGLISH_STEMMER_HPP_
#include "stem/Stemmer.hpp"
#include <string>
#include <string_view>
#pragma once
namespace SERVICE_NAMESPACE {
class EnglishStemAction : public StemAction<std::wstring> {
  const wchar_t *vowels_ = L"aeiouy";
	const wchar_t *doubles_ = L"bdfgmnprt";
	const wchar_t *liEndings_ = L"cdeghkmnrt";

	bool isEndOnShortSyllable(std::wstring_view word);
	bool isShortWord(std::wstring_view word);


	void step0(std::wstring &word);
	void step1a(std::wstring &word);
	void step1b(std::wstring &word);
	void step1c(std::wstring &word);
	void step2(std::wstring &word);
	void step3(std::wstring &word);
	void step4(std::wstring &word);
	void step5(std::wstring &word);
public:
  void operator()(std::wstring &word) override final;
};
} // namespace SERVICE_NAMESPACE
#endif // !ENGLISH_STEMMER_HPP_
