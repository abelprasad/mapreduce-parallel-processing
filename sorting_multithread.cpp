#include <iostream>
#include <pthread.h>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstring>

using namespace std;
using namespace chrono;

struct ThreadData {
    int* arr;
    int start;
    int end;
    int thread_id;
};

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

void* threadSort(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    mergeSort(data->arr, data->start, data->end);
    pthread_exit(NULL);
}

void mergeSortedChunks(int* arr, int num_threads, int size) {
    int chunk_size = size / num_threads;

    for (int step = 1; step < num_threads; step *= 2) {
        for (int i = 0; i < num_threads; i += 2 * step) {
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
        cout << "Usage: " << argv[0] << " <array_size> <num_threads>" << endl;
        return 1;
    }

    int size = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    if (num_threads <= 0 || size <= 0) {
        cout << "Invalid parameters. Size and threads must be positive." << endl;
        return 1;
    }

    if (num_threads > size) {
        num_threads = size;
    }

    cout << "=== Multithreaded Sorting ===" << endl;
    cout << "Array size: " << size << endl;
    cout << "Number of threads: " << num_threads << endl;

    int* arr = new int[size];
    srand(time(NULL));
    generateRandomArray(arr, size);

    cout << "Before sorting:" << endl;
    printArray(arr, size);

    auto start = high_resolution_clock::now();

    pthread_t* threads = new pthread_t[num_threads];
    ThreadData* thread_data = new ThreadData[num_threads];

    int chunk_size = size / num_threads;

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].arr = arr;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? size - 1 : (i + 1) * chunk_size - 1;
        thread_data[i].thread_id = i;

        int rc = pthread_create(&threads[i], NULL, threadSort, (void*)&thread_data[i]);
        if (rc) {
            cerr << "Error creating thread " << i << endl;
            return 1;
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    if (num_threads > 1) {
        mergeSortedChunks(arr, num_threads, size);
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

    size_t memory_used = size * sizeof(int) + num_threads * (sizeof(pthread_t) + sizeof(ThreadData));
    cout << "Approximate memory used: " << memory_used / 1024.0 << " KB" << endl;

    delete[] arr;
    delete[] threads;
    delete[] thread_data;

    return 0;
}
