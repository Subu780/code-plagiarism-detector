#include "ContainmentSimilarity.h"
#include <unordered_set>
#include <algorithm>

using namespace std;

// Returns the containment score: |A ∩ B| / min(|A|, |B|), as a percentage.
double ContainmentSimilarity::calculate(const vector<uint64_t>& setA,
                                        const vector<uint64_t>& setB) const {
    // Both empty = identical; one empty = nothing in common.
    if (setA.empty() && setB.empty()) return 100.0;
    if (setA.empty() || setB.empty()) return 0.0;

    unordered_set<uint64_t> uniqueA(setA.begin(), setA.end());
    unordered_set<uint64_t> uniqueB(setB.begin(), setB.end());

    size_t intersectionCount = 0;
    for (uint64_t hash : uniqueA) {
        if (uniqueB.find(hash) != uniqueB.end()) {
            ++intersectionCount;
        }
    }

    // Divide by the smaller set so a small file copied into a big one scores high.
    size_t minSize = min(uniqueA.size(), uniqueB.size());
    if (minSize == 0) return 0.0;

    return (static_cast<double>(intersectionCount) / minSize) * 100.0;
}

string ContainmentSimilarity::getMetricName() const {
    return "Containment Similarity Index";
}
