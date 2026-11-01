#pragma once

#include <vector>
#include <string>
#include <cstdint>

// Interface for similarity metrics (lets metrics be swapped without changing the detector).
class SimilarityCalculator {
public:
    virtual ~SimilarityCalculator() = default;
    virtual double calculate(const std::vector<uint64_t>& setA,
                             const std::vector<uint64_t>& setB) const = 0;
    virtual std::string getMetricName() const = 0;
};
