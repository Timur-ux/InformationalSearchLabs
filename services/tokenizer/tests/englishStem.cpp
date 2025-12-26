#include "stem/English.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
static const std::vector<std::pair<std::wstring, std::wstring>> tests{
    {L"consign", L"consign"},
    {L"consigned", L"consign"},
    {L"consigning", L"consign"},
    {L"consignment", L"consign"},
    {L"consist", L"consist"},
    {L"consisted", L"consist"},
    {L"consistency", L"consist"},
    {L"consistent", L"consist"},
    {L"consistently", L"consist"},
    {L"consisting", L"consist"},
    {L"consists", L"consist"},
    {L"consolation", L"consol"},
    {L"consolations", L"consol"},
    {L"consolatory", L"consolatori"},
    {L"console", L"consol"},
    {L"consoled", L"consol"},
    {L"consoles", L"consol"},
    {L"consolidate", L"consolid"},
    {L"consolidated", L"consolid"},
    {L"consolidating", L"consolid"},
    {L"consoling", L"consol"},
    {L"consolingly", L"consol"},
    {L"consols", L"consol"},
    {L"consonant", L"conson"},
    {L"consort", L"consort"},
    {L"consorted", L"consort"},
    {L"consorting", L"consort"},
    {L"conspicuous", L"conspicu"},
    {L"conspicuously", L"conspicu"},
    {L"conspiracy", L"conspiraci"},
    {L"conspirator", L"conspir"},
    {L"conspirators", L"conspir"},
    {L"conspire", L"conspir"},
    {L"conspired", L"conspir"},
    {L"conspiring", L"conspir"},
    {L"constable", L"constabl"},
    {L"constables", L"constabl"},
    {L"constance", L"constanc"},
    {L"constancy", L"constanc"},
    {L"constant", L"constant"},
    {L"knack", L"knack"},
    {L"knackeries", L"knackeri"},
    {L"knacks", L"knack"},
    {L"knag", L"knag"},
    {L"knave", L"knave"},
    {L"knaves", L"knave"},
    {L"knavish", L"knavish"},
    {L"kneaded", L"knead"},
    {L"kneading", L"knead"},
    {L"knee", L"knee"},
    {L"kneel", L"kneel"},
    {L"kneeled", L"kneel"},
    {L"kneeling", L"kneel"},
    {L"kneels", L"kneel"},
    {L"knees", L"knee"},
    {L"knell", L"knell"},
    {L"knelt", L"knelt"},
    {L"knew", L"knew"},
    {L"knick", L"knick"},
    {L"knif", L"knif"},
    {L"knife", L"knife"},
    {L"knight", L"knight"},
    {L"knightly", L"knight"},
    {L"knights", L"knight"},
    {L"knit", L"knit"},
    {L"knits", L"knit"},
    {L"knitted", L"knit"},
    {L"knitting", L"knit"},
    {L"knives", L"knive"},
    {L"knob", L"knob"},
    {L"knobs", L"knob"},
    {L"knock", L"knock"},
    {L"knocked", L"knock"},
    {L"knocker", L"knocker"},
    {L"knockers", L"knocker"},
    {L"knocking", L"knock"},
    {L"knocks", L"knock"},
    {L"knopp", L"knopp"},
    {L"knot", L"knot"},
    {L"knots", L"knot"},
};

using namespace SERVICE_NAMESPACE;

TEST(StemmerTestSuite, EnglishLang) {
	EnglishStemAction stemmer;
	for(size_t i = 0; i < tests.size(); ++i) {
		std::wstring given = tests[i].first, expected = tests[i].second;
		std::wcout << i << L") " << given << ' ' << expected << '\n';
		stemmer(given);
		std::wstring result = given;
		std::wcout << i << L") " << result << ' ' << expected << '\n';

		EXPECT_EQ(result, expected);
	}
}
