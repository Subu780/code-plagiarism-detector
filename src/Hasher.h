#pragma once

#include <string>
#include <vector>
#include <cstdint>

class Hasher {
private:
    uint64_t base_;
    uint64_t mod_;

public:
    // Defaults: base 31337, modulus 1,000,000,007 (a large prime).
    Hasher(uint64_t base = 31337ULL, uint64_t mod = 1000000007ULL);
    uint64_t hashString(const std::string& str) const;
    std::vector<uint64_t> hashKGrams(const std::vector<std::string>& kgrams) const;
};
