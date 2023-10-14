#include "AhoCorasick.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

typedef std::vector<uint8_t> StringClass;
typedef AhoCorasick::Match<StringClass> StringMatch;

static const StringClass text = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
static std::vector<StringClass> strings = { {5, 6, 7}, {5}, {6, 7}, {3, 4, 5, 6, 7}, {7, 7, 7}, {7, 6} };

static StringMatch MakeMatch(size_t offset, size_t index)
{
	return StringMatch{ offset, index, &strings[index] };
}

static std::vector<StringMatch> expectedOffsets = {
	MakeMatch(4,  1),
	MakeMatch(2,  3),
	MakeMatch(4,  0),
	MakeMatch(5,  2),
	MakeMatch(12, 5),
	MakeMatch(14, 1),
};

static bool compareMatches(const StringMatch& lhs, const StringMatch& rhs)
{
	return lhs.index == rhs.index && lhs.offset == rhs.offset 
		&& std::equal(lhs.word->begin(), lhs.word->end(), rhs.word->begin());
}

template <AhoCorasick::PerformanceStrategy strategy>
static bool BasicCharStrTest()
{
	std::vector<StringMatch> found;
	auto callback = [&found](const StringMatch& m)
	{
		found.emplace_back(m.offset, m.index, m.word);
		return true;
	};

	AhoCorasick::Scanner<StringClass, strategy> scanner(strings.begin(), strings.end());

	scanner.Scan(callback, text.begin(), text.end());

	return std::equal(expectedOffsets.begin(), expectedOffsets.end(), found.begin(), compareMatches);
}

int main()
{
	bool passed = BasicCharStrTest<AhoCorasick::PerformanceStrategy::MaximumPerformance>()
		&& BasicCharStrTest<AhoCorasick::PerformanceStrategy::Balanced>();

	return passed ? 0 : 1;
}
