#include "SourceFile.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

SourceFile::SourceFile() : filePath_(""), content_(""), isLoaded_(false) {}

SourceFile::SourceFile(const string& filePath)
    : filePath_(filePath), content_(""), isLoaded_(false) {}

// Reads the whole file into memory; returns false (with an error message) on failure.
bool SourceFile::load() {
    if (filePath_.empty()) {
        cerr << "Error: File path is empty.\n";
        isLoaded_ = false;
        return false;
    }

    ifstream fileStream(filePath_);
    if (!fileStream.is_open()) {
        cerr << "Error: Unable to open file: " << filePath_ << "\n";
        isLoaded_ = false;
        return false;
    }

    stringstream buffer;
    buffer << fileStream.rdbuf();
    content_ = buffer.str();
    isLoaded_ = true;
    return true;
}

string SourceFile::getFilePath() const { return filePath_; }
string SourceFile::getContent() const { return content_; }
bool SourceFile::isLoaded() const { return isLoaded_; }

// Resets the object to its unloaded state.
void SourceFile::clear() {
    content_.clear();
    isLoaded_ = false;
}
