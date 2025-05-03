#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <unordered_set>
using namespace std;

struct Record {
    string customerID, product, category, date, paymentMethod;
    double price;
};

struct Node {
    Record data;
    Node* next;
    Node(Record r) : data(r), next(nullptr) {}
};

// Convert date like "01/01/2022" to 20220101 for sorting
int dateToNumber(const string& date) {
    int d, m, y;
    char s;
    stringstream ss(date);
    ss >> d >> s >> m >> s >> y;
    return y * 10000 + m * 100 + d;
}

// Create a unique signature from a record
string makeSignature(const Record& r) {
    return r.customerID + "|" + r.product + "|" + r.category + "|" +
           to_string(r.price) + "|" + r.date + "|" + r.paymentMethod;
}

// Insert a new node at the end of the list
void append(Node*& head, Node*& tail, Record r, int& count) {
    Node* newNode = new Node(r);
    if (!head) head = tail = newNode;
    else {
        tail->next = newNode;
        tail = newNode;
    }
    count++;
}

// Merge two sorted linked lists
Node* merge(Node* a, Node* b) {
    if (!a) return b;
    if (!b) return a;

    if (dateToNumber(a->data.date) <= dateToNumber(b->data.date)) {
        a->next = merge(a->next, b);
        return a;
    } else {
        b->next = merge(a, b->next);
        return b;
    }
}

// Split a linked list into two halves
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

// Merge sort for linked list
void mergeSort(Node*& head) {
    if (!head || !head->next) return;

    Node *a, *b;
    split(head, a, b);
    mergeSort(a);
    mergeSort(b);
    head = merge(a, b);
}

// Load CSV data into linked list
void loadCSV(const string& file, set<string>& ids, Node*& head, Node*& tail,
             bool isTransaction, unordered_set<string>& shared,
             unordered_set<string>& seen, int& totalCount) {
    ifstream f(file);
    string line;
    getline(f, line); // Skip header

    while (getline(f, line)) {
        if (line.size() > 0 && (unsigned char)line[0] == 0xEF)
            line.erase(0, 3); // Remove BOM if present

        Record r;
        stringstream ss(line);
        string dummy, priceStr;

        if (isTransaction) {
            getline(ss, r.customerID, ';');
            getline(ss, r.product, ';');
            getline(ss, r.category, ';');
            getline(ss, priceStr, ';');
            r.price = stod(priceStr);
            getline(ss, r.date, ';');
            getline(ss, r.paymentMethod, ';');
        } else {
            getline(ss, dummy, ',');
            getline(ss, r.customerID, ',');
            getline(ss, dummy, ',');
            getline(ss, dummy, ',');
            r.product = r.category = r.paymentMethod = "";
            r.price = 0;
            r.date = "";
        }

        ids.insert(r.customerID);
        string sig = makeSignature(r);

        if (shared.count(r.customerID) && !seen.count(sig)) {
            if (!isTransaction && r.date.empty()) continue;
            append(head, tail, r, totalCount);
            seen.insert(sig);
        }
    }
    f.close();
}

// Display top 5 unique dates with up to 5 records each
void display(Node* head) {
    string currentDate = "";
    int shown = 0, dateCount = 0;

    cout << "\nShowing top 5 unique dates (up to 5 records each):\n";

    while (head && dateCount < 5) {
        if (head->data.date != currentDate) {
            currentDate = head->data.date;
            dateCount++;
            shown = 0;

            cout << "\nDate: " << currentDate << "\n";
            cout << "-------------------------------------------\n";
        }

        if (shown < 5) {
            cout << "- " << head->data.customerID << " | " << head->data.product
                 << " | " << head->data.category << " | RM" << head->data.price
                 << " | " << head->data.paymentMethod << "\n";
            shown++;
        }
        head = head->next;
    }
}

int main() {
    Node* head = nullptr, *tail = nullptr;
    set<string> transactionIDs, reviewIDs;
    unordered_set<string> sharedIDs, signatures;
    int totalCount = 0;

    string transactionFile = "D:/C++ FOLDER/Final Assignment/Data CSV/transactions_cleaned.csv";
    string reviewFile = "D:/C++ FOLDER/Final Assignment/Data CSV/reviews_cleaned.csv";

    // Step 1: Load IDs from both datasets
    loadCSV(transactionFile, transactionIDs, head, tail, true, sharedIDs, signatures, totalCount);
    head = tail = nullptr;
    signatures.clear(); totalCount = 0;

    loadCSV(reviewFile, reviewIDs, head, tail, false, sharedIDs, signatures, totalCount);
    for (const string& id : transactionIDs)
        if (reviewIDs.count(id)) sharedIDs.insert(id);

    head = tail = nullptr;
    signatures.clear(); totalCount = 0;

    // Step 2: Load valid and unique records from both files
    loadCSV(transactionFile, transactionIDs, head, tail, true, sharedIDs, signatures, totalCount);
    loadCSV(reviewFile, reviewIDs, head, tail, false, sharedIDs, signatures, totalCount);

    // Step 3: Sort and display
    mergeSort(head);
    display(head);

    // Step 4: Show total
    cout << "\nTotal unique merged transactions: " << totalCount << "\n";
    return 0;
}
