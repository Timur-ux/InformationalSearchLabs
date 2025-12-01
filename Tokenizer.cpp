#include <clocale>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>

int main(int argc, const char * argw[]) {
	setlocale(LC_ALL, "");
	const wchar_t* patternText = L"(([A-Z]|[a-z]|[А-Я]|[а-я])+)|([[:punct:]])";
	std::basic_regex<wchar_t> pattern(patternText, std::regex_constants::ECMAScript | std::regex_constants::multiline);

	std::wstring text, line;
	while(std::getline(std::wcin, line)) 
		text += line + L'\n';

	std::wcout << text << '\n';
	std::wcout << text.size() << '\n';

	std::wcout << "Pattern: " << patternText << '\n';
	std::wcout << "Matches: [\n";
	std::regex_iterator<std::wstring::iterator> begin(text.begin(), text.end(), pattern), end;
	while(begin != end)
		std::wcout << (begin++)->str() << ",\n";
	std::wcout << "]\n";

	return 0;
}
