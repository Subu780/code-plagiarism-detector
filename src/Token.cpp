#include "Token.h"

using namespace std;

// Default token: unknown type, empty text.
Token::Token()
    : type_(TokenType::UNKNOWN), rawValue_(""), value_(""), line_(0), isMatched_(false) {}

// Token starts with value equal to its raw text and unmatched.
Token::Token(TokenType type, const string& value, size_t line)
    : type_(type), rawValue_(value), value_(value), line_(line), isMatched_(false) {}

TokenType Token::getType() const { return type_; }
string Token::getValue() const { return value_; }
string Token::getRawValue() const { return rawValue_; }
size_t Token::getLine() const { return line_; }
bool Token::isMatched() const { return isMatched_; }

void Token::setValue(const string& newValue) { value_ = newValue; }
void Token::setMatched(bool matched) { isMatched_ = matched; }

// Debug-friendly text form of a token.
string Token::toString() const {
    return "Token(" + typeToString(type_) + ", \"" + value_ + "\", Line " + to_string(line_) + ")";
}

// Converts a token type to its name.
string Token::typeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD: return "KEYWORD";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::OPERATOR: return "OPERATOR";
        case TokenType::SYMBOL: return "SYMBOL";
        default: return "UNKNOWN";
    }
}
