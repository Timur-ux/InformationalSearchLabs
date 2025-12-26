#ifndef STEMMER_HPP_
#define STEMMER_HPP_
#include <cstddef>
#include <memory>
#include <string>
#include <vector>
#pragma once

namespace SERVICE_NAMESPACE {
template <typename TString = std::wstring> class StemAction {
  size_t r1_{0}, r2_{0}, rv_{0};

protected:
  std::unique_ptr<StemAction<TString>> next_{nullptr};
public:
  using stem_action_type = StemAction<TString>;
  virtual ~StemAction() = default;

  /**
   * @brief calc stem on word
   *
   * @param[in, out] word -- word to stem
   */
  virtual void operator()(TString &word) = 0;
  stem_action_type &insert(std::unique_ptr<stem_action_type> stemAction) {
    if (!stemAction)
      return *this;

    stemAction->next_ = std::move(next_);
    next_ = std::move(stemAction);
    return *this;
  }

  [[nodiscard]]
  size_t r1() const {
    return r1_;
  }
  [[nodiscard]]
  size_t r2() const {
    return r2_;
  }
  [[nodiscard]]
  size_t rv() const {
    return rv_;
  }

	void reset() {
		r1_ = r2_ = rv_ = 0;
	}

	void setupR1R2RV(TString & word, const wchar_t * vowels) {
		// rv
		rv_ = word.find_first_of(vowels);
		if(rv() == TString::npos) 
			rv_ = word.size();
		else
			++rv_;

		// r1
		r1_ = word.find_first_not_of(vowels, rv_);
		if(r1() == TString::npos) 
			r1_ = word.size();
		else
			++r1_;

		// r2
		auto vowel = word.find_first_of(vowels, r1_);
		if(vowel == TString::npos) 
			return (r2_ = word.size()), void();

		r2_ = word.find_first_not_of(vowels, vowel);
		if(r2_ == TString::npos) 
			r2_ = word.size();
		else
			++r2_;
	}

	/**
	 * @brief used for updating sections after stripping word
	 *
	 * @param word new word after strip
	 */
	void updateRSectors(TString & word) {
		if(rv_ > word.size()) 
			rv_ = word.size();
		if(r1_ > word.size()) 
			r1_ = word.size();
		if(r2_ > word.size()) 
			r2_ = word.size();
	}
	
	inline bool removeIfInRV(TString &word, const std::vector<std::pair<const wchar_t*, size_t>> &requiredPreEndings, const wchar_t *ending, size_t len) {
		if(rv() + len > word.size()) 
			return false;

		for(auto [preEnding, preEndingLen] : requiredPreEndings) { 
			bool isGood = true;
			if(rv() + preEndingLen + len > word.size()) 
				continue;
			size_t i = word.size() - preEndingLen - len;
			for(size_t j = 0; j < preEndingLen && isGood; ++j) 
				isGood = isGood && (word[i+j] == preEnding[j]); 

			for(size_t j = 0; j < len && isGood; ++j) 
				isGood = isGood && (word[i+preEndingLen+j] == ending[j]); 

			if(isGood) 
				return word.erase(word.size()-len), true;
		}

		return false;
	}

	inline bool isWordEndOn(TString & word, const wchar_t * ending, size_t n) {
		if(word.size() < n) 
			return false;
		for(size_t i = 0; i < n; ++i) 
			if(word[word.size() - n + i] != ending[i]) 
				return false;
		return true;
	}
};

namespace impl {
/** @brief Converts a full-width number/English letter/various symbols
        into its "narrow" counterpart.
    @param ch The character to convert.
    @returns The narrow version of a character, or the character if not
   full-width.*/
[[nodiscard]]
inline constexpr wchar_t fullWidthToNarrow(const wchar_t ch) noexcept {
  return
      // not in the fullwidth/halfwidth Unicode ranges; return character
      // unchanged
      (ch < 65'000) ? ch :
                    // fullwidth Latin letters, digits, and punctuation
          (ch >= 65'281 && ch <= 65'374) ? (ch - 65'248)
                                         :
                                         // cent and pound sterling
          (ch >= 65'504 && ch <= 65'505) ? (ch - 65'342)
                                         :
                                         // Yen
          (ch == 65'509) ? 165
                         :
                         // Not
          (ch == 65'506) ? 172
                         :
                         // macron
          (ch == 65'507) ? 175
                         :
                         // broken bar
          (ch == 65'508) ? 166
                         : ch;
}

} // namespace impl
} // namespace SERVICE_NAMESPACE
#endif // !STEMMER_HPP_
