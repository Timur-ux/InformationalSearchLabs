#include "Trie.hpp"
#include "Vector.hpp"
#include <algorithm>
#include <cctype>
#include <clocale>
#include <codecvt>
#include <cstdint>
#include <filesystem>
#include <format>
#include <iostream>
#include <locale>
#include <regex>
#include <string>

int main(int argc, const char *argw[]) {
  setlocale(LC_ALL, "");
  if (argc < 2)
    throw std::invalid_argument(
        std::format("Usage: {} <path to tokenizer backward data>", argw[0]));
  std::filesystem::path storagePath_ = argw[1];
  if (!std::filesystem::exists(storagePath_ / "backward"))
    throw std::invalid_argument(
        std::format("Tokenizer backward file doesn't exitst at: {}",
                    (storagePath_ / "backward").c_str()));

  IR::Vector<std::wstring> backward_;
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
  std::uint32_t docId, tokenId;
  size_t counter = 0;
  std::vector<std::pair<std::uint32_t, size_t>> data(backward_.size());
  for (size_t i = 0; i < data.size(); ++i)
    data[i].first = i;
  while (std::cin >> docId >> tokenId) {
    ++counter;
    ++data.at(tokenId).second;
  }

  std::sort(
      std::begin(data), std::end(data),
      [](const auto &lhs, const auto &rhs) { return lhs.second > rhs.second; });
  std::cerr << "Readed: " << counter
            << " entries start printing in sorted by frequency order\n";
  std::cerr << "Format:\n";
  std::cerr << "token\trank\tfrequency\n";
  for (size_t i = 0; i < data.size(); ++i)
    std::wcout << backward_.at(data[i].first) << '\t' << i << '\t'
               << data[i].second << '\n';

  return 0;
}
