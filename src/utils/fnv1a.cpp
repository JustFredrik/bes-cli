#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include <cstdint>

uint32_t fnv1a_hash(std::string str)
{
    uint32_t hash = uint32_t(2166136261);
    for (const auto c : str)
    {
        hash ^= c;
        hash *= uint32_t(16777619);
    }
    return hash;
}

std::string fnv1a_hash_hex(std::string str)
{
    uint32_t hash = uint32_t(2166136261);
    for (const auto c : str)
    {
        hash ^= c;
        hash *= uint32_t(16777619);
    }
    std::ostringstream out;
    out << std::setfill('0') << std::setw(8) << std::hex << hash;
    return out.str();
}

uint16_t fnv1a_hash_uint16(std::string str)
{
    uint32_t hash = uint32_t(2166136261);
    for (const auto c : str)
    {
        hash ^= c;
        hash *= uint32_t(16777619);
    }
    return uint16_t(hash >> 16 ^ (hash & uint32_t(65535)));
}