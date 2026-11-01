#pragma once

#include "Token.h"
#include <string>
#include <vector>

// Everything produced by one comparison: scores, counts and token data.
struct ComparisonResult {
    std::string file1;
    std::string file2;
    std::string content1;
    std::string content2;
    std::vector<Token> tokens1;
    std::vector<Token> tokens2;
    double jaccardScore;
    double containmentScore;
    size_t totalTokens1;
    size_t totalTokens2;
    size_t kGramsGenerated1;
    size_t kGramsGenerated2;
    size_t matchingKGrams;
    size_t k = 4;
};

// Prints the coloured console report.
class ReportGenerator {
private:
    // Prints a file with its suspicious lines highlighted.
    void printAnnotatedFile(const std::string& title,
                            const std::string& filePath,
                            const std::string& content,
                            const std::vector<Token>& tokens) const;

public:
    ReportGenerator();
    // Prints both annotated files and the summary card.
    void printConsoleReport(const ComparisonResult& res) const;
    // Reserved for multi-file summaries (currently unused).
    void printSummary(const std::vector<ComparisonResult>& results) const;
};
