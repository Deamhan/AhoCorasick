#pragma once

#include "AhoCorasick.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

template <class StringMatchType, class StringContainerType>
static StringMatchType MakeMatch(size_t offset, size_t index, const StringContainerType& strings)
{
	return StringMatchType{ offset, index, &strings[index] };
}

template <class StringMatchType>
static bool compareMatches(const StringMatchType& lhs, const StringMatchType& rhs)
{
	return lhs.index == rhs.index && lhs.offset == rhs.offset
		&& std::equal(lhs.word->begin(), lhs.word->end(), rhs.word->begin());
}

template <AhoCorasick::PerformanceStrategy strategy, class MatchContainerType, class StringContainerType, class StringType>
static bool BasicStrTest(StringType text, MatchContainerType matches, StringContainerType strings)
{
	typedef typename MatchContainerType::value_type StringMatch;

	std::vector<StringMatch> found;
	auto callback = [&found](const StringMatch& m)
	{
		found.emplace_back(m.offset, m.index, m.word);
		return true;
	};

	AhoCorasick::Scanner<StringType, strategy> scanner(strings.begin(), strings.end());

	scanner.Scan(callback, text.cbegin(), text.cend());

	return matches.size() == found.size() 
		&& std::equal(matches.begin(), matches.end(), found.begin(), compareMatches<typename MatchContainerType::value_type>);
}

template <class MatchContainerType, class StringContainerType, class StringType>
static bool BasicStrTestAllStrategies(StringType text, MatchContainerType matches, StringContainerType strings)
{
	return BasicStrTest<AhoCorasick::PerformanceStrategy::Balanced>(text, matches, strings)
		&& BasicStrTest<AhoCorasick::PerformanceStrategy::MaximumPerformance>(text, matches, strings);
}

template <AhoCorasick::PerformanceStrategy strategy, class MatchContainerType, class StringContainerType,
	class StringType, class ContinueHandler>
static bool ContinueStrTest(StringType text, MatchContainerType matches, StringContainerType strings, 
	ContinueHandler continueHandler)
{
	typedef typename MatchContainerType::value_type StringMatch;

	std::vector<StringMatch> found;
	auto callback = [&found](const StringMatch& m)
	{
		found.emplace_back(m.offset, m.index, m.word);
		return true;
	};

	AhoCorasick::Scanner<StringType, strategy> scanner(strings.begin(), strings.end());

	scanner.Scan(callback, text.cbegin(), text.cend(), continueHandler);

	return matches.size() == found.size()
		&& std::equal(matches.begin(), matches.end(), found.begin(), compareMatches<typename MatchContainerType::value_type>);
}
