#ifndef RUSSIAN_STEMMER_HPP_
#define RUSSIAN_STEMMER_HPP_
#include "Stemmer.hpp"
#include <string>
#pragma once
namespace SERVICE_NAMESPACE {
class RussianStemAction : public StemAction<std::wstring> {
  const wchar_t *vowels_ = L"аяеэыиёуо";
  std::vector<std::pair<const wchar_t *, size_t>> preEndings1{{L"а", 1},
                                                              {L"я", 1}},
      preEndings2{{L"", 0}};
	void step1(std::wstring &word);
	void step2(std::wstring &word);
	void step3(std::wstring &word);
	void step4(std::wstring &word);
public:
  void operator()(std::wstring &word) override final;
};
} // namespace SERVICE_NAMESPACE
#endif // !RUSSIAN_STEMMER_HPP_
