#!/bin/bash

echo "======================================"
echo "MapReduce Performance Testing Script"
echo "======================================"
echo ""

echo "Building programs..."
make clean
make

if [ $? -ne 0 ]; then
    echo "Error: Build failed. Please fix compilation errors."
    exit 1
fi

echo ""
echo "======================================"
echo "CORRECTNESS TESTS (size = 32)"
echo "======================================"
echo ""

echo "--- Multithreaded Sorting (32 elements) ---"
for workers in 1 2 4 8; do
    echo ""
    echo "Testing with $workers thread(s):"
    ./sorting_multithread 32 $workers
    echo ""
done

echo ""
echo "--- Multiprocess Sorting (32 elements) ---"
for workers in 1 2 4 8; do
    echo ""
    echo "Testing with $workers process(es):"
    ./sorting_multiprocess 32 $workers
    echo ""
done

echo ""
echo "--- Max Aggregation (32 elements) ---"
for workers in 1 2 4 8; do
    echo ""
    echo "Testing with $workers thread(s):"
    ./max_aggregation 32 $workers
    echo ""
done

echo ""
echo "======================================"
echo "PERFORMANCE TESTS (size = 131,072)"
echo "======================================"
echo ""

echo "--- Multithreaded Sorting (131,072 elements) ---"
for workers in 1 2 4 8; do
    echo ""
    echo "Testing with $workers thread(s):"
    ./sorting_multithread 131072 $workers
    echo ""
done

echo ""
echo "--- Multiprocess Sorting (131,072 elements) ---"
for workers in 1 2 4 8; do
    echo ""
    echo "Testing with $workers process(es):"
    ./sorting_multiprocess 131072 $workers
    echo ""
done

echo ""
echo "--- Max Aggregation (131,072 elements) ---"
for workers in 1 2 4 8; do
    echo ""
    echo "Testing with $workers thread(s):"
    ./max_aggregation 131072 $workers
    echo ""
done

echo ""
echo "======================================"
echo "All tests completed!"
echo "======================================"
