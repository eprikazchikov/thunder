#ifndef UTILS_H
#define UTILS_H

#include "engine.h"

class ENGINE_EXPORT Utils {
public:
    static string       wstringToUtf8           (const wstring &in);

    static string       utf32ToUtf8             (const u32string &in);

    static u32string    utf8ToUtf32             (const string &in);

};

#endif // UTILS_H
