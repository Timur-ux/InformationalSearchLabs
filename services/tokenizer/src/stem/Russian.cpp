// --------------------------------------------------------------------------------------------------------
// |  http://snowball.tartarus.org/algorithms/russian/stemmer.html -- отсюда
// взял материалы по стеммингу  |
// --------------------------------------------------------------------------------------------------------
#include "stem/Russian.hpp"
#include "Vector.hpp"
#include "stem/Stemmer.hpp"
#include <algorithm>
#include <string>

namespace SERVICE_NAMESPACE {
bool isRussian(const wchar_t *s) {
  size_t len = 0, russians = 0;
  while (*s != 0) {
    if ((*s >= L'а' && *s <= L'я') || (*s >= L'А' && *s <= L'Я'))
      ++russians;
    ++len, ++s;
  }

  return russians > len / 2;
}
void RussianStemAction::operator()(std::wstring &word) {
  if (word.size() <= 2)
    return;
  std::transform(word.cbegin(), word.cend(), word.begin(),
                 impl::fullWidthToNarrow);
  std::transform(word.cbegin(), word.cend(), word.begin(), [](wchar_t ch) {
    return ch == L'ё' ? L'е' : (ch == L'Ё' ? L'Е' : ch);
  });

  // not russian word, skip
  if (!isRussian(word.c_str())) {
    if (next_)
      (*next_)(word);
    return;
  }

  setupR1R2RV(word, vowels_);
  step1(word);
  step2(word);
  step3(word);
  step4(word);
}

void RussianStemAction::step1(std::wstring &word) {
  if (removeIfInRV(word, preEndings2, L"ывшись", 6) ||
      removeIfInRV(word, preEndings2, L"ившись", 6) ||
      removeIfInRV(word, preEndings1, L"вшись", 5) ||
      removeIfInRV(word, preEndings2, L"ывши", 4) ||
      removeIfInRV(word, preEndings2, L"ивши", 4) ||
      removeIfInRV(word, preEndings1, L"вши", 3) ||
      removeIfInRV(word, preEndings2, L"ыв", 2) ||
      removeIfInRV(word, preEndings2, L"ив", 2) ||
      removeIfInRV(word, preEndings1, L"в", 1)) {
    updateRSectors(word);
  } else {
    const wchar_t *ends = nullptr;
    if (isWordEndOn(word, L"сь", 2))
      ends = L"сь";
    else if (isWordEndOn(word, L"ся", 2))
      ends = L"ся";

    if (ends)
      word.erase(word.size() - 2);
    if (
        // adj
        removeIfInRV(word, preEndings2, L"ими", 3) ||
        removeIfInRV(word, preEndings2, L"ыми", 3) ||
        removeIfInRV(word, preEndings2, L"его", 3) ||
        removeIfInRV(word, preEndings2, L"ого", 3) ||
        removeIfInRV(word, preEndings2, L"ему", 3) ||
        removeIfInRV(word, preEndings2, L"ому", 3) ||
        removeIfInRV(word, preEndings2, L"ее", 2) ||
        removeIfInRV(word, preEndings2, L"ие", 2) ||
        removeIfInRV(word, preEndings2, L"ые", 2) ||
        removeIfInRV(word, preEndings2, L"ое", 2) ||
        removeIfInRV(word, preEndings2, L"ей", 2) ||
        removeIfInRV(word, preEndings2, L"ий", 2) ||
        removeIfInRV(word, preEndings2, L"ый", 2) ||
        removeIfInRV(word, preEndings2, L"ой", 2) ||
        removeIfInRV(word, preEndings2, L"ем", 2) ||
        removeIfInRV(word, preEndings2, L"им", 2) ||
        removeIfInRV(word, preEndings2, L"ым", 2) ||
        removeIfInRV(word, preEndings2, L"ом", 2) ||
        removeIfInRV(word, preEndings2, L"их", 2) ||
        removeIfInRV(word, preEndings2, L"ых", 2) ||
        removeIfInRV(word, preEndings2, L"ую", 2) ||
        removeIfInRV(word, preEndings2, L"юю", 2) ||
        removeIfInRV(word, preEndings2, L"ая", 2) ||
        removeIfInRV(word, preEndings2, L"яя", 2) ||
        removeIfInRV(word, preEndings2, L"ою", 2) ||
        removeIfInRV(word, preEndings2, L"ею", 2)) {
      updateRSectors(word);

      // check for additional participle
      if (removeIfInRV(word, preEndings2, L"ивш", 3) ||
          removeIfInRV(word, preEndings2, L"ывш", 3) ||
          removeIfInRV(word, preEndings2, L"ующ", 3) ||
          removeIfInRV(word, preEndings1, L"ем", 2) ||
          removeIfInRV(word, preEndings1, L"нн", 2) ||
          removeIfInRV(word, preEndings1, L"вш", 2) ||
          removeIfInRV(word, preEndings1, L"ющ", 2) ||
          removeIfInRV(word, preEndings1, L"щ", 1)) {
        updateRSectors(word);
      }
    } else if (
        // verb
        removeIfInRV(word, preEndings2, L"ейте", 4) ||
        removeIfInRV(word, preEndings2, L"уйте", 4) ||
        removeIfInRV(word, preEndings1, L"ете", 3) ||
        removeIfInRV(word, preEndings1, L"йте", 3) ||
        removeIfInRV(word, preEndings1, L"ешь", 3) ||
        removeIfInRV(word, preEndings1, L"нно", 3) ||
        removeIfInRV(word, preEndings2, L"ила", 3) ||
        removeIfInRV(word, preEndings2, L"ыла", 3) ||
        removeIfInRV(word, preEndings2, L"ена", 3) ||
        removeIfInRV(word, preEndings2, L"ите", 3) ||
        removeIfInRV(word, preEndings2, L"или", 3) ||
        removeIfInRV(word, preEndings2, L"ыли", 3) ||
        removeIfInRV(word, preEndings2, L"ило", 3) ||
        removeIfInRV(word, preEndings2, L"ыло", 3) ||
        removeIfInRV(word, preEndings2, L"ено", 3) ||
        removeIfInRV(word, preEndings2, L"ует", 3) ||
        removeIfInRV(word, preEndings2, L"уют", 3) ||
        removeIfInRV(word, preEndings2, L"ены", 3) ||
        removeIfInRV(word, preEndings2, L"ить", 3) ||
        removeIfInRV(word, preEndings2, L"ыть", 3) ||
        removeIfInRV(word, preEndings2, L"ишь", 3) ||
        removeIfInRV(word, preEndings1, L"ла", 2) ||
        removeIfInRV(word, preEndings1, L"на", 2) ||
        removeIfInRV(word, preEndings1, L"ли", 2) ||
        removeIfInRV(word, preEndings1, L"ем", 2) ||
        removeIfInRV(word, preEndings1, L"ло", 2) ||
        removeIfInRV(word, preEndings1, L"но", 2) ||
        removeIfInRV(word, preEndings1, L"ет", 2) ||
        removeIfInRV(word, preEndings1, L"ют", 2) ||
        removeIfInRV(word, preEndings1, L"ны", 2) ||
        removeIfInRV(word, preEndings1, L"ть", 2) ||
        removeIfInRV(word, preEndings2, L"ей", 2) ||
        removeIfInRV(word, preEndings2, L"уй", 2) ||
        removeIfInRV(word, preEndings2, L"ил", 2) ||
        removeIfInRV(word, preEndings2, L"ыл", 2) ||
        removeIfInRV(word, preEndings2, L"им", 2) ||
        removeIfInRV(word, preEndings2, L"ым", 2) ||
        removeIfInRV(word, preEndings2, L"ен", 2) ||
        removeIfInRV(word, preEndings2, L"ят", 2) ||
        removeIfInRV(word, preEndings2, L"ыт", 2) ||
        removeIfInRV(word, preEndings2, L"ит", 2) ||
        removeIfInRV(word, preEndings2, L"ую", 2) ||
        removeIfInRV(word, preEndings1, L"й", 1) ||
        removeIfInRV(word, preEndings1, L"л", 1) ||
        removeIfInRV(word, preEndings1, L"н", 1) ||
        removeIfInRV(word, preEndings2, L"ю", 1) ||

        // noun
        removeIfInRV(word, preEndings2, L"иями", 4) ||
        removeIfInRV(word, preEndings2, L"ями", 3) ||
        removeIfInRV(word, preEndings2, L"ами", 3) ||
        removeIfInRV(word, preEndings2, L"ией", 3) ||
        removeIfInRV(word, preEndings2, L"иям", 3) ||
        removeIfInRV(word, preEndings2, L"ием", 3) ||
        removeIfInRV(word, preEndings2, L"иях", 3) ||
        removeIfInRV(word, preEndings2, L"ев", 2) ||
        removeIfInRV(word, preEndings2, L"ов", 2) ||
        removeIfInRV(word, preEndings2, L"ие", 2) ||
        removeIfInRV(word, preEndings2, L"ье", 2) ||
        removeIfInRV(word, preEndings2, L"еи", 2) ||
        removeIfInRV(word, preEndings2, L"ии", 2) ||
        removeIfInRV(word, preEndings2, L"ей", 2) ||
        removeIfInRV(word, preEndings2, L"ой", 2) ||
        removeIfInRV(word, preEndings2, L"ий", 2) ||
        removeIfInRV(word, preEndings2, L"ям", 2) ||
        removeIfInRV(word, preEndings2, L"ем", 2) ||
        removeIfInRV(word, preEndings2, L"ам", 2) ||
        removeIfInRV(word, preEndings2, L"ом", 2) ||
        removeIfInRV(word, preEndings2, L"ах", 2) ||
        removeIfInRV(word, preEndings2, L"ях", 2) ||
        removeIfInRV(word, preEndings2, L"ию", 2) ||
        removeIfInRV(word, preEndings2, L"ью", 2) ||
        removeIfInRV(word, preEndings2, L"ия", 2) ||
        removeIfInRV(word, preEndings2, L"ья", 2) ||
        removeIfInRV(word, preEndings2, L"я", 1) ||
        removeIfInRV(word, preEndings2, L"ю", 1) ||
        removeIfInRV(word, preEndings2, L"ь", 1) ||
        removeIfInRV(word, preEndings2, L"ы", 1) ||
        removeIfInRV(word, preEndings2, L"у", 1) ||
        removeIfInRV(word, preEndings2, L"о", 1) ||
        removeIfInRV(word, preEndings2, L"й", 1) ||
        removeIfInRV(word, preEndings2, L"и", 1) ||
        removeIfInRV(word, preEndings2, L"а", 1) ||
        removeIfInRV(word, preEndings2, L"е", 1)) {
      updateRSectors(word);
    } else if (ends)
      word += ends;
  }
}

void RussianStemAction::step2(std::wstring &word) {
  if (word[word.size() - 1] == L'и')
    word.erase(word.size() - 1);
}

void RussianStemAction::step3(std::wstring &word) {
  if (word.size() - r2() >= 3 && isWordEndOn(word, L"ост", 3))
    word.erase(word.size() - 3);
  else if (word.size() - r2() >= 4 && isWordEndOn(word, L"ость", 4))
    word.erase(word.size() - 4);
  updateRSectors(word);
}

void RussianStemAction::step4(std::wstring &word) {
  if (word.size() > rv() && this->isWordEndOn(word, L"нн", 2))
    word.pop_back();
  else if (removeIfInRV(word, preEndings2, L"ейше", 4) ||
           removeIfInRV(word, preEndings2, L"ейш", 3)) {
    updateRSectors(word);
    if (word.size() > rv() && this->isWordEndOn(word, L"нн", 2))
      word.pop_back(), updateRSectors(word);
  } else if (word.size() > 0 && word[word.size() - 1] == L'ь')
    word.pop_back(), updateRSectors(word);
}

} // namespace SERVICE_NAMESPACE
