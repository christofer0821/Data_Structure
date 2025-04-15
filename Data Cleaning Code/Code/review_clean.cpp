#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>

// Trim leading/trailing spaces
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    size_t last = str.find_last_not_of(" \t\r\n");
    if (first == std::string::npos || last == std::string::npos)
        return "";
    return str.substr(first, last - first + 1);
}

// Clean and unquote cell
std::string clean_cell(const std::string& cell) {
    std::string result = trim(cell);
    if (!result.empty() && result.front() == '"' && result.back() == '"') {
        result = result.substr(1, result.size() - 2);
    }
    return trim(result);
}

// Parse a CSV line while preserving commas in quotes
std::vector<std::string> parse_csv_line(const std::string& line) {
    std::vector<std::string> cells;
    std::string cell;
    bool inside_quotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        if (c == '"') {
            inside_quotes = !inside_quotes;
        } else if (c == ',' && !inside_quotes) {
            cells.push_back(clean_cell(cell));
            cell.clear();
        } else {
            cell += c;
        }
    }
    cells.push_back(clean_cell(cell)); // last cell
    return cells;
}

// Check for invalid row
bool is_invalid_row(const std::vector<std::string>& row, size_t ratingIndex) {
    for (const auto& cell : row) {
        if (cell.empty()) return true;
    }
    return (ratingIndex < row.size() && row[ratingIndex] == "Invalid Rating");
}

// Read and clean CSV
std::vector<std::vector<std::string>> read_and_clean_csv(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::vector<std::string>> table;
    std::string line;
    bool isHeader = true;
    size_t ratingIndex = 0;
    size_t expectedColCount = 0;
    std::unordered_set<std::string> seenRows;

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return table;
    }

    while (std::getline(file, line)) {
        std::vector<std::string> row = parse_csv_line(line);

        if (isHeader) {
            expectedColCount = row.size();
            for (size_t i = 0; i < row.size(); ++i) {
                if (row[i] == "Rating") ratingIndex = i;
            }
            table.push_back(row);
            isHeader = false;
            continue;
        }

        if (row.size() != expectedColCount) continue;
        if (is_invalid_row(row, ratingIndex)) continue;

        std::string key;
        for (const auto& cell : row) key += cell + "|";
        if (seenRows.find(key) != seenRows.end()) continue;

        seenRows.insert(key);
        table.push_back(row);
    }

    file.close();
    return table;
}

// Write output CSV
void write_csv(const std::string& filename, const std::vector<std::vector<std::string>>& table) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error writing file: " << filename << std::endl;
        return;
    }

    for (const auto& row : table) {
        for (size_t i = 0; i < row.size(); ++i) {
            // Quote cells that contain commas
            if (row[i].find(',') != std::string::npos)
                file << "\"" << row[i] << "\"";
            else
                file << row[i];

            if (i < row.size() - 1) file << ",";
        }
        file << "\n";
    }

    file.close();
}

int main() {
    std::string inputFile = "D:\\C++ FOLDER\\Final Assignment\\Data CSV\\reviews.csv";
    std::string outputFile = "D:\\C++ FOLDER\\Final Assignment\\Data CSV\\reviews_cleaned.csv";

    auto cleanedData = read_and_clean_csv(inputFile);
    std::cout << "Cleaned rows (with header): " << cleanedData.size() << std::endl;

    write_csv(outputFile, cleanedData);
    std::cout << "Saved cleaned file to: " << outputFile << std::endl;

    return 0;
}
