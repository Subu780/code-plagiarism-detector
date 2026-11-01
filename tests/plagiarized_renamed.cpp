#include <iostream>
#include <vector>

/* Copied and renamed variables with extra spaces */
int sumArray(int nums[], int count) {
    int accumulator = 0;
    for (int idx = 0; idx < count; ++idx) {
        accumulator += nums[idx];
    }
    return accumulator;
}

int main() {
    int values[] = {10, 20, 30, 40, 50};
    std::cout << sumArray(values, 5) << "\n";
    return 0;
}
