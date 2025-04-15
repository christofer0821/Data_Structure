#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cctype>  // for isdigit

// Trim leading and trailing spaces
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    size_t last = str.find_last_not_of(" \t\r\n");
    if (first == std::string::npos || last == std::string::npos)
        return "";
    return str.substr(first, last - first + 1);
}

// Clean a cell: trim spaces and remove quotes
std::string clean_cell(const std::string& cell) {
    std::string result = trim(cell);
    if (!result.empty() && result.front() == '"' && result.back() == '"') {
        result = result.substr(1, result.size() - 2);
    }
    return trim(result);
}

// Split CSV row into cleaned cells
std::vector<std::string> split_csv_line(const std::string& line) {
    std::vector<std::string> cells;
    std::stringstream ss(line);
    std::string cell;
    while (std::getline(ss, cell, ',')) {
        cells.push_back(clean_cell(cell));
    }
    return cells;
}

// Check if a string is a valid number
bool is_valid_number(const std::string& str) {
    if (str.empty()) return false;
    std::istringstream iss(str);
    double d;
    return (iss >> d) && iss.eof();
}

// Validate critical fields: Customer ID, Price (numeric), Payment Method, and valid Date
bool has_valid_critical_fields(const std::vector<std::string>& row) {
    // Indexes: 0 = Customer ID, 3 = Price, 5 = Payment Method, 4 = Date
    return !row[0].empty() &&
           is_valid_number(row[3]) &&
           !row[5].empty() &&
           !row[4].empty() &&
           row[4] != "Invalid Date";
}

// Read, clean, and filter the CSV
std::vector<std::vector<std::string>> read_and_clean_csv(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::vector<std::string>> table;
    std::string line;
    bool isHeader = true;
    size_t expectedColCount = 0;

    std::unordered_set<std::string> uniqueRows;

    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return table;
    }

    while (std::getline(file, line)) {
        std::vector<std::string> row = split_csv_line(line);

        if (isHeader) {
            expectedColCount = row.size();
            table.push_back(row); // Always keep the header
            isHeader = false;
            continue;
        }

        if (row.size() != expectedColCount) continue;
        if (!has_valid_critical_fields(row)) continue;

        // Use a pipe-separated string as a unique key
        std::string key;
        for (const std::string& cell : row) key += cell + "|";
        if (uniqueRows.find(key) != uniqueRows.end()) continue;

        uniqueRows.insert(key);
        table.push_back(row);
    }

    file.close();
    return table;
}

// Save the cleaned table to CSV
void write_csv(const std::string& filename, const std::vector<std::vector<std::string>>& table) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot write to file: " << filename << std::endl;
        return;
    }

    for (const auto& row : table) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];
            if (i < row.size() - 1) file << ",";
        }
        file << "\n";
    }

    file.close();
}

int main() {
    std::string inputFile = "D:\\C++ FOLDER\\Final Assignment\\Data CSV\\transactions.csv";
    std::string outputFile = "D:\\C++ FOLDER\\Final Assignment\\Data CSV\\transactions_cleaned.csv";

    std::vector<std::vector<std::string>> cleanedData = read_and_clean_csv(inputFile);
    std::cout << "Total cleaned rows (including header): " << cleanedData.size() << std::endl;

    write_csv(outputFile, cleanedData);
    std::cout << "Cleaned CSV saved to: " << outputFile << std::endl;

    return 0;
}
