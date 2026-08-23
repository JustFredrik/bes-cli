#pragma once
#include <string>

uint16_t fnv1a_hash_uint16(std::string str) {
    uint32_t hash = uint32_t(2166136261);
    for (const auto c : str ) {
        hash ^= c;
        hash *= uint32_t(16777619);
    }
    return uint16_t( hash >> 16 ^ (hash &  uint32_t(65535)));
}