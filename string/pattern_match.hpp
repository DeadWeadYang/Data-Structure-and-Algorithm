#pragma once
#include <string>
#include <vector>
namespace DSA
{
    namespace StringAlgorithm
    {
        namespace PatternMatch
        {
            namespace KMP
            {
                /*
                pi[i] is the length of the longest proper prefix of the substring  s[0 ... i] which is also a suffix of this substring.
                A proper prefix of a string is a prefix that is not equal to the string itself.
                By definition,  pi[0] = 0 .
                */
                std::vector<int> KMP_pi(const std::string &s)
                {
                    int n = s.length();
                    std::vector<int> pi(n);
                    for (int i = 1; i < n; i++)
                    {
                        int j = pi[i - 1];
                        while (j > 0 && s[j] != s[i])
                        {
                            j = pi[j - 1];
                        }
                        pi[i] = (s[j] == s[i]) ? j + 1 : 0;
                    }
                    return pi;
                }
                // pi[i]=next[i+1]
                std::vector<int> KMP_next(const std::string &s)
                {
                    int n = s.length();
                    std::vector<int> nxt(n + 1);
                    nxt[0] = -1;
                    for (int i = 1; i <= n; i++)
                    {
                        int j = nxt[i - 1];
                        while (j >= 0 && s[j] != s[i - 1])
                        {
                            j = nxt[j];
                        }
                        nxt[i] = j + 1;
                    }
                    return nxt;
                }
                std::vector<int> KML_match(const std::string &text, const std::string &pattern)
                {
                    int n = text.length(), m = pattern.length();
                    auto nxt = KMP_next(pattern);
                    std::vector<int> res;
                    for (int i = 0, j = 0; i < n;)
                    {
                        if (text[i] == pattern[j])
                        {
                            ++i, ++j;
                            if (j == m)
                            {
                                res.push_back(i - m);
                                j = nxt[j];
                            }
                        }
                        else
                        {
                            j = nxt[j];
                            if (j < 0)
                            {
                                ++i, ++j;
                            }
                        }
                    }
                    return res;
                }
                

            }
        }
    }
}