# MapReduce-Style Parallel Processing

An implementation of MapReduce-style parallel processing using multithreading and multiprocessing in C++. This project demonstrates OS concepts including threading, process creation, IPC, and synchronization.

## Project Overview

This project implements three main components:

1. **Parallel Sorting (Multithreaded)** - Sorts arrays using multiple threads
2. **Parallel Sorting (Multiprocess)** - Sorts arrays using multiple processes
3. **Max-Value Aggregation** - Finds maximum value with thread synchronization

## Features

- Parallel sorting using merge sort algorithm
- Support for 1, 2, 4, and 8 workers (threads/processes)
- IPC using shared memory (mmap) for multiprocess version
- Thread synchronization using mutex and atomic operations
- Performance measurement and memory usage tracking
- Correctness verification

## Requirements

- C++ compiler with C++11 support (g++ recommended)
- POSIX-compliant system (Linux, macOS, WSL)
- pthread library
- Make utility

## Building the Project

### Build all programs:
```bash
make
```

### Build individual programs:
```bash
make sorting_multithread
make sorting_multiprocess
make max_aggregation
```

### Clean build artifacts:
```bash
make clean
```

## Running the Programs

### 1. Multithreaded Sorting

```bash
./sorting_multithread <array_size> <num_threads>
```

**Examples:**
```bash
./sorting_multithread 32 1        # Small array, 1 thread
./sorting_multithread 32 4        # Small array, 4 threads
./sorting_multithread 131072 8    # Large array, 8 threads
```

### 2. Multiprocess Sorting

```bash
./sorting_multiprocess <array_size> <num_processes>
```

**Examples:**
```bash
./sorting_multiprocess 32 1        # Small array, 1 process
./sorting_multiprocess 32 4        # Small array, 4 processes
./sorting_multiprocess 131072 8    # Large array, 8 processes
```

### 3. Max-Value Aggregation

```bash
./max_aggregation <array_size> <num_threads>
```

**Examples:**
```bash
./max_aggregation 32 1        # Small array, 1 thread
./max_aggregation 32 4        # Small array, 4 threads
./max_aggregation 131072 8    # Large array, 8 threads
```

## Automated Testing

### Run correctness tests (32 elements):
```bash
make test
```

### Run performance tests (131,072 elements):
```bash
make perf
```

## Implementation Details

### Sorting Programs

**Algorithm:** Merge Sort
- Time Complexity: O(n log n)
- Space Complexity: O(n)

**Process:**
1. Divide array into equal chunks based on number of workers
2. Each worker sorts its chunk independently
3. Parent/main thread merges all sorted chunks
4. Verify correctness and measure performance

**Multithreading:**
- Uses pthread library
- Workers share memory space
- Fast context switching

**Multiprocessing:**
- Uses fork() for process creation
- Shared memory via mmap (MAP_ANONYMOUS | MAP_SHARED)
- IPC for result communication
- Heavier context switching overhead

### Max Aggregation Program

**Synchronization Methods:**
1. **Mutex (pthread_mutex):** Traditional locking mechanism
2. **Atomic Operations (std::atomic):** Lock-free approach

**Process:**
1. Each thread finds local maximum in its chunk
2. Thread updates global maximum with synchronization
3. Compare mutex vs atomic performance
4. Measure synchronization overhead

## Architecture

### Multithreaded Sorting
```
Main Thread
    |
    ├─── Thread 1 (sorts chunk 0)
    ├─── Thread 2 (sorts chunk 1)
    ├─── Thread 3 (sorts chunk 2)
    └─── Thread N (sorts chunk N-1)
    |
    └─── Merge all chunks
```

### Multiprocess Sorting
```
Parent Process
    |
    ├─── Child Process 1 (sorts chunk 0) ──┐
    ├─── Child Process 2 (sorts chunk 1) ──┤
    ├─── Child Process 3 (sorts chunk 2) ──┤──► Shared Memory
    └─── Child Process N (sorts chunk N-1) ─┘
    |
    └─── Wait for children → Merge chunks
```

### Max Aggregation
```
Main Thread
    |
    ├─── Thread 1 → local_max → ┐
    ├─── Thread 2 → local_max → ├─→ Mutex/Atomic → Global Max
    ├─── Thread 3 → local_max → ┤
    └─── Thread N → local_max → ┘
```

## Performance Expectations

### Sorting
- **1 worker:** Baseline (sequential)
- **2 workers:** ~1.5-1.8x speedup
- **4 workers:** ~2.5-3x speedup
- **8 workers:** ~3-4x speedup (depends on CPU cores)

### Max Aggregation
- Minimal computation per thread
- Synchronization overhead more visible
- Atomic operations typically faster than mutex
- May see slowdown with many threads due to contention

## Testing Strategy

1. **Correctness (size = 32):**
   - Verify sorting correctness
   - Check all elements present
   - Validate max value is correct

2. **Performance (size = 131,072):**
   - Measure execution time
   - Compare thread/process counts
   - Analyze synchronization overhead
   - Evaluate memory usage

## Output Information

Each program provides:
- Array size and worker count
- Execution time (microseconds and milliseconds)
- Memory usage estimate
- Correctness verification
- Before/after snapshots (for small arrays)

## Common Issues and Solutions

**Issue:** `pthread_create` fails
- **Solution:** Check system thread limits with `ulimit -u`

**Issue:** `mmap` fails (multiprocess)
- **Solution:** Ensure sufficient shared memory available

**Issue:** Performance worse with more workers
- **Solution:** Normal for small arrays; overhead exceeds benefit

**Issue:** Compilation errors
- **Solution:** Ensure g++ supports C++11 (`-std=c++11`)

## Project Structure

```
472/
├── sorting_multithread.cpp   # Multithreaded sorting implementation
├── sorting_multiprocess.cpp  # Multiprocess sorting implementation
├── max_aggregation.cpp       # Max value aggregation with sync
├── Makefile                  # Build configuration
├── README.md                 # This file
├── REPORT.md                 # Detailed project report
└── run_tests.sh              # Automated testing script
```

## Performance Metrics

The programs track:
- **Execution Time:** Wall-clock time in microseconds
- **Memory Usage:** Approximate memory consumption
- **Synchronization Operations:** Count of sync operations (max aggregation)
- **Speedup:** Time improvement vs sequential baseline

## Author Notes

This implementation focuses on:
- Clean, well-commented code
- Proper resource management (no memory leaks)
- Race condition prevention
- Accurate performance measurement
- Educational clarity

## License

This is an educational project for operating systems coursework.
