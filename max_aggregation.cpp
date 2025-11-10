#include <iostream>
#include <pthread.h>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <atomic>

using namespace std;
using namespace chrono;

int global_max = INT_MIN;
pthread_mutex_t max_mutex;
atomic<int> atomic_max(INT_MIN);

atomic<int> sync_operations(0);

struct ThreadData {
    int* arr;
    int start;
    int end;
    int thread_id;
    bool use_atomic;
};

void* findMaxMutex(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    int local_max = INT_MIN;
    for (int i = data->start; i <= data->end; i++) {
        if (data->arr[i] > local_max) {
            local_max = data->arr[i];
        }
    }

    pthread_mutex_lock(&max_mutex);
    sync_operations++;
    if (local_max > global_max) {
        global_max = local_max;
    }
    pthread_mutex_unlock(&max_mutex);

    pthread_exit(NULL);
}

void* findMaxAtomic(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    int local_max = INT_MIN;
    for (int i = data->start; i <= data->end; i++) {
        if (data->arr[i] > local_max) {
            local_max = data->arr[i];
        }
    }

    int expected = atomic_max.load();
    while (local_max > expected &&
           !atomic_max.compare_exchange_weak(expected, local_max)) {
        sync_operations++;
    }
    sync_operations++;

    pthread_exit(NULL);
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

int findSequentialMax(int* arr, int size) {
    int max_val = INT_MIN;
    for (int i = 0; i < size; i++) {
        if (arr[i] > max_val) {
            max_val = arr[i];
        }
    }
    return max_val;
}

void runExperiment(int* arr, int size, int num_threads, bool use_atomic) {
    global_max = INT_MIN;
    atomic_max.store(INT_MIN);
    sync_operations.store(0);

    cout << "\n--- " << (use_atomic ? "Atomic" : "Mutex") << " Version ---" << endl;
    cout << "Number of threads: " << num_threads << endl;

    auto start = high_resolution_clock::now();

    pthread_t* threads = new pthread_t[num_threads];
    ThreadData* thread_data = new ThreadData[num_threads];

    int chunk_size = size / num_threads;

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].arr = arr;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? size - 1 : (i + 1) * chunk_size - 1;
        thread_data[i].thread_id = i;
        thread_data[i].use_atomic = use_atomic;

        void* (*thread_func)(void*) = use_atomic ? findMaxAtomic : findMaxMutex;

        int rc = pthread_create(&threads[i], NULL, thread_func, (void*)&thread_data[i]);
        if (rc) {
            cerr << "Error creating thread " << i << endl;
            exit(1);
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    int result_max = use_atomic ? atomic_max.load() : global_max;

    cout << "Maximum value found: " << result_max << endl;
    cout << "Execution time: " << duration.count() << " microseconds" << endl;
    cout << "Synchronization operations: " << sync_operations.load() << endl;
    cout << "Avg sync overhead: " << (sync_operations.load() > 0 ?
                                       duration.count() / (double)sync_operations.load() : 0)
         << " microseconds per operation" << endl;

    delete[] threads;
    delete[] thread_data;
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

    cout << "=== Max-Value Aggregation ===" << endl;
    cout << "Array size: " << size << endl;

    pthread_mutex_init(&max_mutex, NULL);

    int* arr = new int[size];
    srand(time(NULL));
    generateRandomArray(arr, size);

    printArray(arr, size);

    auto seq_start = high_resolution_clock::now();
    int correct_max = findSequentialMax(arr, size);
    auto seq_end = high_resolution_clock::now();
    auto seq_duration = duration_cast<microseconds>(seq_end - seq_start);

    cout << "\n--- Sequential Version (Baseline) ---" << endl;
    cout << "Maximum value: " << correct_max << endl;
    cout << "Execution time: " << seq_duration.count() << " microseconds" << endl;

    runExperiment(arr, size, num_threads, false);

    if (global_max == correct_max) {
        cout << "✓ Mutex version correct!" << endl;
    } else {
        cout << "✗ Mutex version INCORRECT! Expected: " << correct_max
             << ", Got: " << global_max << endl;
    }

    runExperiment(arr, size, num_threads, true);

    if (atomic_max.load() == correct_max) {
        cout << "✓ Atomic version correct!" << endl;
    } else {
        cout << "✗ Atomic version INCORRECT! Expected: " << correct_max
             << ", Got: " << atomic_max.load() << endl;
    }

    cout << "\n=== Performance Summary ===" << endl;
    cout << "Array size: " << size << endl;
    cout << "Number of threads: " << num_threads << endl;
    cout << "Sequential baseline: " << seq_duration.count() << " µs" << endl;

    size_t memory_used = size * sizeof(int) + num_threads * (sizeof(pthread_t) + sizeof(ThreadData));
    cout << "Approximate memory used: " << memory_used / 1024.0 << " KB" << endl;

    pthread_mutex_destroy(&max_mutex);
    delete[] arr;

    return 0;
}
