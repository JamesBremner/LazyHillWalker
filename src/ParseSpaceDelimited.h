#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

/** Parse Space Delimited line
 * @param[in] l line from a space delimited text file
 * @return vector of strings containing the columns extracted from line
 */
std::vector<std::string> ParseSpaceDelimited(
    const std::string &l)
{
    std::vector<std::string> token;
    std::stringstream sst(l);
    std::string a;
    while (getline(sst, a, ' '))
        token.push_back(a);

    token.erase(
        remove_if(
            token.begin(),
            token.end(),
            [](std::string t) {
                return (t.empty());
            }),
        token.end());

    return token;
}
