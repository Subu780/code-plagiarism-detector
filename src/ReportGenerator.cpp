#include "ReportGenerator.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <string>

using namespace std;

// ANSI colour codes for terminal output.
namespace Color {
    const string RESET        = "\033[0m";
    const string BOLD         = "\033[1m";
    const string DIM          = "\033[2m";
    const string GRAY         = "\033[90m";
    const string CYAN         = "\033[36m";
    const string BRIGHT_CYAN  = "\033[1;96m";
    const string GREEN        = "\033[32m";
    const string BRIGHT_GREEN = "\033[1;92m";
    const string YELLOW       = "\033[33m";
    const string BRIGHT_YELLOW= "\033[1;93m";
    const string RED          = "\033[31m";
    const string BRIGHT_RED   = "\033[1;91m";
    const string WHITE        = "\033[1;97m";
}

ReportGenerator::ReportGenerator() {}

// Counts printable columns (ignores colour codes and UTF-8 continuation bytes).
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

// Width of the box interior in characters.
static const int INNER_WIDTH = 72;

// Top border, with an optional title.
static void printBorderTop(const string& title = "") {
    cout << "  " << Color::CYAN << "╭";
    int used = 0;
    if (!title.empty()) {
        string t = "─[ " + title + " ]";
        cout << t;
        used = static_cast<int>(visibleWidth(t));
    }
    for (int i = 0; i < INNER_WIDTH - used; ++i) cout << "─";
    cout << "╮" << Color::RESET << "\n";
}

// Horizontal divider line.
static void printBorderDivider() {
    cout << "  " << Color::CYAN << "├";
    for (int i = 0; i < INNER_WIDTH; ++i) cout << "─";
    cout << "┤" << Color::RESET << "\n";
}

// Bottom border.
static void printBorderBottom() {
    cout << "  " << Color::CYAN << "╰";
    for (int i = 0; i < INNER_WIDTH; ++i) cout << "─";
    cout << "╯" << Color::RESET << "\n";
}

// Prints one padded row; long rows wrap onto a second line.
static void printCardRow(const string& content) {
    size_t vis = visibleWidth(content);
    if (vis <= 68) {
        cout << "  " << Color::CYAN << "│" << Color::RESET << "  " << content;
        cout << string(68 - vis, ' ');
        cout << "  " << Color::CYAN << "│" << Color::RESET << "\n";
        return;
    }

    // Find the last space before the width limit to wrap at.
    size_t splitPos = string::npos;
    size_t curVis = 0;
    bool inEsc = false;

    for (size_t i = 0; i < content.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(content[i]);
        if (c == '\033') {
            inEsc = true;
        } else if (inEsc) {
            if (c == 'm') inEsc = false;
        } else {
            if ((c & 0xC0) != 0x80) curVis++;
        }

        if (curVis <= 65 && content[i] == ' ') {
            splitPos = i;
        }
        if (curVis >= 65) break;
    }

    if (splitPos != string::npos) {
        string part1 = content.substr(0, splitPos);
        string part2 = "              " + content.substr(splitPos + 1);
        printCardRow(part1);
        printCardRow(part2);
    } else {
        cout << "  " << Color::CYAN << "│" << Color::RESET << "  " << content.substr(0, 65) << "...";
        cout << "  " << Color::CYAN << "│" << Color::RESET << "\n";
    }
}

// Draws a bar like [■■■···], coloured green/yellow/red by score.
static string renderDistributionBar(double percent, int barWidth = 20) {
    if (percent < 0.0) percent = 0.0;
    if (percent > 100.0) percent = 100.0;
    int filled = static_cast<int>((percent / 100.0) * barWidth);

    string col = Color::BRIGHT_GREEN;
    if (percent >= 75.0) col = Color::BRIGHT_RED;
    else if (percent >= 45.0) col = Color::BRIGHT_YELLOW;

    string bar = col + "[";
    for (int i = 0; i < barWidth; ++i) {
        bar += (i < filled) ? "■" : "·";
    }
    bar += "]" + Color::RESET;
    return bar;
}

