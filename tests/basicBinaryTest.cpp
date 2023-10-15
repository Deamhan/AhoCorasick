#include "BasicTestHelpers.hpp"

typedef std::vector<uint8_t> StringClass;
typedef AhoCorasick::Match<StringClass> StringMatch;

static const StringClass text = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
static std::vector<StringClass> strings = { {5, 6, 7}, {5}, {6, 7}, {3, 4, 5, 6, 7}, {7, 7, 7}, {7, 6} };

static std::vector<StringMatch> expected = {
	MakeMatch<StringMatch>(4,  1),
	MakeMatch<StringMatch>(2,  3),
	MakeMatch<StringMatch>(4,  0),
	MakeMatch<StringMatch>(5,  2),
	MakeMatch<StringMatch>(12, 5),
	MakeMatch<StringMatch>(14, 1),
};

int main()
{
	bool passed = BasicStrTest<AhoCorasick::PerformanceStrategy::MaximumPerformance>(text, expected, strings)
		&& BasicStrTest<AhoCorasick::PerformanceStrategy::Balanced>(text, expected, strings);

	return passed ? 0 : 1;
}
