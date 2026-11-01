#include "PlagiarismDetector.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdio>

using namespace std;

// Counts printable columns in a string (skips ANSI colour codes and UTF-8 continuation bytes).
static size_t visibleWidth(const string& s) {
    size_t w = 0;
    bool inEsc = false;
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c == '\033') {
            inEsc = true;
        } else if (inEsc) {
            if (c == 'm') inEsc = false;
        } else {
            if ((c & 0xC0) != 0x80) {
                w++;
            }
        }
    }
    return w;
}

// Strips leading/trailing whitespace.
static string trimString(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

// Returns true if the line is an end-of-input marker (END, DONE, EOF, :Q, // END).
static bool isSentinel(const string& rawLine) {
    string s = trimString(rawLine);
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return toupper(c);
    });
    return (s == "END" || s == "DONE" || s == "EOF" || s == ":Q" || s == "// END" || s == "//END");
}

// Prints one padded row inside the paste-instructions card.
static void printPromptRow(const string& text, int innerWidth = 58) {
    size_t vis = visibleWidth(text);
    cout << "  \033[1;36m│\033[0m  " << text;
    if (vis < static_cast<size_t>(innerWidth - 4)) {
        cout << string(innerWidth - 4 - vis, ' ');
    }
    cout << "  \033[1;36m│\033[0m\n";
}

// Draws the instruction card shown before pasting code.
static void printPromptCard(const string& artifactName) {
    const int INNER = 58;
    string title = "─[ " + artifactName + " ]";
    int titleVis = static_cast<int>(visibleWidth(title));
    int fill = INNER - titleVis;
    if (fill < 0) fill = 0;

    cout << "\n  \033[1;36m╭" << title;
    for (int i = 0; i < fill; ++i) cout << "─";
    cout << "╮\033[0m\n";

    printPromptRow("Paste your C++ code below.", INNER);
    printPromptRow("When done, type \033[1;33mEND\033[0m on a new line and press \033[1;33mEnter\033[0m.", INNER);
    printPromptRow("\033[90m(Or hit Enter once, then press Ctrl+D)\033[0m", INNER);

    cout << "  \033[1;36m╰";
    for (int i = 0; i < INNER; ++i) cout << "─";
    cout << "╯\033[0m\n\n";
}

// Reads pasted code from the terminal until the user types a sentinel or hits EOF.
static string readPastedCode(const string& artifactName) {
    printPromptCard(artifactName);

    string fullCode;
    string line;
    size_t lineCount = 0;

    while (true) {
        if (!getline(cin, line)) {
            break;
        }

        string trimmed = trimString(line);

        if (isSentinel(trimmed)) {
            break;
        }

        // Also accepts a trailing END glued to code, e.g. "}END".
        if (trimmed.size() >= 3) {
            string upper = trimmed;
            transform(upper.begin(), upper.end(), upper.begin(), [](unsigned char c) { return toupper(c); });
            if (upper.rfind("END") == upper.size() - 3) {
                string prefix = trimString(trimmed.substr(0, trimmed.size() - 3));
                bool onlyDelims = true;
                for (char c : prefix) {
                    if (c != '}' && c != ';' && c != ')') {
                        onlyDelims = false;
                        break;
                    }
                }
                if (onlyDelims && !prefix.empty()) {
                    fullCode += prefix + "\n";
                    lineCount++;
                    break;
                }
            }
        }

        fullCode += line + "\n";
        lineCount++;
    }

    // After Ctrl+D, reattach stdin to the terminal so later prompts still work.
    if (cin.eof()) {
        cin.clear();
        freopen("/dev/tty", "r", stdin);
    }

    cout << "  \033[1;32m✓ " << artifactName << " captured (" << lineCount << " lines)\033[0m\n";
    return fullCode;
}

// Saves pasted code to a file so the detector can load it like a normal source file.
static bool writeSnippetFile(const string& filePath, const string& code) {
    ofstream out(filePath);
    if (!out.is_open()) return false;
    out << code;
    out.close();
    return true;
}

// Entry point: CLI mode (detector file1 file2 [k]) or interactive menu.
int main(int argc, char* argv[]) {
    // Default k-gram window size (number of tokens per k-gram).
    size_t k = 4;

    // CLI mode: two file paths given, optional third argument overrides k.
    if (argc >= 3) {
        string file1 = argv[1];
        string file2 = argv[2];
        if (argc >= 4) {
            k = static_cast<size_t>(stoi(argv[3]));
        }
        PlagiarismDetector detector(k);
        detector.compare(file1, file2);
        return 0;
    }

    // Title banner.
    cout << "\n\033[1;36m  ╭────────────────────────────────────────╮\033[0m\n";
    cout << "\033[1;36m  │        CODE PLAGIARISM DETECTOR        │\033[0m\n";
    cout << "\033[1;36m  ╰────────────────────────────────────────╯\033[0m\n\n";
    // Menu options.
    cout << "  \033[1m1.\033[0m Run default demo comparison (tests/orig.cpp vs tests/plagiarized_renamed.cpp)\n";
    cout << "  \033[1m2.\033[0m Paste custom source codes directly into terminal\n";
    cout << "  \033[1m3.\033[0m Enter file paths manually\n\n";
    cout << "  \033[1;33mSelect mode (1/2/3) [default: 1]: \033[0m";

    // Read the user's menu choice (anything other than 2 or 3 runs the demo).
    string choice;
    getline(cin, choice);

    PlagiarismDetector detector(k);

    // Mode 2: paste both snippets, save them under build/, then compare.
    if (choice == "2") {
        string codeA = readPastedCode("Artifact A");
        string codeB = readPastedCode("Artifact B");

        string pathA = "build/snippet_a.cpp";
        string pathB = "build/snippet_b.cpp";

        if (!writeSnippetFile(pathA, codeA) || !writeSnippetFile(pathB, codeB)) {
            cerr << "Error: Could not save pasted snippets to build/ directory.\n";
            return 1;
        }

        detector.compare(pathA, pathB);
    } else if (choice == "3") {
        // Mode 3: ask for two file paths and compare them.
        cout << "\n  Path to first file  : ";
        string f1;
        getline(cin, f1);

        cout << "  Path to second file : ";
        string f2;
        getline(cin, f2);

        detector.compare(f1, f2);
    } else {
        // Mode 1 (default): compare the bundled demo files.
        cout << "\n  \033[32mLaunching default benchmark suite...\033[0m\n";
        detector.compare("tests/orig.cpp", "tests/plagiarized_renamed.cpp");
    }

    return 0;
}
