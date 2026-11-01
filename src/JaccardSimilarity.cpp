#include "JaccardSimilarity.h"
#include <unordered_set>

using namespace std;

// Returns the Jaccard score as a percentage.
double JaccardSimilarity::calculate(const vector<uint64_t>& setA,
                                    const vector<uint64_t>& setB) const {
    // Both empty = identical; one empty = nothing in common.
    if (setA.empty() && setB.empty()) return 100.0;
    if (setA.empty() || setB.empty()) return 0.0;

    // Use sets so duplicate hashes count once.
    unordered_set<uint64_t> uniqueA(setA.begin(), setA.end());
    unordered_set<uint64_t> uniqueB(setB.begin(), setB.end());

    // Count hashes present in both sets (the intersection).
    size_t intersectionCount = 0;
    for (uint64_t hash : uniqueA) {
        if (uniqueB.find(hash) != uniqueB.end()) {
            ++intersectionCount;
        }
    }

    // Union = |A| + |B| - |A ∩ B|.
    size_t unionCount = uniqueA.size() + uniqueB.size() - intersectionCount;
    if (unionCount == 0) return 0.0;

    return (static_cast<double>(intersectionCount) / unionCount) * 100.0;
}

string JaccardSimilarity::getMetricName() const {
    return "Jaccard Similarity Index";
}
