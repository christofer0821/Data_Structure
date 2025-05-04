#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
using namespace std;
using namespace chrono;

// Structure to hold each transaction's data
struct Transaction {
    string customerID;
    string product;
    string category;
    float price;
    string date;
    string paymentMethod;
};

// Node structure for linked list
struct Node {
    Transaction data;
    Node* next;
};

// Function declarations
void trim(string& s);
bool isValidCustomerID(const string& id);
bool existsInArray(const string& id, string arr[], int count);
int dateToNumber(const string& date);
Node* mergeSort(Node* head);
Node* merge(Node* a, Node* b);
void split(Node* source, Node*& front, Node*& back);

int main() {
    Node* head = nullptr; // Head of the linked list
    string uniqueCustomers[10000]; // Stores unique customer IDs
    int uniqueCustomerCount = 0;
    int transCount = 0;

    // File paths (transaction and review CSVs)
    string files[] = {
        R"(D:\C++ FOLDER\Final Assignment\Data CSV\transactions_cleaned.csv)",
        R"(D:\C++ FOLDER\Final Assignment\Data CSV\reviews_cleaned.csv)"
    };

    // Start timing
    auto start = high_resolution_clock::now();

    // Load and process both files
    for (int f = 0; f < 2; f++) {
        ifstream file(files[f]);
        if (!file) continue;

        string line;
        getline(file, line); // Skip header

        while (getline(file, line)) {
            stringstream ss(line);
            string id, product, category, priceStr, date, paymentMethod;

            // Read each field from CSV line
            getline(ss, id, ',');
            getline(ss, product, ',');
            getline(ss, category, ',');
            getline(ss, priceStr, ',');
            getline(ss, date, ',');
            getline(ss, paymentMethod, ',');

            // Clean up strings
            trim(id); trim(product); trim(category); trim(priceStr); trim(date); trim(paymentMethod);
            if (!isValidCustomerID(id)) continue;

            // Check for uniqueness of customer ID
            bool exists = existsInArray(id, uniqueCustomers, uniqueCustomerCount);
            if (!exists && uniqueCustomerCount < 10000) {
                uniqueCustomers[uniqueCustomerCount++] = id;
            }

            // Create and append node to linked list
            try {
                float price = stof(priceStr);
                Node* newNode = new Node{{id, product, category, price, date, paymentMethod}, head};
                head = newNode;
                transCount++;
            } catch (...) {
                continue; // Skip if price conversion fails
            }
        }

        file.close();
    }

    // Sort linked list by date using merge sort
    head = mergeSort(head);

    // Group and display top 20 dates
    cout << "\nTop 20 Transaction Dates:\n";
    if (head) {
        string currentDate = head->data.date;
        int count = 0;
        int printed = 0;
        Node* curr = head;

        while (curr && printed < 20) {
            if (curr->data.date == currentDate) {
                count++;
            } else {
                cout << currentDate << ": " << count << " transactions" << endl;
                printed++;
                currentDate = curr->data.date;
                count = 1;
            }
            curr = curr->next;
        }

        if (printed < 20 && !currentDate.empty()) {
            cout << currentDate << ": " << count << " transactions" << endl;
        }
    }

    // Stop timing
    auto end = high_resolution_clock::now();
    auto durationMs = duration_cast<milliseconds>(end - start);

    // Calculate approximate memory usage
    size_t nodeSize = sizeof(Node);
    size_t totalMemoryUsed = nodeSize * transCount;

    // Final output
    cout << "\nTotal unique customers: " << uniqueCustomerCount << endl;
    cout << "Total transactions processed: " << transCount << endl;
    cout << "Execution time: " << durationMs.count() << " ms" << endl;
    cout << "Approximate memory used: " << totalMemoryUsed << " bytes" << endl;

    return 0;
}

// Remove leading and trailing whitespace and newline chars
void trim(string& s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == ' ' || s.back() == '\n')) s.pop_back();
    while (!s.empty() && (s.front() == '\r' || s.front() == ' ' || s.front() == '\n')) s.erase(s.begin());
}

// Validates customer ID field
bool isValidCustomerID(const string& id) {
    return !id.empty() && id != "NULL" && id != "unknown";
}

// Checks if a string exists in an array
bool existsInArray(const string& id, string arr[], int count) {
    for (int i = 0; i < count; i++) {
        if (arr[i] == id) return true;
    }
    return false;
}

// Converts date in DD/MM/YYYY format to sortable integer
int dateToNumber(const string& date) {
    int d, m, y;
    char slash;
    stringstream ss(date);
    ss >> d >> slash >> m >> slash >> y;
    return y * 10000 + m * 100 + d;
}

// Merge sort for linked list
Node* mergeSort(Node* head) {
    if (!head || !head->next) return head;

    Node *front, *back;
    split(head, front, back);

    front = mergeSort(front);
    back = mergeSort(back);

    return merge(front, back);
}

// Merges two sorted linked lists
Node* merge(Node* a, Node* b) {
    if (!a) return b;
    if (!b) return a;

    Node* result = nullptr;

    if (dateToNumber(a->data.date) <= dateToNumber(b->data.date)) {
        result = a;
        result->next = merge(a->next, b);
    } else {
        result = b;
        result->next = merge(a, b->next);
    }

    return result;
}

// Splits a linked list into two halves
void split(Node* source, Node*& front, Node*& back) {
    Node* slow = source;
    Node* fast = source->next;

    while (fast) {
        fast = fast->next;
        if (fast) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    front = source;
    back = slow->next;
    slow->next = nullptr;
}
