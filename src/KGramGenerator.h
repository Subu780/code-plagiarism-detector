#pragma once

#include "Token.h"
#include <string>
#include <vector>

// Builds k-grams (sliding windows of k consecutive tokens).
class KGramGenerator {
private:
    size_t k_;

public:
    explicit KGramGenerator(size_t k = 4);
    void setK(size_t k);
    size_t getK() const;
    // Returns one string per window, tokens joined by '_'.
    std::vector<std::string> generateKGrams(const std::vector<Token>& tokens) const;
};
