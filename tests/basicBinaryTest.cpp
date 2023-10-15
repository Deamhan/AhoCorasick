#include "BasicTestHelpers.hpp"

typedef std::vector<uint8_t> StringClass;
typedef AhoCorasick::Match<StringClass> StringMatch;

static const StringClass text = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
static std::vector<StringClass> strings = { {5, 6, 7}, {5}, {6, 7}, {3, 4, 5, 6, 7}, {7, 7, 7}, {7, 6} };

static std::vector<StringMatch> expected = {
	MakeMatch<StringMatch>(4,  1, strings),
	MakeMatch<StringMatch>(2,  3, strings),
	MakeMatch<StringMatch>(4,  0, strings),
	MakeMatch<StringMatch>(5,  2, strings),
	MakeMatch<StringMatch>(12, 5, strings),
	MakeMatch<StringMatch>(14, 1, strings),
};

int main()
{
	return BasicStrTestAllStrategies(text, expected, strings) ? 0 : 1;
}
