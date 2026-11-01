#include "PlagiarismDetector.h"
#include <iostream>
#include <unordered_set>

using namespace std;

// Sets up the k-gram size and both similarity calculators.
PlagiarismDetector::PlagiarismDetector(size_t k)
    : kgramGen_(k),
      jaccardCalc_(make_unique<JaccardSimilarity>()),
      containmentCalc_(make_unique<ContainmentSimilarity>()) {}

// Runs the full pipeline on two files and prints the report.
ComparisonResult PlagiarismDetector::compare(const string& path1, const string& path2) {
    SourceFile file1(path1);
    SourceFile file2(path2);

    ComparisonResult res;
    res.file1 = path1;
    res.file2 = path2;

    // Stop early if either file cannot be read.
    if (!file1.load() || !file2.load()) {
        cerr << "Error: Pipeline failed to load one or both source files.\n";
        return res;
    }

    res.content1 = file1.getContent();
    res.content2 = file2.getContent();

    // Clean the code (no comments, directives or extra whitespace).
    string clean1 = preprocessor_.process(res.content1);
    string clean2 = preprocessor_.process(res.content2);

    // Turn the cleaned code into normalised tokens.
    vector<Token> tokens1 = tokenizer_.tokenize(clean1);
    vector<Token> tokens2 = tokenizer_.tokenize(clean2);

    // Build k-grams from tokens.
    vector<string> grams1 = kgramGen_.generateKGrams(tokens1);
    vector<string> grams2 = kgramGen_.generateKGrams(tokens2);

    // Hash each k-gram into a number for fast comparison.
    vector<uint64_t> hashes1 = hasher_.hashKGrams(grams1);
    vector<uint64_t> hashes2 = hasher_.hashKGrams(grams2);

    size_t k = kgramGen_.getK();
    res.k = k;

    // Mark tokens of file 1 whose k-gram also appears in file 2.
    unordered_set<uint64_t> set2(hashes2.begin(), hashes2.end());
    for (size_t i = 0; i < hashes1.size(); ++i) {
        if (set2.find(hashes1[i]) != set2.end()) {
            for (size_t off = 0; off < k && (i + off) < tokens1.size(); ++off) {
                tokens1[i + off].setMatched(true);
            }
        }
    }

    // Same marking for file 2 against file 1.
    unordered_set<uint64_t> set1(hashes1.begin(), hashes1.end());
    for (size_t j = 0; j < hashes2.size(); ++j) {
        if (set1.find(hashes2[j]) != set1.end()) {
            for (size_t off = 0; off < k && (j + off) < tokens2.size(); ++off) {
                tokens2[j + off].setMatched(true);
            }
        }
    }

    // Count distinct k-gram hashes shared by both files.
    size_t intersectionCount = 0;
    for (uint64_t h : set1) {
        if (set2.find(h) != set2.end()) {
            ++intersectionCount;
        }
    }

    // Store results for the report.
    res.tokens1 = tokens1;
    res.tokens2 = tokens2;
    res.totalTokens1 = tokens1.size();
    res.totalTokens2 = tokens2.size();
    res.kGramsGenerated1 = grams1.size();
    res.kGramsGenerated2 = grams2.size();
    res.matchingKGrams = intersectionCount;

    // Compute both similarity scores.
    res.jaccardScore = jaccardCalc_->calculate(hashes1, hashes2);
    res.containmentScore = containmentCalc_->calculate(hashes1, hashes2);

    // Print the report.
    reportGen_.printConsoleReport(res);
    return res;
}
