#include "stem/English.hpp"
#include "algo.hpp"
#include "stem/Stemmer.hpp"
#include <algorithm>
#include <string>
#include <string_view>
#include <sys/socket.h>

namespace SERVICE_NAMESPACE {

void EnglishStemAction::operator()(std::wstring &word) {
  if (word.size() <= 2)
    return;

  std::transform(word.cbegin(), word.cend(), word.begin(),
                 impl::fullWidthToNarrow);

  size_t englishes = 0;
  for (wchar_t c : word)
    if ((c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z'))
      ++englishes;
  // not english word, skip
  if (englishes < word.size() / 2) {
    if (next_)
      (*next_)(word);
    return;
  }

  if (word[0] == L'\'')
    word.erase(word.begin());

  if (word[0] == L'y')
    (word[0] = L'Y');
  else {
    size_t ind = word.find(L'y', 1);
    while (ind != std::wstring::npos) {
      if (IR::algo::contains<wchar_t>(word[ind - 1], vowels_))
        word[ind] = L'Y';
      ind = word.find(L'y', ind + 1);
    }
  }

  setupR1R2RV(word, vowels_);

  step0(word);
  step1a(word);
  step1b(word);
  step1c(word);
  step2(word);
  step3(word);
  step4(word);
  step5(word);

  size_t ind = word.find(L'Y');
  while (ind != std::wstring::npos) {
    word[ind] = L'y';
    ind = word.find(L'Y', ind + 1);
  }
}

bool EnglishStemAction::isEndOnShortSyllable(std::wstring_view word) {
  if (word.size() >= 3 &&
      word.find_first_not_of(vowels_, word.size() - 3) == word.size() - 3 &&
      word.find_first_of(vowels_, word.size() - 2) == word.size() - 2 &&
      word.find_first_not_of(vowels_, word.size() - 1) == word.size() - 1 &&
      word[word.size() - 1] != L'w' && word[word.size() - 1] != L'x' &&
      word[word.size() - 1] != L'Y')
    return true;

  if (word.size() == 2 && word.find_first_of(vowels_, 0) == 0 &&
      word.find_first_not_of(vowels_, 1) == 1)
    return true;

  return false;
}

bool EnglishStemAction::isShortWord(std::wstring_view word) {
  return r1() == word.size() && isEndOnShortSyllable(word);
}

void EnglishStemAction::step0(std::wstring &word) {
  size_t ind;
  if ((ind = word.rfind(L"'s'", 0)) != std::wstring::npos)
    word.erase(ind, 3);
  else if ((ind = word.rfind(L"s'", 0)) != std::wstring::npos)
    word.erase(ind, 2);
  else if ((ind = word.rfind(L'\'', 0)) != std::wstring::npos)
    word.erase(ind, 1);
  else
    return;
  updateRSectors(word);
}

void EnglishStemAction::step1a(std::wstring &word) {
	size_t ind;
  if (isWordEndOn(word, L"sses", 4))
    word.erase(word.size() - 2);
  else if (isWordEndOn(word, L"ies", 3) || isWordEndOn(word, L"ied", 3)) {
    word.pop_back();
    if (word.size() > 3)
      word.pop_back();
  } else if (word[word.size() - 1] == L's' &&
             (ind = word.find_first_of(vowels_, 0)) != std::wstring::npos && ind != word.size() - 2
						 && !IR::algo::contains(word[word.size() - 2], L"usUS"))
    word.pop_back();
  else
    return;
  updateRSectors(word);
}

void EnglishStemAction::step1b(std::wstring &word) {
  if (isWordEndOn(word, L"eed", 3) && word.size() >= r1() + 3)
    return word.pop_back(), updateRSectors(word);
  if (isWordEndOn(word, L"eedly", 5) && word.size() >= r1() + 5)
    return word.erase(word.size() - 3), updateRSectors(word);

  size_t ind = word.find_first_of(vowels_);
  if (ind == std::wstring::npos)
    return;

  bool wasRemove = false;
  if (isWordEndOn(word, L"ingly", 5) && ind < word.size() - 5)
    word.erase(word.size() - 5), wasRemove = true;
  else if (isWordEndOn(word, L"edly", 4) && ind < word.size() - 4)
    word.erase(word.size() - 4), wasRemove = true;
  else if (isWordEndOn(word, L"ing", 3) && ind < word.size() - 3)
    word.erase(word.size() - 3), wasRemove = true;
  else if (isWordEndOn(word, L"ed", 2) && ind < word.size() - 2)
    word.erase(word.size() - 2), wasRemove = true;

  if (!wasRemove)
    return;

  if (isWordEndOn(word, L"at", 2) || isWordEndOn(word, L"bl", 2) ||
      isWordEndOn(word, L"iz", 2) || isShortWord(word))
    return word.push_back(L'e'), updateRSectors(word);

  size_t i = 0;
  while (doubles_[i] != 0 && !(word[word.size() - 1] == doubles_[i] &&
                               word[word.size() - 2] == doubles_[i]))
    ++i;
  if (doubles_[i] != 0)
    word.pop_back(), updateRSectors(word);
}

void EnglishStemAction::step1c(std::wstring &word) {
  if (word.size() > 2 && IR::algo::contains(word[word.size() - 1], L"yY") &&
      !IR::algo::contains(word[word.size() - 2], vowels_))
    word[word.size() - 1] = L'i';
}

void EnglishStemAction::step2(std::wstring &word) {
  if (isWordEndOn(word, L"iveness", 7) && word.size() >= r1() + 7)
    word.erase(word.size() - 4);
  else if (isWordEndOn(word, L"ousness", 7) && word.size() >= r1() + 7)
    word.erase(word.size() - 4);
  else if (isWordEndOn(word, L"fulness", 7) && word.size() >= r1() + 7)
    word.erase(word.size() - 4);
  else if (isWordEndOn(word, L"ational", 7) && word.size() >= r1() + 7)
    word.erase(word.size() - 5), word.push_back(L'i');
  else if (isWordEndOn(word, L"ization", 7) && word.size() >= r1() + 7)
    word.erase(word.size() - 5), word.push_back(L'e');
  else if (isWordEndOn(word, L"biliti", 6) && word.size() >= r1() + 6)
    word.erase(word.size() - 5), word.push_back(L'l'), word.push_back(L'e');
  else if (isWordEndOn(word, L"lessli", 6) && word.size() >= r1() + 6)
    word.erase(word.size() - 2);
  else if (isWordEndOn(word, L"fulli", 5) && word.size() >= r1() + 5)
    word.erase(word.size() - 2);
  else if (isWordEndOn(word, L"iviti", 5) && word.size() >= r1() + 5)
    word.erase(word.size() - 3), word.push_back(L'e');
  else if (isWordEndOn(word, L"alism", 5) && word.size() >= r1() + 5)
    word.erase(word.size() - 3);
  else if (isWordEndOn(word, L"aliti", 5) && word.size() >= r1() + 5)
    word.erase(word.size() - 3);
  else if (isWordEndOn(word, L"entli", 5) && word.size() >= r1() + 5)
    word.erase(word.size() - 2);
  else if (isWordEndOn(word, L"ation", 5) && word.size() >= r1() + 5)
    word.erase(word.size() - 3), word.push_back(L'e');
  else if (isWordEndOn(word, L"ousli", 5) && word.size() >= r1() + 5)
    word.erase(word.size() - 2);
  else if (isWordEndOn(word, L"alli", 4) && word.size() >= r1() + 4)
    word.erase(word.size() - 2);
  else if (isWordEndOn(word, L"enci", 4) && word.size() >= r1() + 4)
    word.erase(word.size() - 1), word.push_back(L'e');
  else if (isWordEndOn(word, L"izer", 4) && word.size() >= r1() + 4)
    word.erase(word.size() - 1);
  else if (isWordEndOn(word, L"anci", 4) && word.size() >= r1() + 4)
    word.erase(word.size() - 1), word.push_back(L'e');
  else if (isWordEndOn(word, L"abli", 4) && word.size() >= r1() + 4)
    word.erase(word.size() - 1), word.push_back(L'e');
  else if (isWordEndOn(word, L"ator", 4) && word.size() >= r1() + 4)
    word.erase(word.size() - 2), word.push_back(L'e');
  else if (isWordEndOn(word, L"bli", 3) && word.size() >= r1() + 3)
    word.erase(word.size() - 1), word.push_back(L'e');
  else if (word.size() > 3 && word[word.size() - 4] == 'l' &&
           isWordEndOn(word, L"ogi", 3) && word.size() >= r1() + 3)
    word.erase(word.size() - 1);
  else if (word.size() > 2 && word.size() >= r1() + 2 &&
           isWordEndOn(word, L"li", 2) &&
           IR::algo::contains(word[word.size() - 3], liEndings_))
    word.erase(word.size() - 2);
  else
    return;
  updateRSectors(word);
}
void EnglishStemAction::step3(std::wstring &word) {

  if (isWordEndOn(word, L"tional", 6) && word.size() >= r1() + 6)
    word.erase(word.size() - 2);
  else if (isWordEndOn(word, L"ation", 5) && word.size() >= r1() + 5)
    word.erase(word.size() - 3), word.push_back(L'e');
  else if (isWordEndOn(word, L"alize", 5) && word.size() >= r1() + 5)
    word.erase(word.size() - 3);
  else if (isWordEndOn(word, L"icate", 5) && word.size() >= r1() + 5)
    word.erase(word.size() - 3);
  else if (isWordEndOn(word, L"iciti", 5) && word.size() >= r1() + 5)
    word.erase(word.size() - 3);
  else if (isWordEndOn(word, L"ative", 5) && word.size() >= r2() + 5)
    word.erase(word.size() - 5);
  else if (isWordEndOn(word, L"ical", 4) && word.size() >= r1() + 4)
    word.erase(word.size() - 2);
  else if (isWordEndOn(word, L"ness", 4) && word.size() >= r1() + 4)
    word.erase(word.size() - 4);
  else if (isWordEndOn(word, L"ful", 3) && word.size() >= r1() + 3)
    word.erase(word.size() - 3);
  else
    return;
  updateRSectors(word);
}
void EnglishStemAction::step4(std::wstring &word) {
  if (isWordEndOn(word, L"ement", 5) && word.size() >= r2() + 5)
    word.erase(word.size() - 5);
  else if (isWordEndOn(word, L"ance", 4) && word.size() >= r2() + 4)
    word.erase(word.size() - 4);
  else if (isWordEndOn(word, L"ence", 4) && word.size() >= r2() + 4)
    word.erase(word.size() - 4);
  else if (isWordEndOn(word, L"able", 4) && word.size() >= r2() + 4)
    word.erase(word.size() - 4);
  else if (isWordEndOn(word, L"ible", 4) && word.size() >= r2() + 4)
    word.erase(word.size() - 4);
  else if (isWordEndOn(word, L"ment", 4) && word.size() >= r2() + 4)
    word.erase(word.size() - 4);
  else if (isWordEndOn(word, L"ant", 3) && word.size() >= r2() + 3)
    word.erase(word.size() - 3);
  else if (isWordEndOn(word, L"ent", 3) && word.size() >= r2() + 3)
    word.erase(word.size() - 3);
  else if (isWordEndOn(word, L"ism", 3) && word.size() >= r2() + 3)
    word.erase(word.size() - 3);
  else if (isWordEndOn(word, L"ate", 3) && word.size() >= r2() + 3)
    word.erase(word.size() - 3);
  else if (isWordEndOn(word, L"iti", 3) && word.size() >= r2() + 3)
    word.erase(word.size() - 3);
  else if (isWordEndOn(word, L"ous", 3) && word.size() >= r2() + 3)
    word.erase(word.size() - 3);
  else if (isWordEndOn(word, L"ive", 3) && word.size() >= r2() + 3)
    word.erase(word.size() - 3);
  else if (isWordEndOn(word, L"ize", 3) && word.size() >= r2() + 3)
    word.erase(word.size() - 3);
  else if (word.size() > 3 &&
           IR::algo::contains(word[word.size() - 4], L"st") &&
           isWordEndOn(word, L"ion", 3) && word.size() >= r2() + 3)
    word.erase(word.size() - 3);
  else if (isWordEndOn(word, L"al", 2) && word.size() >= r2() + 2)
    word.erase(word.size() - 2);
  else if (isWordEndOn(word, L"er", 2) && word.size() >= r2() + 2)
    word.erase(word.size() - 2);
  else if (isWordEndOn(word, L"ic", 2) && word.size() >= r2() + 2)
    word.erase(word.size() - 2);
  else
    return;
  updateRSectors(word);
}
void EnglishStemAction::step5(std::wstring &word) {
  if (word.size() > 3 && word[word.size() - 1] == L'e' &&
      (word.size() >= r2() + 1 ||
       (word.size() >= r1() + 1 &&
        !isEndOnShortSyllable({word.begin(), --word.end()}))))
    word.pop_back();
  else if (word.size() > 2 && word[word.size() - 1] == L'l' &&
           word.size() >= r2() + 1 && word[word.size() - 2] == L'l')
    word.pop_back();
  else
    return;
  updateRSectors(word);
}

} // namespace SERVICE_NAMESPACE
