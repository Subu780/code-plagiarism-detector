#include <iostream>

int sumArray(int arr[], int n) {
    int total = 0; // compute sum
    for (int i = 0; i < n; ++i) {
        total += arr[i];
    }
    return total;
}

int main() {
    int data[] = {1, 2, 3, 4, 5};
    std::cout << sumArray(data, 5) << std::endl;
    return 0;
}
