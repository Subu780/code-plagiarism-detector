#pragma once

#include <string>

// Cleans raw source before tokenizing (comments, directives, whitespace).
class Preprocessor {
private:
    std::string removeComments(const std::string& source) const;
    std::string removeDirectives(const std::string& source) const;
    std::string normalizeWhitespace(const std::string& source) const;

public:
    Preprocessor();
    std::string process(const std::string& rawSource) const;
};
