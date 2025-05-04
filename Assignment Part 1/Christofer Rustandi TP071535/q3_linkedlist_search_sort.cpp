#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <chrono>
using namespace std;
using namespace chrono;

const int TABLE_SIZE = 101;
const int MAX_STOPS = 50;

struct WordNode {
    string word;
    int count;
    WordNode* next;
};

// === Function Prototypes ===
string toLowerCase(string text);
bool isValidChar(char c);
bool isStopWord(string word);
int hashFunc(string word);
void insertToHash(string word);
void transferToLinkedList();
WordNode* sortedMerge(WordNode* a, WordNode* b);
void splitList(WordNode* source, WordNode** front, WordNode** back);
void mergeSort(WordNode** headRef);
void extractWords(string line);
// ===========================

string stopWords[MAX_STOPS] = {
    "the", "is", "a", "an", "to", "and", "in", "on", "for", "of",
    "with", "at", "by", "from", "up", "about", "into", "over", "after",
    "not", "be", "this", "that", "it", "as", "are", "was", "but", "or",
    "you", "i", "we", "they", "he", "she", "so", "very", "too", "if",
    "out", "no", "do", "did", "has", "have", "had", "can", "will", "just", "my"
};

// Hash table to store words
WordNode* hashTable[TABLE_SIZE] = {nullptr};

// Linked list head (used for sorting after hash table)
WordNode* wordListHead = nullptr;

string toLowerCase(string text) {
    for (char& c : text)
        c = tolower(c);
    return text;
}

bool isValidChar(char c) {
    return isalpha(c) || isdigit(c);
}

bool isStopWord(string word) {
    for (int i = 0; i < MAX_STOPS; i++)
        if (stopWords[i] == word)
            return true;
    return false;
}

int hashFunc(string word) {
    int sum = 0;
    for (char c : word) sum += c;
    return sum % TABLE_SIZE;
}

void insertToHash(string word) {
    int index = hashFunc(word);
    WordNode* curr = hashTable[index];

    while (curr != nullptr) {
        if (curr->word == word) {
            curr->count++;
            return;
        }
        curr = curr->next;
    }

    // If not found, create new node
    WordNode* newNode = new WordNode{word, 1, hashTable[index]};
    hashTable[index] = newNode;
}

void transferToLinkedList() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        WordNode* curr = hashTable[i];
        while (curr) {
            WordNode* newNode = new WordNode{curr->word, curr->count, wordListHead};
            wordListHead = newNode;
            curr = curr->next;
        }
    }
}

WordNode* sortedMerge(WordNode* a, WordNode* b) {
    if (!a) return b;
    if (!b) return a;

    WordNode* result = nullptr;
    if (a->count > b->count) {
        result = a;
        result->next = sortedMerge(a->next, b);
    } else {
        result = b;
        result->next = sortedMerge(a, b->next);
    }
    return result;
}

void splitList(WordNode* source, WordNode** front, WordNode** back) {
    WordNode* slow = source;
    WordNode* fast = source->next;

    while (fast) {
        fast = fast->next;
        if (fast) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *front = source;
    *back = slow->next;
    slow->next = nullptr;
}

void mergeSort(WordNode** headRef) {
    WordNode* head = *headRef;
    if (!head || !head->next) return;

    WordNode* a;
    WordNode* b;
    splitList(head, &a, &b);
    mergeSort(&a);
    mergeSort(&b);
    *headRef = sortedMerge(a, b);
}

void extractWords(string line) {
    string word = "";
    for (char c : line + " ") { // Add space to process last word
        if (isValidChar(c)) {
            word += tolower(c);
        } else if (!word.empty()) {
            if (!isStopWord(word)) {
                insertToHash(word);
            }
            word = "";
        }
    }
}

// === MAIN ===
int main() {
    auto start = high_resolution_clock::now(); // Start timing

    string filename = "D:/C++ FOLDER/Final Assignment/Data CSV/reviews_cleaned.csv";
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open file!\n";
        return 1;
    }

    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        stringstream ss(line);
        string productID, customerID, ratingStr, reviewText;
        getline(ss, productID, ',');
        getline(ss, customerID, ',');
        getline(ss, ratingStr, ',');
        getline(ss, reviewText);

        int rating = stoi(ratingStr);
        if (rating == 1) {
            extractWords(reviewText);
        }
    }

    file.close();

    transferToLinkedList();      // Move from hash table to linked list
    mergeSort(&wordListHead);    // Sort list by word frequency (descending)

    // Display top 10 frequent words
    cout << "\nTop 10 Frequent Words in 1-Star Reviews:\n";
    WordNode* temp = wordListHead;
    int displayed = 0;
    while (temp && displayed < 10) {
        cout << temp->word << " : " << temp->count << endl;
        temp = temp->next;
        displayed++;
    }

    auto end = high_resolution_clock::now(); // End timing
    auto durationMs = duration_cast<milliseconds>(end - start);

    // Estimate memory used (hash table + linked list)
    size_t totalNodes = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
        WordNode* node = hashTable[i];
        while (node) {
            totalNodes++;
            node = node->next;
        }
    }

    WordNode* temp2 = wordListHead;
    while (temp2) {
        totalNodes++;
        temp2 = temp2->next;
    }

    size_t memoryUsed = totalNodes * sizeof(WordNode);

    cout << "\nExecution time: " << durationMs.count() << " ms" << endl;
    cout << "Approximate memory used: " << memoryUsed << " bytes" << endl;

    return 0;
}