// Prints a file with line numbers; lines with matched tokens are marked and highlighted.
void ReportGenerator::printAnnotatedFile(const string& title,
                                        const string& filePath,
                                        const string& content,
                                        const vector<Token>& tokens) const {
    // Group tokens by line and remember which lines have matches.
    unordered_map<size_t, vector<Token>> tokensByLine;
    unordered_set<size_t> matchedLines;

    for (const auto& t : tokens) {
        tokensByLine[t.getLine()].push_back(t);
        if (t.isMatched()) {
            matchedLines.insert(t.getLine());
        }
    }

    cout << "\n";
    printBorderTop(title);

    string targetRow = Color::GRAY + "Target : " + Color::RESET + Color::WHITE + filePath + Color::RESET;
    printCardRow(targetRow);

    // Table header separator.
    cout << "  " << Color::CYAN << "├───────┬────────────────────────────────────────────────────────────────┤" << Color::RESET << "\n";

    stringstream ss(content);
    string rawLine;
    size_t lineNum = 1;

    // Go through the original file line by line.
    while (getline(ss, rawLine)) {
        if (!rawLine.empty() && rawLine.back() == '\r') rawLine.pop_back();

        string line;
        for (char ch : rawLine) {
            // Expand tabs to four spaces for display.
            if (ch == '\t') line += "    ";
            else line += ch;
        }

        // ▶ marker for flagged lines, plain number otherwise.
        bool hasMatch = (matchedLines.find(lineNum) != matchedLines.end());

        cout << "  " << Color::CYAN << "│" << Color::RESET;
        if (hasMatch) {
            cout << Color::BRIGHT_YELLOW << "▶ " << right << setw(4) << lineNum << " " << Color::RESET;
        } else {
            cout << Color::GRAY << "  " << right << setw(4) << lineNum << " " << Color::RESET;
        }

        cout << Color::CYAN << "│" << Color::RESET << " ";

        string codeBody;
        string disp = line;
        bool truncated = false;
        // Cut long lines so the box stays aligned.
        if (disp.size() > 62) {
            disp = disp.substr(0, 59);
            truncated = true;
        }

        // Unmatched line: show dimmed. Matched line: highlight matched tokens.
        if (!hasMatch) {
            codeBody = Color::DIM + disp + Color::RESET;
        } else {
            const auto& lTokens = tokensByLine[lineNum];
            size_t cursor = 0;

            for (const auto& tok : lTokens) {
                const string& raw = tok.getRawValue();
                if (raw.empty()) continue;

                size_t pos = disp.find(raw, cursor);
                if (pos != string::npos) {
                    if (pos > cursor) {
                        codeBody += Color::DIM + disp.substr(cursor, pos - cursor) + Color::RESET;
                    }
                    if (tok.isMatched()) {
                        codeBody += Color::BRIGHT_YELLOW + Color::BOLD + raw + Color::RESET;
                    } else {
                        codeBody += disp.substr(pos, raw.size());
                    }
                    cursor = pos + raw.size();
                }
            }
            if (cursor < disp.size()) {
                codeBody += Color::DIM + disp.substr(cursor) + Color::RESET;
            }
        }

        if (truncated) {
            codeBody += Color::DIM + "..." + Color::RESET;
        }

        // Pad to the box width.
        size_t vis = visibleWidth(codeBody);
        cout << codeBody;
        if (vis < 62) {
            cout << string(62 - vis, ' ');
        }

        cout << " " << Color::CYAN << "│" << Color::RESET << "\n";
        ++lineNum;
    }

    cout << "  " << Color::CYAN << "├───────┴────────────────────────────────────────────────────────────────┤" << Color::RESET << "\n";

    // Footer: number of flagged lines.
    string flagInfo = Color::GRAY + "Flagged : " + Color::RESET + Color::BRIGHT_YELLOW +
                      to_string(matchedLines.size()) + Color::RESET + Color::GRAY + " suspicious code lines detected" + Color::RESET;
    printCardRow(flagInfo);

    printBorderBottom();
}

