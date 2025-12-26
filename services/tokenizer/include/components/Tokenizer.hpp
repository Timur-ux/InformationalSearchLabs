#ifndef TOKENIZER_HPP_
#define TOKENIZER_HPP_
#include "Trie.hpp"
#include "Vector.hpp"
#include "stem/Stemmer.hpp"
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <userver/components/component_base.hpp>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/yaml_config/schema.hpp>
#pragma once
namespace SERVICE_NAMESPACE {
using namespace userver;
class Tokenizer {
  static constexpr const wchar_t *wpatternText =
      L"(([A-Z]|[a-z]|[А-Я]|[а-я])+)|(<\\w+>)|(</\\w+>)|([0-9])";
  static constexpr const char *patternText =
      "(([A-Z]|[a-z]|[А-Я]|[а-я])+)|(<\\w+>)|(</\\w+>)|([0-9])";

  std::filesystem::path storagePath_;
  IR::Trie<wchar_t, std::uint32_t> forward_;
  IR::Vector<std::wstring> backward_;
	std::unique_ptr<StemAction<std::wstring>> stemmer_;

  Tokenizer();

  void load();
  void save();
  friend class TokenizerComponent;

public:
  IR::Vector<std::uint32_t> tokenize(const std::wstring &s);
  std::wstring detokenize(const IR::Vector<std::uint32_t> &tokens);
  ~Tokenizer();
};

class TokenizerComponent : public components::ComponentBase {
  Tokenizer tokenizer_;

public:
  static constexpr std::string_view kName = "tokenizer";

	using ComponentBase::ComponentBase;
	using components::ComponentBase::GetStaticConfigSchema;

	~TokenizerComponent();

  Tokenizer &GetTokenizer();
};
} // namespace SERVICE_NAMESPACE
#endif // !TOKENIZER_HPP_
