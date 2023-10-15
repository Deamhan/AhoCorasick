#pragma once

#include "AhoCorasick.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

template<class StringMatchType>
static StringMatchType MakeMatch(size_t offset, size_t index)
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
	std::vector<StringMatch> found;
	auto callback = [&found](const StringMatch& m)
	{
		found.emplace_back(m.offset, m.index, m.word);
		return true;
	};

	AhoCorasick::Scanner<StringClass, strategy> scanner(strings.begin(), strings.end());

	scanner.Scan(callback, text.begin(), text.end());

	return std::equal(matches.begin(), matches.end(), found.begin(), compareMatches<typename MatchContainerType::value_type>);
}
