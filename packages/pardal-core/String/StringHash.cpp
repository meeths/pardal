#include <String/StringHash.h>
#include <Base/Hash/Hashes.h>

namespace pdl
{
    StringHash::StringHash(const char* _string)
    {
        m_Id = murmur::StaticHashValueInternal32(_string, strlen(_string), defaultHashSeed);//MurmurHash64A(_string, int(strlen(_string)), defaultHashSeed);
#ifdef KEEP_ORIGINAL_STRING
        _debugOriginalString = _string;
#endif
    }

    StringHash::StringHash(const String &_string)
    {
        m_Id = murmur::StaticHashValueInternal32(_string.c_str(), strlen(_string.c_str()), defaultHashSeed);//MurmurHash64A(_string.c_str(), int(_string.size()), defaultHashSeed);
#ifdef KEEP_ORIGINAL_STRING
        _debugOriginalString = _string;
#endif
    }

    bool operator==(const ConstStringHash& a, const StringHash& b)
    {
            return StringHash(a) == b;
    }
}
