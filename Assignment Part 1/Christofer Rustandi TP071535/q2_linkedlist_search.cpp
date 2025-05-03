#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

const int MAX = 5000;
const int TABLE_SIZE = 101; // Size of hash table

// Store one transaction
struct Transaction {
    string customerID;
    string product;
    string category;
    double price;
    string date;
    string paymentMethod;
};

// Node for hash table
struct HashNode {
    string category;
    int totalCount;
    int creditCardCount;
    HashNode* next;
};

// Hash function for strings
int hashFunc(string key) {
    int hash = 0;
    for (char ch : key)
        hash += (int)ch;
    return hash % TABLE_SIZE;
}

// Add or update a category in the hash table
void insertToHash(HashNode* table[], string category, string paymentMethod) {
    int index = hashFunc(category);
    HashNode* current = table[index];

    while (current != nullptr) {
        if (current->category == category) {
            current->totalCount++;
            if (paymentMethod == "Credit Card")
                current->creditCardCount++;
            return;
        }
        current = current->next;
    }

    // New category
    HashNode* newNode = new HashNode;
    newNode->category = category;
    newNode->totalCount = 1;
    newNode->creditCardCount = (paymentMethod == "Credit Card") ? 1 : 0;
    newNode->next = table[index];
    table[index] = newNode;
}

// Search for a category in the hash table
HashNode* searchCategory(HashNode* table[], string category) {
    int index = hashFunc(category);
    HashNode* current = table[index];

    while (current != nullptr) {
        if (current->category == category)
            return current;
        current = current->next;
    }
    return nullptr;
}

// Load transactions from file and update hash table
int loadAndHashTransactions(string filename, HashNode* hashTable[]) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: File not found." << endl;
        return 0;
    }

    string line;
    getline(file, line); // skip header
    int total = 0;

    while (getline(file, line)) {
        stringstream ss(line);
        string priceStr;
        Transaction tx;

        getline(ss, tx.customerID, ';');
        getline(ss, tx.product, ';');
        getline(ss, tx.category, ';');
        getline(ss, priceStr, ';');
        tx.price = stod(priceStr);
        getline(ss, tx.date, ';');
        getline(ss, tx.paymentMethod, ';');

        insertToHash(hashTable, tx.category, tx.paymentMethod);
        total++;
    }

    file.close();
    return total;
}

int main() {
    string transactionFile = "D:/C++ FOLDER/Final Assignment/Data CSV/transactions_cleaned.csv";
    HashNode* hashTable[TABLE_SIZE] = {nullptr}; // Initialize table

    int totalTransactions = loadAndHashTransactions(transactionFile, hashTable);

    // Get stats for Electronics category
    HashNode* electronics = searchCategory(hashTable, "Electronics");

    if (electronics == nullptr) {
        cout << "No Electronics transactions found.\n";
    } else {
        double percentage = (double)electronics->creditCardCount / electronics->totalCount * 100;
        cout << "Total Electronics purchases : " << electronics->totalCount << endl;
        cout << "Paid with Credit Card       : " << electronics->creditCardCount << endl;
        cout << "Percentage                  : " << percentage << "%" << endl;
    }

    return 0;
}
