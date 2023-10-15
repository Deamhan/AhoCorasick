#include "BasicTestHelpers.hpp"

typedef std::vector<uint64_t> StringClass;
typedef AhoCorasick::Match<StringClass> StringMatch;

static const StringClass text = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0x1234567890ABCull, 0x987654321BCDull, 34 };
static std::vector<StringClass> strings = { {5, 6, 7}, {5}, {6, 7}, {3, 4, 5, 6, 7}, {7, 7, 7}, {7, 6}, {0x1234567890ABCull, 0x987654321BCDull} };

static std::vector<StringMatch> expected = {
	MakeMatch<StringMatch>(4,  1, strings),
	MakeMatch<StringMatch>(2,  3, strings),
	MakeMatch<StringMatch>(4,  0, strings),
	MakeMatch<StringMatch>(5,  2, strings),
	MakeMatch<StringMatch>(12, 5, strings),
	MakeMatch<StringMatch>(14, 1, strings),
	MakeMatch<StringMatch>(19, 6, strings),
};

int main()
{
	bool passed = BasicStrTest<AhoCorasick::PerformanceStrategy::MaximumPerformance>(text, expected, strings)
		&& BasicStrTest<AhoCorasick::PerformanceStrategy::Balanced>(text, expected, strings);

	return passed ? 0 : 1;
}
