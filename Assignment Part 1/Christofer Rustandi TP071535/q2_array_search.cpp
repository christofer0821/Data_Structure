#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono> // for time tracking

using namespace std;
using namespace chrono;

const int MAX = 10000;

struct Record {
    string category;
    string paymentMethod;
};

// Binary search helper (find exact matches)
bool binarySearch(string arr[], int size, const string& key) {
    int low = 0, high = size - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (arr[mid] == key) return true;
        else if (arr[mid] < key) low = mid + 1;
        else high = mid - 1;
    }
    return false;
}

// Simple bubble sort for strings
void bubbleSort(string arr[], int size) {
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                string temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main() {
    auto start = high_resolution_clock::now(); // Start time

    ifstream file("D:\\C++ FOLDER\\Final Assignment\\Data CSV\\transactions_cleaned.csv");
    if (!file) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    string line;
    Record records[MAX];
    int count = 0;

    getline(file, line); // skip header
    while (getline(file, line) && count < MAX) {
        stringstream ss(line);
        string id, product, category, price, date, payment;

        getline(ss, id, ',');
        getline(ss, product, ',');
        getline(ss, category, ',');
        getline(ss, price, ',');
        getline(ss, date, ',');
        getline(ss, payment, ',');

        records[count++] = {category, payment};
    }

    // Step 2: Extract Electronics only
    string payments[MAX];
    int electronicsCount = 0;
    for (int i = 0; i < count; ++i) {
        if (records[i].category == "Electronics") {
            payments[electronicsCount++] = records[i].paymentMethod;
        }
    }

    // Step 3: Sort for binary search
    bubbleSort(payments, electronicsCount);

    // Step 4: Count Credit Card using simple comparison
    int creditCardCount = 0;
    for (int i = 0; i < electronicsCount; ++i) {
        if (payments[i] == "Credit Card") creditCardCount++;
    }

    // Step 5: Output results
    double percentage = (electronicsCount == 0) ? 0.0 :
                        (double)creditCardCount / electronicsCount * 100.0;

    cout << "Total 'Electronics' purchases: " << electronicsCount << endl;
    cout << "Using Credit Card: " << creditCardCount << endl;
    cout << fixed << setprecision(4);
    cout << "Percentage: " << percentage << "%" << endl;

    auto end = high_resolution_clock::now(); // End time
    auto durationMs = duration_cast<milliseconds>(end - start);

    // Memory usage: size of arrays used
    size_t totalMemory = sizeof(records[0]) * count + sizeof(payments[0]) * electronicsCount;

    cout << "Execution time: " << durationMs.count() << " ms" << endl;
    cout << "Approximate memory used: " << totalMemory << " bytes" << endl;

    return 0;
}