// Prints both files, then the summary and verdict.
void ReportGenerator::printConsoleReport(const ComparisonResult& res) const {
    printAnnotatedFile("ARTIFACT A INSPECTION", res.file1, res.content1, res.tokens1);
    printAnnotatedFile("ARTIFACT B INSPECTION", res.file2, res.content2, res.tokens2);

    // Default verdict: clean.
    string badgePill = Color::BRIGHT_GREEN + "[ CLEAN ]" + Color::RESET;
    string statusText = Color::BRIGHT_GREEN + "Codebases Distinct" + Color::RESET;
    string evLine1 = "No structural, sequence, or lexical correlation";
    string evLine2 = "detected between codebases.";

    // Critical: Jaccard >= 75% or containment >= 80%.
    if (res.jaccardScore >= 75.0 || res.containmentScore >= 80.0) {
        badgePill = Color::BRIGHT_RED + "[ CRITICAL ]" + Color::RESET;
        statusText = Color::BRIGHT_RED + "Plagiarism Confirmed" + Color::RESET;
        evLine1 = "Systematic structural replication detected";
        evLine2 = "under identifier renaming.";
    } else if (res.jaccardScore >= 45.0 || res.containmentScore >= 55.0) {
        // Warning: Jaccard >= 45% or containment >= 55%.
        badgePill = Color::BRIGHT_YELLOW + "[ WARNING ]" + Color::RESET;
        statusText = Color::BRIGHT_YELLOW + "Suspicious Overlap" + Color::RESET;
        evLine1 = "Substantial shared token logic detected.";
        evLine2 = "Manual faculty review recommended.";
    }

    cout << "\n";
    // Summary card with token and k-gram counts.
    printBorderTop("DETECTION TELEMETRY SUMMARY");

    printCardRow(string(Color::WHITE) + "Artifact A  : " + Color::RESET + res.file1);
    printCardRow(string(Color::GRAY) + "              └─ Tokens: " + Color::WHITE + to_string(res.totalTokens1) +
                 Color::GRAY + "  │  K-Grams: " + Color::WHITE + to_string(res.kGramsGenerated1) + Color::RESET);

    printCardRow(string(Color::WHITE) + "Artifact B  : " + Color::RESET + res.file2);
    printCardRow(string(Color::GRAY) + "              └─ Tokens: " + Color::WHITE + to_string(res.totalTokens2) +
                 Color::GRAY + "  │  K-Grams: " + Color::WHITE + to_string(res.kGramsGenerated2) + Color::RESET);

    printCardRow(string(Color::WHITE) + "Matches     : " + Color::BRIGHT_CYAN + to_string(res.matchingKGrams) +
                 " identical k-gram hashes" + Color::RESET + Color::GRAY + " (k = " + to_string(res.k) + " window)" + Color::RESET);

    printBorderDivider();

    // Score table with distribution bars.
    printCardRow(string(Color::GRAY) + "SIMILARITY METRIC       SCORE         DISTRIBUTION PROFILE" + Color::RESET);

    printBorderDivider();

    string jaccardVal = to_string(res.jaccardScore).substr(0, 5) + " %";
    string jaccardRow = string("Jaccard Index           ") + Color::WHITE + jaccardVal + Color::RESET +
                        string(14 - jaccardVal.size(), ' ') + renderDistributionBar(res.jaccardScore, 20);
    printCardRow(jaccardRow);

    string contVal = to_string(res.containmentScore).substr(0, 5) + " %";
    string contRow = string("Containment Index       ") + Color::WHITE + contVal + Color::RESET +
                     string(14 - contVal.size(), ' ') + renderDistributionBar(res.containmentScore, 20);
    printCardRow(contRow);

    printBorderDivider();

    // Final verdict.
    printCardRow(string(Color::WHITE) + "AUDIT VERDICT" + Color::RESET);
    printCardRow(badgePill + "  " + statusText);
    printCardRow(string(Color::GRAY) + "Evidence    : " + Color::RESET + evLine1);
    if (!evLine2.empty()) {
        printCardRow(string(Color::GRAY) + "              " + Color::RESET + evLine2);
    }

    printBorderBottom();
    cout << "\n";
}

// Not used yet.
void ReportGenerator::printSummary(const vector<ComparisonResult>&) const {}
