#include "BasicTestHelpers.hpp"

typedef std::string StringClass;
typedef AhoCorasick::Match<StringClass> StringMatch;

static const StringClass text = "First word is hello, the secoind one is world";
std::vector<StringClass> strings = { "hello", "world", "bla-bla", "orld", "orl" };

std::vector<StringMatch> expected = {
	{ 14, 0, &strings[0]},
	{ 41, 4, &strings[4]},
	{ 40, 1, &strings[1]},
	{ 41, 3, &strings[3]} 
};

int main()
{
	bool passed = BasicStrTest<AhoCorasick::PerformanceStrategy::MaximumPerformance>(text, expected, strings)
		&& BasicStrTest<AhoCorasick::PerformanceStrategy::Balanced>(text, expected, strings);

	return passed ? 0 : 1;
}
