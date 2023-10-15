#include "BasicTestHelpers.hpp"

typedef std::string StringClass;
typedef AhoCorasick::Match<StringClass> StringMatch;

static const StringClass text = "First word is hello, the secoind one is world";
static std::vector<StringClass> strings = { "hello", "world", "bla-bla", "orld", "orl" };

static std::vector<StringMatch> expected = {
	MakeMatch<StringMatch>(14, 0, strings),
	MakeMatch<StringMatch>(41, 4, strings),
	MakeMatch<StringMatch>(40, 1, strings),
	MakeMatch<StringMatch>(41, 3, strings),
};

int main()
{
	return BasicStrTestAllStrategies(text, expected, strings) ? 0 : 1;
}
