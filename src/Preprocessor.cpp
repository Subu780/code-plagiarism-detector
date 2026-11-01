#include "Preprocessor.h"
#include <sstream>

using namespace std;

// Nothing to initialise; the class is stateless.
Preprocessor::Preprocessor() {}

// Strips // and /* */ comments but keeps string/char literals intact and preserves newlines (so line numbers stay correct).
string Preprocessor::removeComments(const string& source) const {
    string result;
    result.reserve(source.size());

    // State flags for the character-by-character scan.
    bool inString = false;
    bool inChar = false;
    bool inSingleComment = false;
    bool inBlockComment = false;
    size_t length = source.size();

    for (size_t i = 0; i < length; ++i) {
        char current = source[i];
        // Look-ahead character (used to spot // and /*).
        char next = (i + 1 < length) ? source[i + 1] : '\0';

        // Inside // comment: skip until end of line.
        if (inSingleComment) {
            if (current == '\n') {
                inSingleComment = false;
                result += '\n';
            }
            continue;
        }

        // Inside /* */ comment: skip until the closing */.
        if (inBlockComment) {
            if (current == '*' && next == '/') {
                inBlockComment = false;
                ++i;
            } else if (current == '\n') {
                result += '\n';
            }
            continue;
        }

        // Inside a string literal: copy it, honouring backslash escapes.
        if (inString) {
            result += current;
            if (current == '\\' && i + 1 < length) result += source[++i];
            else if (current == '"') inString = false;
            continue;
        }

        // Inside a char literal: same handling as strings.
        if (inChar) {
            result += current;
            if (current == '\\' && i + 1 < length) result += source[++i];
            else if (current == '\'') inChar = false;
            continue;
        }

        // Start of a // comment.
        if (current == '/' && next == '/') {
            inSingleComment = true;
            ++i;
            continue;
        }

        // Start of a /* */ comment.
        if (current == '/' && next == '*') {
            inBlockComment = true;
            ++i;
            continue;
        }

        // Start of a string literal.
        if (current == '"') {
            inString = true;
            result += current;
            continue;
        }

        // Start of a char literal.
        if (current == '\'') {
            inChar = true;
            result += current;
            continue;
        }

        // Ordinary code character: keep it.
        result += current;
    }
    return result;
}

// Blanks out preprocessor lines (#include, #define, ...) but keeps the line so numbering is unchanged.
string Preprocessor::removeDirectives(const string& source) const {
    stringstream inputStream(source);
    string line, result;

    while (getline(inputStream, line)) {
        size_t firstNonSpace = line.find_first_not_of(" \t\r");
        if (firstNonSpace != string::npos && line[firstNonSpace] == '#') {
            result += '\n';
            continue;
        }
        result += line;
        result += '\n';
    }
    return result;
}

// Trims each line and collapses runs of spaces/tabs into a single space.
string Preprocessor::normalizeWhitespace(const string& source) const {
    stringstream inputStream(source);
    string line, result;

    while (getline(inputStream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();

        size_t first = line.find_first_not_of(" \t");
        if (first == string::npos) {
            result += '\n';
            continue;
        }

        size_t last = line.find_last_not_of(" \t");
        string trimmed = line.substr(first, (last - first + 1));
        string normalizedLine;
        bool inSpace = false;

        for (char ch : trimmed) {
            if (ch == ' ' || ch == '\t') {
                if (!inSpace) {
                    normalizedLine += ' ';
                    inSpace = true;
                }
            } else {
                normalizedLine += ch;
                inSpace = false;
            }
        }
        result += normalizedLine + '\n';
    }
    return result;
}

// Full cleaning pipeline: remove comments -> remove directives -> normalise whitespace.
string Preprocessor::process(const string& rawSource) const {
    return normalizeWhitespace(removeDirectives(removeComments(rawSource)));
}
