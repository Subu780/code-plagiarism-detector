#pragma once

#include "SimilarityCalculator.h"

// Jaccard metric: |A ∩ B| / |A ∪ B|.
class JaccardSimilarity : public SimilarityCalculator {
public:
    double calculate(const std::vector<uint64_t>& setA,
                     const std::vector<uint64_t>& setB) const override;
    std::string getMetricName() const override;
};
