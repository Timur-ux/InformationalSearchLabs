#include <clocale>
#include <filesystem>
#include <iostream>
#include <regex>
#include <string>
#include "tokenizer.hpp"
#include "Trie.hpp"

int main() {
	setlocale(LC_ALL, "");
	std::basic_regex<wchar_t> pattern(patternText, std::regex_constants::ECMAScript | std::regex_constants::multiline);
	IR::Trie<wchar_t, size_t> tokens;

	std::wstring text, line;
	if(std::filesystem::exists("/tmp/temporary-trie-data-file")) 
		tokens.deserialize("/tmp/temporary-trie-data-file");
	std::cout << "Tokens at begin: " <<  tokens.size()<< '\n';

	while(std::getline(std::wcin, line)) 
		text += line + L'\n';

	std::wcout << text << '\n';
	std::wcout << text.size() << '\n';

	std::wcout << "Pattern: " << patternText << '\n';
	std::wcout << "Matches: [\n";
	std::regex_iterator<std::wstring::iterator> it(text.begin(), text.end(), pattern), end;
	while(it != end) {
		const auto & str = it->str();
		size_t key = tokens.getOrInsert({str.c_str(), str.size()}, tokens.size());
		std::wcout << "word = " << str << "; token = "<< key <<  "\n";
		++it;
	}

	std::wcout << "]\n";
	std::cout << "Tokens at end: " <<  tokens.size()<< '\n';

	tokens.serialize("/tmp/temporary-trie-data-file");

	return 0;
}
