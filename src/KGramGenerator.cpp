#include "KGramGenerator.h"

using namespace std;

// A k of 0 is invalid, so it is forced to 1.
KGramGenerator::KGramGenerator(size_t k) : k_(k == 0 ? 1 : k) {}
void KGramGenerator::setK(size_t k) { k_ = (k == 0 ? 1 : k); }
size_t KGramGenerator::getK() const { return k_; }

vector<string> KGramGenerator::generateKGrams(const vector<Token>& tokens) const {
    vector<string> kgrams;
    // Too few tokens to form even one k-gram.
    if (tokens.size() < k_) return kgrams;

    // Number of sliding windows = tokens - k + 1.
    size_t numGrams = tokens.size() - k_ + 1;
    kgrams.reserve(numGrams);

    // Join the k tokens of this window into one string.
    for (size_t i = 0; i < numGrams; ++i) {
        string gram;
        for (size_t j = 0; j < k_; ++j) {
            gram += tokens[i + j].getValue();
            if (j + 1 < k_) gram += "_";
        }
        kgrams.push_back(move(gram));
    }
    return kgrams;
}
