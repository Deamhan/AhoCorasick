#include "AhoCorasick.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

typedef std::wstring StringClass;

static const StringClass text = L"First word is hello, the secoind one is world";
std::vector<StringClass> strings = { L"hello", L"world", L"bla-bla", L"orld", L"orl" };

typedef AhoCorasick::Match<StringClass> StringMatch;

bool compareMatches(const StringMatch& lhs, const StringMatch& rhs)
{
	return lhs.index == rhs.index && lhs.offset == rhs.offset && *lhs.word == *rhs.word;
}

std::vector<StringMatch> expectedOffsets = { 
	{ 14, 0, &strings[0]},
	{ 41, 4, &strings[4]},
	{ 40, 1, &strings[1]},
	{ 41, 3, &strings[3]} 
};

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
