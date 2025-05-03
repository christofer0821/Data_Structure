#include <iostream>
#include <fstream>
#include <sstream>
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

// Add a new record to the linked list
void append(Node*& head, Node*& tail, Record r, int& count) {
    Node* newNode = new Node(r);
    if (!head) head = tail = newNode;
    else {
        tail->next = newNode;
        tail = newNode;
    }
    count++;
}

// Convert date string to sortable number (e.g., 01/01/2023 -> 20230101)
int dateToNumber(const string& date) {
    int d, m, y;
    char s;
    stringstream ss(date);
    ss >> d >> s >> m >> s >> y;
    return y * 10000 + m * 100 + d;
}

// Create a signature string from record for duplicate detection
string makeSignature(const Record& r) {
    return r.customerID + "|" + r.product + "|" + r.category + "|" +
           to_string(r.price) + "|" + r.date + "|" + r.paymentMethod;
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

// Perform merge sort on a linked list
void mergeSort(Node*& head) {
    if (!head || !head->next) return;

    Node *a, *b;
    split(head, a, b);
    mergeSort(a);
    mergeSort(b);
    head = merge(a, b);
}

// Check if a string exists in an array
bool stringExists(string arr[], int size, const string& val) {
    for (int i = 0; i < size; ++i)
        if (arr[i] == val) return true;
    return false;
}

// Add string to array if it's not already in it
void addString(string arr[], int& size, const string& val) {
    if (!stringExists(arr, size, val)) {
        arr[size++] = val;
    }
}

// Load data from CSV file into a linked list and track customer IDs and unique records
void loadCSV(const string& file, string idArr[], int& idCount,
             Node*& head, Node*& tail, bool isTransaction,
             string sharedIDs[], int sharedCount,
             string seen[], int& seenCount, int& totalCount) {

    ifstream f(file);
    string line;
    getline(f, line); // Skip header

    while (getline(f, line)) {
        if (line.size() > 0 && (unsigned char)line[0] == 0xEF)
            line.erase(0, 3); // Remove BOM

        Record r;
        string dummy, priceStr;
        stringstream ss(line);

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
            r.price = 0.0;
            r.date = "";
        }

        addString(idArr, idCount, r.customerID);

        string sig = makeSignature(r);
        if (stringExists(sharedIDs, sharedCount, r.customerID) &&
            !stringExists(seen, seenCount, sig)) {

            if (!isTransaction && r.date.empty()) continue;

            append(head, tail, r, totalCount);
            addString(seen, seenCount, sig);
        }
    }
    f.close();
}

// Show top 5 unique dates, with up to 5 records each
void display(Node* head) {
    string currentDate = "";
    int shown = 0, dateCount = 0;

    cout << "\nTop 5 Unique Dates (up to 5 records each):\n";

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
    Node* head = nullptr;
    Node* tail = nullptr;

    string transactionIDs[1000], reviewIDs[1000], sharedIDs[1000], signatures[2000];
    int transCount = 0, reviewCount = 0, sharedCount = 0, sigCount = 0;
    int totalCount = 0;

    string transactionFile = "D:/C++ FOLDER/Final Assignment/Data CSV/transactions_cleaned.csv";
    string reviewFile = "D:/C++ FOLDER/Final Assignment/Data CSV/reviews_cleaned.csv";

    // Load customer IDs from transactions
    loadCSV(transactionFile, transactionIDs, transCount, head, tail, true,
            sharedIDs, sharedCount, signatures, sigCount, totalCount);
    head = tail = nullptr;
    sigCount = totalCount = 0;

    // Load customer IDs from reviews
    loadCSV(reviewFile, reviewIDs, reviewCount, head, tail, false,
            sharedIDs, sharedCount, signatures, sigCount, totalCount);
    head = tail = nullptr;
    sigCount = totalCount = 0;

    // Find shared customer IDs between both files
    for (int i = 0; i < transCount; ++i) {
        if (stringExists(reviewIDs, reviewCount, transactionIDs[i])) {
            sharedIDs[sharedCount++] = transactionIDs[i];
        }
    }

    // Load and merge valid transaction and review records
    loadCSV(transactionFile, transactionIDs, transCount, head, tail, true,
            sharedIDs, sharedCount, signatures, sigCount, totalCount);
    loadCSV(reviewFile, reviewIDs, reviewCount, head, tail, false,
            sharedIDs, sharedCount, signatures, sigCount, totalCount);

    // Sort and display results
    mergeSort(head);
    display(head);

    cout << "\nTotal Unique Valid Records: " << totalCount << "\n";
    return 0;
}
