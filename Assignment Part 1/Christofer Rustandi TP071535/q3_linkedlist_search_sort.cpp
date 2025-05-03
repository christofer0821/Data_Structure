#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
using namespace std;

const int TABLE_SIZE = 101;
const int MAX_STOPS = 50;

struct WordNode {
    string word;
    int count;
    WordNode* next;
};

// Stop word list
string stopWords[MAX_STOPS] = {
    "the", "is", "a", "an", "to", "and", "in", "on", "for", "of",
    "with", "at", "by", "from", "up", "about", "into", "over", "after",
    "not", "be", "this", "that", "it", "as", "are", "was", "but", "or",
    "you", "i", "we", "they", "he", "she", "so", "very", "too", "if",
    "out", "no", "do", "did", "has", "have", "had", "can", "will", "just", "my"
};

// Hash table to store words and frequencies
WordNode* hashTable[TABLE_SIZE] = {nullptr};

// Linked list head (to be sorted later)
WordNode* wordListHead = nullptr;

// Helper: convert to lowercase
string toLowerCase(string text) {
    for (int i = 0; i < text.length(); i++)
        text[i] = tolower(text[i]);
    return text;
}

// Check if char is letter or number
bool isValidChar(char c) {
    return isalpha(c) || isdigit(c);
}

// Check stop word
bool isStopWord(string word) {
    for (int i = 0; i < MAX_STOPS; i++)
        if (stopWords[i] == word)
            return true;
    return false;
}

// Simple hash function
int hashFunc(string word) {
    int sum = 0;
    for (char c : word) sum += c;
    return sum % TABLE_SIZE;
}

// Insert or update word in hash table
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

    // New word: insert at front
    WordNode* newNode = new WordNode;
    newNode->word = word;
    newNode->count = 1;
    newNode->next = hashTable[index];
    hashTable[index] = newNode;
}

// Convert hash table to a linked list
void transferToLinkedList() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        WordNode* curr = hashTable[i];
        while (curr != nullptr) {
            WordNode* newNode = new WordNode;
            newNode->word = curr->word;
            newNode->count = curr->count;
            newNode->next = wordListHead;
            wordListHead = newNode;

            curr = curr->next;
        }
    }
}

// Merge Sort on Linked List
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

// Split list into halves
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

// Recursive merge sort
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

// Extract words from review text
void extractWords(string line) {
    string word = "";
    for (int i = 0; i <= line.length(); i++) {
        char c = line[i];
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

// Main
int main() {
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

    // Convert hash table to linked list
    transferToLinkedList();

    // Sort linked list by frequency
    mergeSort(&wordListHead);

    // Display top 10
    cout << "\nTop 10 Frequent Words in 1-Star Reviews:\n";
    WordNode* temp = wordListHead;
    for (int i = 0; i < 10 && temp != nullptr; i++) {
        cout << temp->word << " : " << temp->count << endl;
        temp = temp->next;
    }

    return 0;
}
