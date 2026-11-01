#include "Hasher.h"

using namespace std;

// Polynomial rolling-style hash settings (base and modulus).
Hasher::Hasher(uint64_t base, uint64_t mod) : base_(base), mod_(mod) {}

// Hashes one string: hash = (hash * base + char) mod m.
uint64_t Hasher::hashString(const string& str) const {
    uint64_t hashVal = 0;
    for (char ch : str) {
        hashVal = (hashVal * base_ + static_cast<unsigned char>(ch)) % mod_;
    }
    return hashVal;
}

// Hashes every k-gram.
vector<uint64_t> Hasher::hashKGrams(const vector<string>& kgrams) const {
    vector<uint64_t> hashes;
    hashes.reserve(kgrams.size());
    for (const auto& gram : kgrams) {
        hashes.push_back(hashString(gram));
    }
    return hashes;
}
