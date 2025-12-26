#include "components/Tokenizer.hpp"
#include "exceptions/UndefinedToken.hpp"
#include "stem/English.hpp"
#include "stem/Russian.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <filesystem>
#include <memory>
#include <random>
#include <regex>
#include <stdexcept>
#include <string>
#include <userver/components/component_base.hpp>
#include <userver/logging/log.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/yaml_config/schema.hpp>

namespace SERVICE_NAMESPACE {
Tokenizer::Tokenizer() {
  stemmer_ = std::make_unique<RussianStemAction>();
  stemmer_->insert(std::make_unique<EnglishStemAction>());

  static char randomPath[30];
  static std::random_device device;
  const char *storagePath = getenv("STORAGE_PATH");
  if (!storagePath) {
    strcpy(randomPath, "/tmp/");
    for (size_t i = 5; i < 28; ++i)
      randomPath[i] = device() % 26 + 'a';
    randomPath[29] = 0;
    LOG_DEBUG() << "Environment variable [STORAGE_PATH] not set, so i use "
                   "random folder in /tmp/ directory";
    storagePath = randomPath;
  }
  LOG_DEBUG() << "STORAGE FOLDER: " << storagePath;

  storagePath_ = storagePath;

  if (std::filesystem::exists(storagePath_ / "forward"))
    load();
}

Tokenizer::~Tokenizer() { 
	LOG_DEBUG() << "Saving tokenizer data";
	save();
}

void Tokenizer::load() {
  forward_.deserialize((storagePath_ / "forward").c_str());
  FILE *file = fopen((storagePath_ / "backward").c_str(), "rb");
  size_t maxLen, nWords = backward_.size();
  if (fread(&maxLen, sizeof(size_t), 1, file) != 1)
    throw std::runtime_error("Can't read maximum buffer length from file");
  auto buffer = static_cast<wchar_t *>(malloc(maxLen * sizeof(wchar_t)));
  if (fread(&nWords, sizeof(size_t), 1, file) != 1)
    throw std::runtime_error("Can't read number of words from file");

  for (size_t i = 0; i < nWords; ++i) {
    size_t len;
    if (fread(&len, sizeof(size_t), 1, file) != 1)
      throw std::runtime_error("Can't read length of the string from file");
    if (fread(buffer, sizeof(wchar_t), len, file) != len)
      throw std::runtime_error("Can't read word from file");
    buffer[len] = 0;
    backward_.emplace_back(buffer);
  }

  free(buffer);
  fclose(file);

  if (forward_.size() != backward_.size())
    throw std::runtime_error("After deserialization forward and backward "
                             "structures have different sizes");
}

void Tokenizer::save() {
  std::filesystem::create_directories(storagePath_);
  forward_.serialize((storagePath_ / "forward").c_str());
  FILE *file = fopen((storagePath_ / "backward").c_str(), "wb");
  size_t maxLen = 0, nWords = backward_.size();
  for (const auto &s : backward_)
    maxLen = std::max(maxLen, s.size());

  ++maxLen;
  auto buffer = static_cast<wchar_t *>(malloc(maxLen * sizeof(wchar_t)));
  if (fwrite(&maxLen, sizeof(size_t), 1, file) != 1)
    throw std::runtime_error("Can't write maximum buffer length to file");

  if (fwrite(&nWords, sizeof(size_t), 1, file) != 1)
    throw std::runtime_error("Can't write number of words to file");

  for (const auto &s : backward_) {
    size_t len = s.size();
    if (fwrite(&len, sizeof(size_t), 1, file) != 1)
      throw std::runtime_error("Can't write length of the string to file");
    if (fwrite(s.data(), sizeof(wchar_t), s.size(), file) != s.size())
      throw std::runtime_error("Can't write word to file");
  }
  free(buffer);
  fclose(file);
}

IR::Vector<std::uint32_t> Tokenizer::tokenize(const std::wstring &s) {
  std::basic_regex<wchar_t> pattern(wpatternText,
                                    std::regex_constants::ECMAScript |
                                        std::regex_constants::multiline);
  std::regex_iterator<std::wstring::const_iterator> it(s.begin(), s.end(),
                                                       pattern),
      end;

  IR::Vector<std::uint32_t> tokens;
  while (it != end) {
    auto str = it->str();

    // do not tokenize tags
    if (!(str.size() > 2 && str[0] == L'<' && str[str.size() - 1] == L'>'))
      (*stemmer_)(str);
    size_t oldSize = forward_.size();
    std::uint32_t token = forward_.getOrInsert(str.data(), str.size(), oldSize);
    if (token == oldSize)
      backward_.push_back(str);

    tokens.emplace_back(token);
    ++it;
  }

  return tokens;
}

std::wstring Tokenizer::detokenize(const IR::Vector<std::uint32_t> &tokens) {
  std::wstring s;
  for (size_t i = 0; const auto &token : tokens) {
    if (token >= backward_.size())
      throw exception::UndefinedTokenException(token);
    s += backward_[token] + (i < tokens.size() ? L" " : L"");
  }
  return s;
}

Tokenizer &TokenizerComponent::GetTokenizer() { return tokenizer_; }
TokenizerComponent::~TokenizerComponent() { }
} // namespace SERVICE_NAMESPACE
