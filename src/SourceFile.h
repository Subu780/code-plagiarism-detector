#pragma once

#include <string>

// Represents one source file and its loaded text.
class SourceFile {
private:
    std::string filePath_;
    std::string content_;
    bool isLoaded_;

public:
    SourceFile();
    explicit SourceFile(const std::string& filePath);

    bool load();
    std::string getFilePath() const;
    std::string getContent() const;
    bool isLoaded() const;
    void clear();
};
