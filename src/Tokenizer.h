#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_set>

// Converts source text into a token stream.
class Tokenizer {
private:
    std::unordered_set<std::string> keywords_;

    void initKeywords();
    bool isIdentifierStart(char ch) const;
    bool isIdentifierBody(char ch) const;
    bool isOperatorChar(char ch) const;
    void normalize(std::vector<Token>& tokens) const;

public:
    Tokenizer();
    std::vector<Token> tokenize(const std::string& source) const;
};
