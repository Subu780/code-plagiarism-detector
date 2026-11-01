#pragma once

#include <string>

// Categories of lexical tokens.
enum class TokenType {
    UNKNOWN,
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    STRING_LITERAL,
    OPERATOR,
    SYMBOL
};

// One lexical token plus its source line and match flag.
class Token {
private:
    TokenType type_;
    // Original text, kept so the report can highlight the source.
    std::string rawValue_;
    // Normalised text (ID/NUM/STR for identifiers, numbers, strings) used for comparison.
    std::string value_;
    size_t line_;
    // True if this token is part of a k-gram found in the other file.
    bool isMatched_;

public:
    Token();
    Token(TokenType type, const std::string& value, size_t line);

    TokenType getType() const;
    std::string getValue() const;
    std::string getRawValue() const;
    size_t getLine() const;
    bool isMatched() const;

    void setValue(const std::string& newValue);
    void setMatched(bool matched);
    std::string toString() const;
    static std::string typeToString(TokenType type);
};
