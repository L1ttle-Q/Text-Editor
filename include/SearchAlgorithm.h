#ifndef _SEARCHALGORITHM_H_
#define _SEARCHALGORITHM_H_

#include <cstring>
#include <iostream>
#include "globals.h"

class KMP
{
private:
    char* pattern;
    int* kmp;
    int length;

public:
    KMP() = delete;
    KMP(const char *s, const int& l): length(l)
    {
        pattern = new char[length + 1];
        kmp = new int[length + 1];
        memcpy(pattern + 1, s, sizeof(char) * length);
        int j = 0, t = 0;
        for (int i = 2; i <= l; i++)
        {
            while (j && pattern[i] != pattern[j + 1])
                j = kmp[j];
            j += (pattern[i] == pattern[j + 1]);
            kmp[i] = j;
        }
    }
    KMP(const std::string& s): KMP(s.c_str(), s.length()) {}

    std::vector<int> find(const char *s, bool enableRepeat = false)
    {
        int l = strlen(s), j = 0;
        std::vector<int> res;
        for (int i = 0; i < l; i++)
        {
            while (j && s[i] != pattern[j + 1])
                j = kmp[j];
            if (s[i] == pattern[j + 1])
                j++;
            if (j == length)
            {
                res.emplace_back(i - j + 2);
                j = enableRepeat ? kmp[j] : 0;
            }
        }
        return res;
    }
    std::vector<int> find(const std::string& s, bool enableRepeat = false)
    {return find(s.c_str(), enableRepeat);}
};


#endif