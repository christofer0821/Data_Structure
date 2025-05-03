#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

const int MAX = 5000;

struct Transaction {
    string customerID;
    string product;
    string category;
    double price;
    string date;
    string paymentMethod;
};

// Load CSV data into an array
int loadTransactions(Transaction arr[], string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return 0;
    }

    string line;
    getline(file, line); // skip header
    int count = 0;

    while (getline(file, line) && count < MAX) {
        stringstream ss(line);
        string priceStr;

        getline(ss, arr[count].customerID, ';');
        getline(ss, arr[count].product, ';');
        getline(ss, arr[count].category, ';');
        getline(ss, priceStr, ';');
        arr[count].price = stod(priceStr);
        getline(ss, arr[count].date, ';');
        getline(ss, arr[count].paymentMethod, ';');
        count++;
    }

    file.close();
    return count;
}

// Simple case-insensitive string comparison
bool equalsIgnoreCase(string a, string b) {
    if (a.length() != b.length()) return false;
    for (int i = 0; i < a.length(); i++) {
        if (tolower(a[i]) != tolower(b[i])) return false;
    }
    return true;
}

// Sort array by category using bubble sort (need to use bubble sort because the binary search only can work with the sorted array)
void sortByCategory(Transaction arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].category > arr[j + 1].category) {
                Transaction temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// Binary search to find the starting index of "Electronics"
int binarySearchCategory(Transaction arr[], int n, string category) {
    int left = 0;
    int right = n - 1;
    int result = -1;

    while (left <= right) {
        int mid = (left + right) / 2;
        if (arr[mid].category == category) {
            result = mid;
            right = mid - 1; // continue searching to the left
        } else if (arr[mid].category < category) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return result;
}

int main() {
    string transactionFile = "D:/C++ FOLDER/Final Assignment/Data CSV/transactions_cleaned.csv";
    Transaction arr[MAX];
    int total = loadTransactions(arr, transactionFile);

    // Sort by category for binary search
    sortByCategory(arr, total);

    // Find the starting point of "Electronics" category
    string targetCategory = "Electronics";
    int index = binarySearchCategory(arr, total, targetCategory);

    if (index == -1) {
        cout << "No transactions found in 'Electronics' category.\n";
        return 0;
    }

    // Count total Electronics & those paid with Credit Card
    int totalElectronics = 0;
    int creditCardCount = 0;

    // From the found index, scan forward while category matches
    for (int i = index; i < total && arr[i].category == targetCategory; i++) {
        totalElectronics++;
        if (arr[i].paymentMethod == "Credit Card")
            creditCardCount++;
    }

    // Calculate percentage
    double percentage = (double)creditCardCount / totalElectronics * 100;

    // Output result
    cout << "Total 'Electronics' purchases: " << totalElectronics << endl;
    cout << "Using Credit Card: " << creditCardCount << endl;
    cout << "Percentage: " << percentage << "%" << endl;

    return 0;
}
