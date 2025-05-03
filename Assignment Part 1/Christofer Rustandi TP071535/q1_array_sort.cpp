#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <unordered_set>
using namespace std;

struct Record {
    string customerID;
    string product;
    string category;
    double price;
    string date;
    string paymentMethod;
};

// Convert date to sortable number (e.g., 01/01/2022 → 20220101)
int dateToNumber(const string& date) {
    int day, month, year;
    char slash;
    stringstream ss(date);
    ss >> day >> slash >> month >> slash >> year;
    return year * 10000 + month * 100 + day;
}

// Merge sort helper
void merge(vector<Record>& arr, int left, int mid, int right) {
    vector<Record> leftArr(arr.begin() + left, arr.begin() + mid + 1);
    vector<Record> rightArr(arr.begin() + mid + 1, arr.begin() + right + 1);
    int i = 0, j = 0, k = left;

    while (i < leftArr.size() && j < rightArr.size()) {
        if (dateToNumber(leftArr[i].date) <= dateToNumber(rightArr[j].date))
            arr[k++] = leftArr[i++];
        else
            arr[k++] = rightArr[j++];
    }

    while (i < leftArr.size()) arr[k++] = leftArr[i++];
    while (j < rightArr.size()) arr[k++] = rightArr[j++];
}

// Merge sort wrapper
void mergeSort(vector<Record>& arr, int left, int right) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

// Create a unique string signature from the record
string makeSignature(const Record& r) {
    return r.customerID + "|" + r.product + "|" + r.category + "|" +
           to_string(r.price) + "|" + r.date + "|" + r.paymentMethod;
}

// Load transactions
int loadTransactions(vector<Record>& transactions, set<string>& customerIDs, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Failed to open file: " << filename << endl;
        return 0;
    }

    string line;
    getline(file, line);
    if (line.size() > 0 && (unsigned char)line[0] == 0xEF) {
        // Remove UTF-8 BOM if present
        line.erase(0, 3);
    }

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

        transactions.push_back(r);
        customerIDs.insert(r.customerID);
    }

    file.close();
    return transactions.size();
}

// Load reviews
int loadReviews(vector<Record>& reviews, set<string>& customerIDs, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Failed to open file: " << filename << endl;
        return 0;
    }

    string line;
    getline(file, line);
    if (line.size() > 0 && (unsigned char)line[0] == 0xEF) {
        line.erase(0, 3);
    }

    while (getline(file, line)) {
        Record r;
        stringstream ss(line);
        string dummy;

        getline(ss, dummy, ','); // Product ID
        getline(ss, r.customerID, ','); // Customer ID
        getline(ss, dummy, ','); // Rating
        getline(ss, dummy, ','); // Review Text

        r.product = "";
        r.category = "";
        r.price = 0.0;
        r.date = ""; // Reviews may not have date
        r.paymentMethod = "";

        reviews.push_back(r);
        customerIDs.insert(r.customerID);
    }

    file.close();
    return reviews.size();
}

int main() {
    vector<Record> transactions, reviews, combined;
    set<string> transactionIDs, reviewIDs;
    unordered_set<string> addedSignatures;

    string transactionFile = "D:/C++ FOLDER/Final Assignment/Data CSV/transactions_cleaned.csv";
    string reviewFile = "D:/C++ FOLDER/Final Assignment/Data CSV/reviews_cleaned.csv";

    loadTransactions(transactions, transactionIDs, transactionFile);
    loadReviews(reviews, reviewIDs, reviewFile);

    // Find Customer IDs present in both files
    set<string> sharedCustomerIDs;
    for (const string& id : transactionIDs) {
        if (reviewIDs.count(id)) {
            sharedCustomerIDs.insert(id);
        }
    }

    // Add transactions with shared IDs and unique full record
    for (const Record& r : transactions) {
        if (sharedCustomerIDs.count(r.customerID)) {
            string sig = makeSignature(r);
            if (!addedSignatures.count(sig)) {
                combined.push_back(r);
                addedSignatures.insert(sig);
            }
        }
    }

    // Add reviews with shared ID, a date, and no duplicate
    for (const Record& r : reviews) {
        if (sharedCustomerIDs.count(r.customerID) && !r.date.empty()) {
            string sig = makeSignature(r);
            if (!addedSignatures.count(sig)) {
                combined.push_back(r);
                addedSignatures.insert(sig);
            }
        }
    }

    // Sort by date using merge sort
    mergeSort(combined, 0, combined.size() - 1);

    // Print top 5 unique dates, up to 5 records per date
    cout << "\nShowing top 5 unique dates (5 records per date):\n";
    string currentDate = "";
    int shownCount = 0;
    int dateCount = 0;

    for (const Record& r : combined) {
        if (r.date != currentDate) {
            if (dateCount >= 5) break; // Only top 5 unique dates
            currentDate = r.date;
            shownCount = 0;
            dateCount++;

            cout << "\nDate: " << currentDate << "\n";
            cout << "-------------------------------------------\n";
        }

        if (shownCount < 5) {
            cout << "- " << r.customerID << " | " << r.product
                 << " | " << r.category << " | RM" << r.price
                 << " | " << r.paymentMethod << "\n";
            shownCount++;
        }
    }

    cout << "\nTotal unique merged records (valid + no duplicates): " << combined.size() << "\n";
    return 0;
}
