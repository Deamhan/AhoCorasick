#include "BasicTestHelpers.hpp"

typedef std::string StringClass;
typedef AhoCorasick::Match<StringClass> StringMatch;

static const StringClass text = "First word is hello, the secoind one is world";
std::vector<StringClass> strings = { "hello", "world", "bla-bla", "orld", "orl" };

std::vector<StringMatch> expected = {
	MakeMatch<StringMatch>(14, 0, strings),
	MakeMatch<StringMatch>(41, 4, strings),
	MakeMatch<StringMatch>(40, 1, strings),
	MakeMatch<StringMatch>(41, 3, strings),
};

int main()
{
	bool passed = BasicStrTest<AhoCorasick::PerformanceStrategy::MaximumPerformance>(text, expected, strings)
		&& BasicStrTest<AhoCorasick::PerformanceStrategy::Balanced>(text, expected, strings);

	return passed ? 0 : 1;
}
