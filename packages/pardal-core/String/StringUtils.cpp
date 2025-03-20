
#include <String/StringUtils.h>
#include <algorithm>

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include <String/fts_fuzzymatch/fts_fuzzy_match.h>
// Created on 2019-03-18 by sisco


pdl::String pdl::StringUtils::ToLower(const String& _string)
{
	String newString = _string;
	std::transform(newString.begin(), newString.end(), newString.begin(),
		[](unsigned char c) { return std::tolower(c); });

	return newString;
}

pdl::String pdl::StringUtils::ToUpper(const String& _string)
{
	String newString = _string;
	std::transform(newString.begin(), newString.end(), newString.begin(),
		[](unsigned char c) { return std::toupper(c); });

	return newString;
}

bool pdl::StringUtils::FuzzyMatch(const char* _pattern, const char* _string, int& _score)
{
	return fts::fuzzy_match(_pattern, _string, _score);
}

bool pdl::StringUtils::FuzzyMatch(const char* _pattern, const char* _string)
{
    return fts::fuzzy_match_simple(_pattern, _string);
}
