#include "Tokenizer.h"
#include <cctype>

using namespace std;

// Loads the keyword table on construction.
Tokenizer::Tokenizer() { initKeywords(); }

// Fills the set of C/C++ keywords; these are kept as-is during normalisation.
void Tokenizer::initKeywords() {
    keywords_ = {
        "auto", "break", "case", "char", "const", "continue", "default", "do",
        "double", "else", "enum", "extern", "float", "for", "goto", "if",
        "int", "long", "register", "return", "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while",
        "bool", "catch", "class", "constexpr", "delete", "explicit", "export",
        "false", "friend", "inline", "mutable", "namespace", "new", "noexcept",
        "nullptr", "operator", "private", "protected", "public", "static_assert",
        "template", "this", "throw", "true", "try", "typename", "using", "virtual"
    };
}

// Identifiers start with a letter or underscore.
bool Tokenizer::isIdentifierStart(char ch) const {
    return isalpha(static_cast<unsigned char>(ch)) || ch == '_';
}

// Identifiers continue with letters, digits or underscore.
bool Tokenizer::isIdentifierBody(char ch) const {
    return isalnum(static_cast<unsigned char>(ch)) || ch == '_';
}

// True for characters that can begin an operator.
bool Tokenizer::isOperatorChar(char ch) const {
    static const string ops = "=+-*/%<>!&|^~?:.";
    return ops.find(ch) != string::npos;
}

// Normalises tokens so renamed variables, changed numbers and changed strings still match.
void Tokenizer::normalize(vector<Token>& tokens) const {
    for (auto& token : tokens) {
        switch (token.getType()) {
            case TokenType::IDENTIFIER: token.setValue("ID"); break;
            case TokenType::NUMBER:     token.setValue("NUM"); break;
            case TokenType::STRING_LITERAL: token.setValue("STR"); break;
            default: break;
        }
    }
}

// Scans the cleaned source and produces a list of tokens with line numbers.
vector<Token> Tokenizer::tokenize(const string& source) const {
    vector<Token> tokens;
    size_t length = source.size(), i = 0, line = 1;

    while (i < length) {
        char ch = source[i];

        // Track line numbers and skip whitespace.
        if (ch == '\n') { ++line; ++i; continue; }
        if (isspace(static_cast<unsigned char>(ch))) { ++i; continue; }

        // Identifier or keyword.
        if (isIdentifierStart(ch)) {
            size_t start = i;
            while (i < length && isIdentifierBody(source[i])) ++i;
            string word = source.substr(start, i - start);
            TokenType type = (keywords_.find(word) != keywords_.end()) 
                             ? TokenType::KEYWORD : TokenType::IDENTIFIER;
            tokens.emplace_back(type, word, line);
            continue;
        }

        // Number: hex (0x..) or decimal/float.
        if (isdigit(static_cast<unsigned char>(ch))) {
            size_t start = i;
            if (ch == '0' && i + 1 < length && (source[i + 1] == 'x' || source[i + 1] == 'X')) {
                i += 2;
                while (i < length && isxdigit(static_cast<unsigned char>(source[i]))) ++i;
            } else {
                while (i < length && (isdigit(static_cast<unsigned char>(source[i])) || source[i] == '.')) ++i;
            }
            tokens.emplace_back(TokenType::NUMBER, source.substr(start, i - start), line);
            continue;
        }

        // String or char literal (handles escaped quotes).
        if (ch == '"' || ch == '\'') {
            char quote = ch;
            size_t start = i++;
            while (i < length && source[i] != quote) {
                if (source[i] == '\\' && i + 1 < length) ++i;
                if (source[i] == '\n') ++line;
                ++i;
            }
            if (i < length) ++i;
            tokens.emplace_back(TokenType::STRING_LITERAL, source.substr(start, i - start), line);
            continue;
        }

        // Operator: try two-character operators first (==, <=, ->, ...), then single.
        if (isOperatorChar(ch)) {
            size_t start = i;
            if (i + 1 < length) {
                string twoChar = source.substr(i, 2);
                static const unordered_set<string> multiOps = {
                    "==", "!=", "<=", ">=", "&&", "||", "++", "--",
                    "<<", ">>", "->", "+=", "-=", "*=", "/=", "%="
                };
                if (multiOps.find(twoChar) != multiOps.end()) {
                    i += 2;
                    tokens.emplace_back(TokenType::OPERATOR, twoChar, line);
                    continue;
                }
            }
            ++i;
            tokens.emplace_back(TokenType::OPERATOR, source.substr(start, 1), line);
            continue;
        }

        // Punctuation: ; { } ( ) [ ] , :
        if (ch == ';' || ch == '{' || ch == '}' || ch == '(' || ch == ')' ||
            ch == '[' || ch == ']' || ch == ',' || ch == ':') {
            tokens.emplace_back(TokenType::SYMBOL, string(1, ch), line);
            ++i;
            continue;
        }
        // Unknown character: skip it.
        ++i;
    }

    // Replace identifiers/numbers/strings with generic placeholders.
    normalize(tokens);
    return tokens;
}
