#pragma once

#include "SourceFile.h"
#include "Preprocessor.h"
#include "Tokenizer.h"
#include "KGramGenerator.h"
#include "Hasher.h"
#include "JaccardSimilarity.h"
#include "ContainmentSimilarity.h"
#include "ReportGenerator.h"
#include <memory>

// Facade that runs the whole pipeline: load -> clean -> tokenize -> k-grams -> hash -> compare -> report.
class PlagiarismDetector {
private:
    // Pipeline stages.
    Preprocessor preprocessor_;
    Tokenizer tokenizer_;
    KGramGenerator kgramGen_;
    Hasher hasher_;
    // Similarity metrics, held via the common interface.
    std::unique_ptr<SimilarityCalculator> jaccardCalc_;
    std::unique_ptr<SimilarityCalculator> containmentCalc_;
    ReportGenerator reportGen_;

public:
    // k = k-gram window size.
    explicit PlagiarismDetector(size_t k = 4);
    // Compares two source files and returns the scores and token data.
    ComparisonResult compare(const std::string& path1, const std::string& path2);
};
