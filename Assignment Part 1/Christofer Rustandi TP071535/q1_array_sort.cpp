#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

// Custom Record Structure 
struct Record {
    string customerID;
    string product;
    string category;
    double price;
    string date;
    string paymentMethod;
};

// Linked List Node for Record 
struct RecordNode {
    Record data;
    RecordNode* next;
};

// Helper to Convert Date (e.g., 01/01/2022 → 20220101)
int dateToNumber(const string& date) {
    int day, month, year;
    char slash;
    stringstream ss(date);
    ss >> day >> slash >> month >> slash >> year;
    return year * 10000 + month * 100 + day;
}

// Append Record to Linked List 
void appendRecord(RecordNode*& head, Record data) {
    RecordNode* newNode = new RecordNode{data, nullptr};
    if (!head) head = newNode;
    else {
        RecordNode* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
    }
}

// Check if string exists in array (simulate set) 
bool stringExists(string arr[], int size, const string& val) {
    for (int i = 0; i < size; ++i) {
        if (arr[i] == val) return true;
    }
    return false;
}

// Add string to array if not already exists
void addStringIfNotExists(string arr[], int& size, const string& val) {
    if (!stringExists(arr, size, val)) {
        arr[size++] = val;
    }
}

// Create signature string from Record 
string makeSignature(const Record& r) {
    return r.customerID + "|" + r.product + "|" + r.category + "|" +
           to_string(r.price) + "|" + r.date + "|" + r.paymentMethod;
}

// Load Transactions
int loadTransactions(RecordNode*& head, string customerIDs[], int& idCount, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Cannot open file: " << filename << endl;
        return 0;
    }

    string line;
    getline(file, line); // Skip header
    if (line.size() > 0 && (unsigned char)line[0] == 0xEF) line.erase(0, 3); // Remove BOM

    int count = 0;
    while (getline(file, line)) {
        Record r;
        string priceStr;
        stringstream ss(line);

        getline(ss, r.customerID, ';');
        getline(ss, r.product, ';');
        getline(ss, r.category, ';');
        getline(ss, priceStr, ';');
        r.price = stod(priceStr);
        getline(ss, r.date, ';');
        getline(ss, r.paymentMethod, ';');

        appendRecord(head, r);
        addStringIfNotExists(customerIDs, idCount, r.customerID);
        count++;
    }

    file.close();
    return count;
}

// Load Reviews 
int loadReviews(RecordNode*& head, string customerIDs[], int& idCount, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Cannot open file: " << filename << endl;
        return 0;
    }

    string line;
    getline(file, line); // Skip header
    if (line.size() > 0 && (unsigned char)line[0] == 0xEF) line.erase(0, 3); // Remove BOM

    int count = 0;
    while (getline(file, line)) {
        Record r;
        string dummy;
        stringstream ss(line);

        getline(ss, dummy, ','); // Product ID
        getline(ss, r.customerID, ','); // Customer ID
        getline(ss, dummy, ','); // Rating
        getline(ss, dummy, ','); // Review Text

        r.product = "";
        r.category = "";
        r.price = 0.0;
        r.date = ""; // No date
        r.paymentMethod = "";

        appendRecord(head, r);
        addStringIfNotExists(customerIDs, idCount, r.customerID);
        count++;
    }

    file.close();
    return count;
}

// Merge Two Sorted Lists
RecordNode* mergeSortedLists(RecordNode* a, RecordNode* b) {
    if (!a) return b;
    if (!b) return a;

    if (dateToNumber(a->data.date) <= dateToNumber(b->data.date)) {
        a->next = mergeSortedLists(a->next, b);
        return a;
    } else {
        b->next = mergeSortedLists(a, b->next);
        return b;
    }
}

// Split Linked List into Halves
void splitList(RecordNode* source, RecordNode*& front, RecordNode*& back) {
    RecordNode* slow = source;
    RecordNode* fast = source->next;

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

// Merge Sort for Linked List
void mergeSort(RecordNode*& head) {
    if (!head || !head->next) return;

    RecordNode* a;
    RecordNode* b;
    splitList(head, a, b);

    mergeSort(a);
    mergeSort(b);

    head = mergeSortedLists(a, b);
}

// Check if Signature Exists
bool signatureExists(string sigs[], int count, string sig) {
    for (int i = 0; i < count; ++i) {
        if (sigs[i] == sig) return true;
    }
    return false;
}

// MAIN PROGRAM
int main() {
    RecordNode* transactions = nullptr;
    RecordNode* reviews = nullptr;
    RecordNode* combined = nullptr;

    string transactionIDs[1000], reviewIDs[1000], sharedIDs[1000], addedSigs[2000];
    int transCount = 0, reviewCount = 0, sharedCount = 0, sigCount = 0;

    string transactionFile = "transactions_cleaned.csv";
    string reviewFile = "reviews_cleaned.csv";

    loadTransactions(transactions, transactionIDs, transCount, transactionFile);
    loadReviews(reviews, reviewIDs, reviewCount, reviewFile);

    // Find Shared ID
    for (int i = 0; i < transCount; ++i) {
        if (stringExists(reviewIDs, reviewCount, transactionIDs[i])) {
            sharedIDs[sharedCount++] = transactionIDs[i];
        }
    }

    // Combine Transactions
    for (RecordNode* temp = transactions; temp; temp = temp->next) {
        if (stringExists(sharedIDs, sharedCount, temp->data.customerID)) {
            string sig = makeSignature(temp->data);
            if (!signatureExists(addedSigs, sigCount, sig)) {
                appendRecord(combined, temp->data);
                addedSigs[sigCount++] = sig;
            }
        }
    }

    // Combine Reviews
    for (RecordNode* temp = reviews; temp; temp = temp->next) {
        if (stringExists(sharedIDs, sharedCount, temp->data.customerID) && !temp->data.date.empty()) {
            string sig = makeSignature(temp->data);
            if (!signatureExists(addedSigs, sigCount, sig)) {
                appendRecord(combined, temp->data);
                addedSigs[sigCount++] = sig;
            }
        }
    }

    // Sort Combined 
    mergeSort(combined);

    // Show Top 5 Unique Dates
    cout << "\nTop 5 Unique Dates (max 5 records each):\n";
    string currentDate = "";
    int shown = 0, dateCount = 0;

    for (RecordNode* temp = combined; temp && dateCount < 5; temp = temp->next) {
        if (temp->data.date != currentDate) {
            currentDate = temp->data.date;
            dateCount++;
            shown = 0;
            cout << "\nDate: " << currentDate << "\n";
            cout << "-----------------------------\n";
        }

        if (shown < 5) {
            cout << "- " << temp->data.customerID << " | " << temp->data.product
                 << " | " << temp->data.category << " | RM" << temp->data.price
                 << " | " << temp->data.paymentMethod << "\n";
            shown++;
        }
    }

    cout << "\nTotal Valid Combined Records: " << sigCount << "\n";
    return 0;
}
