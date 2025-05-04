#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
using namespace std;
using namespace chrono;

const int MAX_HASH = 100; // simple hash table size

// Linked list node for records
struct RecordNode {
    string category;
    string paymentMethod;
    RecordNode* next;
};

// Simple hash function for strings
int hashString(const string& str) {
    int hash = 0;
    for (char c : str) {
        hash += c;
    }
    return hash % MAX_HASH;
}

// Append to linked list
void append(RecordNode*& head, string category, string payment) {
    RecordNode* newNode = new RecordNode{category, payment, nullptr};
    if (!head) {
        head = newNode;
    } else {
        RecordNode* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
    }
}

int main() {
    auto start = high_resolution_clock::now(); // Start timing

    ifstream file("D:\\C++ FOLDER\\Final Assignment\\Data CSV\\transactions_cleaned.csv");
    if (!file) {
        cerr << "File could not be opened!" << endl;
        return 1;
    }

    string line;
    RecordNode* head = nullptr;
    int nodeCount = 0;

    getline(file, line); // Skip header

    while (getline(file, line)) {
        stringstream ss(line);
        string id, product, category, price, date, payment;
        getline(ss, id, ',');
        getline(ss, product, ',');
        getline(ss, category, ',');
        getline(ss, price, ',');
        getline(ss, date, ',');
        getline(ss, payment, ',');
        append(head, category, payment);
        nodeCount++;
    }

    // Step 1: Filter Electronics and hash-check for Credit Card
    int electronicsTotal = 0, creditCardCount = 0;
    int creditCardHash = hashString("Credit Card");

    RecordNode* current = head;
    while (current) {
        if (current->category == "Electronics") {
            electronicsTotal++;
            if (hashString(current->paymentMethod) == creditCardHash &&
                current->paymentMethod == "Credit Card") {
                creditCardCount++;
            }
        }
        current = current->next;
    }

    // Step 2: Calculate and print percentage
    double percentage = (electronicsTotal == 0) ? 0.0 :
                        (double)creditCardCount / electronicsTotal * 100.0;

    cout << "Total 'Electronics' purchases: " << electronicsTotal << endl;
    cout << "Using Credit Card: " << creditCardCount << endl;
    cout << fixed << setprecision(4);
    cout << "Percentage: " << percentage << "%" << endl;

    auto end = high_resolution_clock::now(); // End timing
    auto durationMs = duration_cast<milliseconds>(end - start);

    size_t nodeSize = sizeof(RecordNode);
    size_t totalMemory = nodeSize * nodeCount;

    cout << "Execution time: " << durationMs.count() << " ms" << endl;
    cout << "Approximate memory used: " << totalMemory << " bytes" << endl;

    return 0;
}
