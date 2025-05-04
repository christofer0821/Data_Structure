#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
using namespace std;
using namespace chrono;

const int MAX = 10000;

struct Transaction {
    string customerID;
    string product;
    string category;
    float price;
    string date;
    string paymentMethod;
};

void trim(string& s);
bool isValidCustomerID(const string& id);
void mergeSort(Transaction arr[], int left, int right);
void merge(Transaction arr[], int left, int mid, int right);
int dateToNumber(const string& date);

int main() {
    Transaction transactions[MAX];
    string uniqueCustomers[MAX];
    int transCount = 0, uniqueCustomerCount = 0;

    string files[] = {
        R"(D:\C++ FOLDER\Final Assignment\Data CSV\transactions_cleaned.csv)",
        R"(D:\C++ FOLDER\Final Assignment\Data CSV\reviews_cleaned.csv)"
    };

    auto start = high_resolution_clock::now();

    for (int f = 0; f < 2; f++) {
        ifstream file(files[f]);
        if (!file) continue;

        string line;
        getline(file, line);

        while (getline(file, line) && transCount < MAX) {
            stringstream ss(line);
            string id, product, category, priceStr, date, paymentMethod;

            getline(ss, id, ',');
            getline(ss, product, ',');
            getline(ss, category, ',');
            getline(ss, priceStr, ',');
            getline(ss, date, ',');
            getline(ss, paymentMethod, ',');

            trim(id); trim(product); trim(category); trim(priceStr); trim(date); trim(paymentMethod);
            if (!isValidCustomerID(id)) continue;

            bool exists = false;
            for (int i = 0; i < uniqueCustomerCount; ++i) {
                if (uniqueCustomers[i] == id) {
                    exists = true;
                    break;
                }
            }

            if (!exists && uniqueCustomerCount < MAX) {
                uniqueCustomers[uniqueCustomerCount++] = id;
            }

            try {
                float price = stof(priceStr);
                transactions[transCount++] = {id, product, category, price, date, paymentMethod};
            } catch (...) {
                continue;
            }
        }

        file.close();
    }

    mergeSort(transactions, 0, transCount - 1);

    cout << "\nTop 20 Transaction Dates:\n";

    if (transCount > 0) {
        string currentDate = transactions[0].date;
        int dailyCount = 1;
        int printed = 0;

        for (int i = 1; i < transCount && printed < 20; ++i) {
            if (transactions[i].date == currentDate) {
                dailyCount++;
            } else {
                cout << currentDate << ": " << dailyCount << " transactions" << endl;
                printed++;
                currentDate = transactions[i].date;
                dailyCount = 1;
            }
        }

        if (printed < 20) {
            cout << currentDate << ": " << dailyCount << " transactions" << endl;
        }
    }

    auto end = high_resolution_clock::now();
    auto durationMs = duration_cast<milliseconds>(end - start);

    size_t transactionSize = sizeof(Transaction);
    size_t totalMemoryUsed = transactionSize * transCount;

    cout << "\nTotal unique customers: " << uniqueCustomerCount << endl;
    cout << "Total transactions processed: " << transCount << endl;
    cout << "Execution time: " << durationMs.count() << " ms" << endl;
    cout << "Approximate memory used: " << totalMemoryUsed << " bytes" << endl;

    return 0;
}

void trim(string& s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == ' ' || s.back() == '\n')) s.pop_back();
    while (!s.empty() && (s.front() == '\r' || s.front() == ' ' || s.front() == '\n')) s.erase(s.begin());
}

bool isValidCustomerID(const string& id) {
    return !id.empty() && id != "NULL" && id != "unknown";
}

int dateToNumber(const string& date) {
    int d, m, y;
    char sep;
    stringstream ss(date);
    ss >> d >> sep >> m >> sep >> y;
    return y * 10000 + m * 100 + d;
}

void merge(Transaction arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Transaction* L = new Transaction[n1];
    Transaction* R = new Transaction[n2];

    for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
    for (int j = 0; j < n2; ++j) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (dateToNumber(L[i].date) <= dateToNumber(R[j].date)) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    delete[] L;
    delete[] R;
}

void mergeSort(Transaction arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}
