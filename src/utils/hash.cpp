#pragma once
#include <string>

uint16_t fnv1a_hash_u16(const std::string &s);

uint16_t fnv1a_hash_u16(const std::string &s)
{
// fnv documentation: http://www.isthe.com/chongo/tech/comp/fnv/#google_vignette
// 32bit hash and then xor fold the first 16 and the last 16 bits to get a uint16.
    uint32_t hash = 0x811c9dc5;
    const uint32_t prime = 0x01000193;
    for (char c : s)
    {
        hash ^= static_cast<uint8_t>(c);
        hash *= prime;
    }
    return static_cast<uint16_t>((hash >> 16) ^ (hash & 0xFFFF));
}