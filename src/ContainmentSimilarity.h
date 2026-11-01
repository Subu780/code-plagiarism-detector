#pragma once

#include "SimilarityCalculator.h"

// Containment metric: shared k-grams relative to the smaller file.
class ContainmentSimilarity : public SimilarityCalculator {
public:
    double calculate(const std::vector<uint64_t>& setA,
                     const std::vector<uint64_t>& setB) const override;
    std::string getMetricName() const override;
};
