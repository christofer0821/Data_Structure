#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <chrono>
using namespace std;
using namespace chrono;

const int MAX_WORDS = 10000;
const int MAX_STOPS = 50;

struct WordCount {
    string word;
    int count;
};

// Function Prototypes
string toLowerCase(string text);
bool isValidChar(char c);
bool isStopWord(string word);
int findWordIndex(WordCount wordList[], int size, string word, bool& found);
void extractWords(string line, WordCount wordList[], int& wordTotal);
void mergeByFrequency(WordCount arr[], int left, int mid, int right);
void mergeSortByFrequency(WordCount arr[], int left, int right);

// List of common stop words
string stopWords[MAX_STOPS] = {
    "the", "is", "a", "an", "to", "and", "in", "on", "for", "of",
    "with", "at", "by", "from", "up", "about", "into", "over", "after",
    "not", "be", "this", "that", "it", "as", "are", "was", "but", "or",
    "you", "i", "we", "they", "he", "she", "so", "very", "too", "if",
    "out", "no", "do", "did", "has", "have", "had", "can", "will", "just", "my"
};

// Convert string to lowercase
string toLowerCase(string text) {
    for (int i = 0; i < text.length(); i++)
        text[i] = tolower(text[i]);
    return text;
}

// Check if a character is alphanumeric
bool isValidChar(char c) {
    return isalpha(c) || isdigit(c);
}

// Check if a word is in the stop word list
bool isStopWord(string word) {
    for (int i = 0; i < MAX_STOPS; i++) {
        if (stopWords[i] == word)
            return true;
    }
    return false;
}

// Binary search for word position or insert point
int findWordIndex(WordCount wordList[], int size, string word, bool& found) {
    int left = 0, right = size - 1;
    found = false;

    while (left <= right) {
        int mid = (left + right) / 2;
        if (wordList[mid].word == word) {
            found = true;
            return mid;
        } else if (wordList[mid].word < word) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return left;
}

// Extract words from a line and count their frequency
void extractWords(string line, WordCount wordList[], int& wordTotal) {
    string word = "";

    for (int i = 0; i <= line.length(); i++) {
        char c = line[i];

        if (isValidChar(c)) {
            word += tolower(c);
        } else if (!word.empty()) {
            if (!isStopWord(word)) {
                bool found;
                int index = findWordIndex(wordList, wordTotal, word, found);

                if (found) {
                    wordList[index].count++;
                } else if (wordTotal < MAX_WORDS) {
                    for (int j = wordTotal; j > index; j--) {
                        wordList[j] = wordList[j - 1];
                    }
                    wordList[index].word = word;
                    wordList[index].count = 1;
                    wordTotal++;
                }
            }
            word = "";
        }
    }
}

// Merge step for merge sort (sorts by count descending)
void mergeByFrequency(WordCount arr[], int left, int mid, int right) {
    int n1 = mid - left + 1, n2 = right - mid;
    WordCount* L = new WordCount[n1];
    WordCount* R = new WordCount[n2];

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].count > R[j].count)
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    delete[] L;
    delete[] R;
}

// Recursive merge sort to sort word list by frequency
void mergeSortByFrequency(WordCount arr[], int left, int right) {
    if (left < right) {
        int mid = (left + right) / 2;
        mergeSortByFrequency(arr, left, mid);
        mergeSortByFrequency(arr, mid + 1, right);
        mergeByFrequency(arr, left, mid, right);
    }
}

int main() {
    auto start = high_resolution_clock::now();

    string filename = "D:/C++ FOLDER/Final Assignment/Data CSV/reviews_cleaned.csv";
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open file!" << endl;
        return 1;
    }

    string line;
    getline(file, line); // Skip header

    WordCount wordList[MAX_WORDS];
    int wordTotal = 0;

    // Read file line-by-line and process 1-star reviews
    while (getline(file, line)) {
        stringstream ss(line);
        string productID, customerID, ratingStr, reviewText;
        getline(ss, productID, ',');
        getline(ss, customerID, ',');
        getline(ss, ratingStr, ',');
        getline(ss, reviewText);

        int rating = stoi(ratingStr);
        if (rating == 1) {
            extractWords(reviewText, wordList, wordTotal);
        }
    }

    file.close();

    // Sort words by frequency
    mergeSortByFrequency(wordList, 0, wordTotal - 1);

    // Display top 10 most frequent words
    cout << "\nTop 10 Frequent Words in 1-Star Reviews:\n";
    for (int i = 0; i < 10 && i < wordTotal; i++) {
        cout << wordList[i].word << " : " << wordList[i].count << endl;
    }

    auto end = high_resolution_clock::now();
    auto durationMs = duration_cast<milliseconds>(end - start);

    // Estimate memory usage
    size_t memoryUsed = sizeof(WordCount) * wordTotal;

    cout << "\nExecution time: " << durationMs.count() << " ms" << endl;
    cout << "Approximate memory used: " << memoryUsed << " bytes" << endl;

    return 0;
}
