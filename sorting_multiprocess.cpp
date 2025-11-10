#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstring>

using namespace std;
using namespace chrono;

void merge(int* arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int* L = new int[n1];
    int* R = new int[n2];

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    delete[] L;
    delete[] R;
}

void mergeSort(int* arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void mergeSortedChunks(int* arr, int num_processes, int size) {
    int chunk_size = size / num_processes;

    for (int step = 1; step < num_processes; step *= 2) {
        for (int i = 0; i < num_processes; i += 2 * step) {
            int left = i * chunk_size;
            int mid = min((i + step) * chunk_size - 1, size - 1);
            int right = min((i + 2 * step) * chunk_size - 1, size - 1);

            if (mid < right) {
                merge(arr, left, mid, right);
            }
        }
    }
}

void generateRandomArray(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 10000;
    }
}

void printArray(int* arr, int size) {
    if (size <= 32) {
        cout << "Array: ";
        for (int i = 0; i < size; i++) {
            cout << arr[i] << " ";
        }
        cout << endl;
    }
}

bool isSorted(int* arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <array_size> <num_processes>" << endl;
        return 1;
    }

    int size = atoi(argv[1]);
    int num_processes = atoi(argv[2]);

    if (num_processes <= 0 || size <= 0) {
        cout << "Invalid parameters. Size and processes must be positive." << endl;
        return 1;
    }

    if (num_processes > size) {
        num_processes = size;
    }

    cout << "=== Multiprocess Sorting ===" << endl;
    cout << "Array size: " << size << endl;
    cout << "Number of processes: " << num_processes << endl;

    int* arr = (int*)mmap(NULL, size * sizeof(int),
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (arr == MAP_FAILED) {
        cerr << "Error: Failed to create shared memory" << endl;
        return 1;
    }

    srand(time(NULL));
    generateRandomArray(arr, size);

    cout << "Before sorting:" << endl;
    printArray(arr, size);

    auto start = high_resolution_clock::now();

    int chunk_size = size / num_processes;

    for (int i = 0; i < num_processes; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            cerr << "Error creating process " << i << endl;
            return 1;
        } else if (pid == 0) {
            int start_idx = i * chunk_size;
            int end_idx = (i == num_processes - 1) ? size - 1 : (i + 1) * chunk_size - 1;

            mergeSort(arr, start_idx, end_idx);

            exit(0);
        }
    }

    for (int i = 0; i < num_processes; i++) {
        wait(NULL);
    }

    if (num_processes > 1) {
        mergeSortedChunks(arr, num_processes, size);
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    cout << "After sorting:" << endl;
    printArray(arr, size);

    if (isSorted(arr, size)) {
        cout << "✓ Array is correctly sorted!" << endl;
    } else {
        cout << "✗ Error: Array is NOT sorted correctly!" << endl;
    }

    cout << "Execution time: " << duration.count() << " microseconds" << endl;
    cout << "Execution time: " << duration.count() / 1000.0 << " milliseconds" << endl;

    size_t memory_used = size * sizeof(int);
    cout << "Approximate memory used: " << memory_used / 1024.0 << " KB" << endl;

    munmap(arr, size * sizeof(int));

    return 0;
}
