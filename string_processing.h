#pragma once
#include <vector>
#include <string>
#include <set>
std::vector<std::string> SplitIntoWords(const std::string& text);
template <typename StringContainer>
std::set<std::string> MakeUniqueNonEmptyStrings(const StringContainer& strings);
bool IsStopWord(const std::string& word);
bool IsValidWord(const std::string& word);