// ----------------------------------------------------------------------------
// constexpr_hash_murmur.h 
// ----------------------------------------------------------------------------
// compile time string hashing(murmur hash equivalent).
// @see also https://sites.google.com/site/murmurhash/
#pragma once
#include <stdint.h>

#if defined(_MSC_VER)
#pragma warning (push)
#pragma warning (disable:4592)
#endif

namespace murmur
{
    #define MURMUR_M_32      (0x5bd1e995)
    #define MURMUR_R_32      (24)

    #define MURMUR_M_64      (0xc6a4a7935bd1e995)
    #define MURMUR_R_64      (47)

    constexpr uint32_t  Load4(const char* data)
    {
        return (uint32_t(data[3]) << 24) | (uint32_t(data[2]) << 16) |
               (uint32_t(data[1]) <<  8) | (uint32_t(data[0]) <<  0);
    }

    constexpr uint64_t  Load8(const char* data)
    {
        return (uint64_t(data[7]) << 56) | (uint64_t(data[6]) << 48) |
               (uint64_t(data[5]) << 40) | (uint64_t(data[4]) << 32) |
               (uint64_t(data[3]) << 24) | (uint64_t(data[2]) << 16) |
               (uint64_t(data[1]) <<  8) | (uint64_t(data[0]) <<  0);
    }

    constexpr uint32_t  StaticHashValueLast32(uint32_t h)
    {
        return (((h*MURMUR_M_32) ^ ((h*MURMUR_M_32) >> 13)) * MURMUR_M_32) ^
              ((((h*MURMUR_M_32) ^ ((h*MURMUR_M_32) >> 13)) * MURMUR_M_32) >> 15);
    }

    constexpr uint32_t  StaticHashValueLast32_(uint32_t h)
    {
        return (((h) ^ ((h) >> 13)) * MURMUR_M_32) ^
              ((((h) ^ ((h) >> 13)) * MURMUR_M_32) >> 15);
    }

    constexpr uint32_t  StaticHashValue32Tail1(uint32_t h, const char* data)
    {
        return StaticHashValueLast32((h ^ uint32_t(data[0])));
    }

    constexpr uint32_t  StaticHashValue32Tail2(uint32_t h, const char* data)
    {
        return StaticHashValue32Tail1((h ^ uint32_t(data[1]) << 8), data);
    }

    constexpr uint32_t  StaticHashValue32Tail3(uint32_t h, const char* data)
    {
        return StaticHashValue32Tail2((h ^ uint32_t(data[2]) << 16), data);
    }

    constexpr uint64_t  StaticHashValueLast64(uint64_t h)
    {
        return (((h*MURMUR_M_64) ^ ((h*MURMUR_M_64) >> MURMUR_R_64)) * MURMUR_M_64) ^
              ((((h*MURMUR_M_64) ^ ((h*MURMUR_M_64) >> MURMUR_R_64)) * MURMUR_M_64) >> MURMUR_R_64);
    }

    constexpr uint64_t  StaticHashValueLast64_(uint64_t h)
    {
        return (((h) ^ ((h) >> MURMUR_R_64)) * MURMUR_M_64) ^
              ((((h) ^ ((h) >> MURMUR_R_64)) * MURMUR_M_64) >> MURMUR_R_64);
    }

    constexpr uint64_t  StaticHashValue64Tail1(uint64_t h, const char* data)
    {
        return StaticHashValueLast64((h ^ uint64_t(data[0])));
    }

    constexpr uint64_t  StaticHashValue64Tail2(uint64_t h, const char* data)
    {
        return StaticHashValue64Tail1((h ^ uint64_t(data[1]) << 8), data);
    }

    constexpr uint64_t  StaticHashValue64Tail3(uint64_t h, const char* data)
    {
        return StaticHashValue64Tail2((h ^ uint64_t(data[2]) << 16), data);
    }

    constexpr uint64_t  StaticHashValue64Tail4(uint64_t h, const char* data)
    {
        return StaticHashValue64Tail3((h ^ uint64_t(data[3]) << 24), data);
    }

    constexpr uint64_t  StaticHashValue64Tail5(uint64_t h, const char* data)
    {
        return StaticHashValue64Tail4((h ^ uint64_t(data[4]) << 32), data);
    }

    constexpr uint64_t  StaticHashValue64Tail6(uint64_t h, const char* data)
    {
        return StaticHashValue64Tail5((h ^ uint64_t(data[5]) << 40), data);
    }

    constexpr uint64_t  StaticHashValue64Tail7(uint64_t h, const char* data)
    {
        return StaticHashValue64Tail6((h ^ uint64_t(data[6]) << 48), data);
    }

    constexpr uint32_t  StaticHashValueRest32(uint32_t h, size_t len, const char* data)
    {
        return ((len & 3) == 3) ? StaticHashValue32Tail3(h, data) :
               ((len & 3) == 2) ? StaticHashValue32Tail2(h, data) :
               ((len & 3) == 1) ? StaticHashValue32Tail1(h, data) :
                                  StaticHashValueLast32_(h);
    }

    constexpr uint64_t  StaticHashValueRest64(uint64_t h, size_t len, const char* data)
    {
        return ((len & 7) == 7) ? StaticHashValue64Tail7(h, data) :
               ((len & 7) == 6) ? StaticHashValue64Tail6(h, data) :
               ((len & 7) == 5) ? StaticHashValue64Tail5(h, data) :
               ((len & 7) == 4) ? StaticHashValue64Tail4(h, data) :
               ((len & 7) == 3) ? StaticHashValue64Tail3(h, data) :
               ((len & 7) == 2) ? StaticHashValue64Tail2(h, data) :
               ((len & 7) == 1) ? StaticHashValue64Tail1(h, data) :
                                  StaticHashValueLast64_(h);
    }

    constexpr uint32_t  StaticHashValueLoop32(size_t i, uint32_t h, size_t len, const char* data)
    {
        return (i == 0 ?
            StaticHashValueRest32(h, len, (const char*)data) :
            StaticHashValueLoop32(i - 1,
            ((h * MURMUR_M_32) ^ (((Load4(data) * MURMUR_M_32) ^
            ((Load4(data) * MURMUR_M_32) >> MURMUR_R_32)) * MURMUR_M_32)),
            len,
            data + 4));
    }

    constexpr uint64_t  StaticHashValueLoop64(size_t i, uint64_t h, size_t len, const char* data)
    {
        return (i == 0 ?
            StaticHashValueRest64(h, len, (const char*)data) :
            StaticHashValueLoop64(i - 1,
            (h ^ (((Load8(data) * MURMUR_M_64) ^
            ((Load8(data) * MURMUR_M_64) >> MURMUR_R_64)) *
            MURMUR_M_64)) * MURMUR_M_64,
            len,
            data + 8));
    }

    constexpr uint32_t  StaticHashValueInternal32(const char* key, size_t len, uint32_t seed)
    {
        return StaticHashValueLoop32(len/4, seed ^ (uint32_t(len)), len, (const char*)key);
    }

    constexpr uint64_t  StaticHashValueInternal64(const char* key, size_t len, uint64_t seed)
    {
        return StaticHashValueLoop64(len/8, seed ^ (uint64_t(len)*MURMUR_M_64), (len), (const char*)key);
    }

} // namespace murmur 

#define HASH32(NAME)    murmur::StaticHashValueInternal32(NAME, sizeof(NAME), 0)
#define HASH64(NAME)    murmur::StaticHashValueInternal64(NAME, sizeof(NAME), 0)

#if defined(_MSC_VER)
#pragma warning (pop)
#endif