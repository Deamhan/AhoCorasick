#include "BasicTestHelpers.hpp"

typedef std::wstring StringClass;
typedef AhoCorasick::Match<StringClass> StringMatch;

// source: https://en.wikipedia.org/wiki/Aho%E2%80%93Corasick_string_matching_algorithm
static bool WikiTest()
{
	const StringClass text = L"abccab";
	std::vector<StringClass> strings = { L"a", L"ab", L"bab", L"bc", L"bca", L"c", L"caa" };

	std::vector<StringMatch> expected = {
		MakeMatch<StringMatch>(0, 0, strings),
		MakeMatch<StringMatch>(0, 1, strings),
		MakeMatch<StringMatch>(1, 3, strings),
		MakeMatch<StringMatch>(2, 5, strings),
		MakeMatch<StringMatch>(3, 5, strings),
		MakeMatch<StringMatch>(4, 0, strings),
		MakeMatch<StringMatch>(4, 1, strings),
	};

	return BasicStrTestAllStrategies(text, expected, strings);
}

// source:  https://www.geeksforgeeks.org/aho-corasick-algorithm-pattern-searching/
static bool GeeksTest()
{
	const StringClass text = L"ahishers";
	std::vector<StringClass> strings = { L"he", L"she", L"hers", L"his" };

	std::vector<StringMatch> expected = {
		MakeMatch<StringMatch>(1, 3, strings),
		MakeMatch<StringMatch>(3, 1, strings),
		MakeMatch<StringMatch>(4, 0, strings),
		MakeMatch<StringMatch>(4, 2, strings),
	};

	return BasicStrTestAllStrategies(text, expected, strings);
}

int main()
{
	if (!WikiTest())
	{
		std::cerr << "Wikipedia test failed\n";
		return 1;
	}

	if (!GeeksTest())
	{
		std::cerr << "Geeksforgeeks test failed\n";
		return 2;
	}

	return 0;
}
