# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++11 -pthread
LDFLAGS = -pthread

# Target executables
TARGETS = sorting_multithread sorting_multiprocess max_aggregation

# Default target: build all
all: $(TARGETS)

# Build multithreaded sorting program
sorting_multithread: sorting_multithread.cpp
	$(CXX) $(CXXFLAGS) -o sorting_multithread sorting_multithread.cpp $(LDFLAGS)

# Build multiprocess sorting program
sorting_multiprocess: sorting_multiprocess.cpp
	$(CXX) $(CXXFLAGS) -o sorting_multiprocess sorting_multiprocess.cpp $(LDFLAGS)

# Build max aggregation program
max_aggregation: max_aggregation.cpp
	$(CXX) $(CXXFLAGS) -o max_aggregation max_aggregation.cpp $(LDFLAGS)

# Clean build artifacts
clean:
	rm -f $(TARGETS) *.o

# Run correctness tests (small input: 32 elements)
test: all
	@echo "=== Testing Correctness (32 elements) ==="
	@echo "\n--- Multithreaded Sorting ---"
	./sorting_multithread 32 1
	@echo "\n"
	./sorting_multithread 32 2
	@echo "\n"
	./sorting_multithread 32 4
	@echo "\n"
	./sorting_multithread 32 8
	@echo "\n\n--- Multiprocess Sorting ---"
	./sorting_multiprocess 32 1
	@echo "\n"
	./sorting_multiprocess 32 2
	@echo "\n"
	./sorting_multiprocess 32 4
	@echo "\n"
	./sorting_multiprocess 32 8
	@echo "\n\n--- Max Aggregation ---"
	./max_aggregation 32 1
	@echo "\n"
	./max_aggregation 32 2
	@echo "\n"
	./max_aggregation 32 4
	@echo "\n"
	./max_aggregation 32 8

# Run performance tests (large input: 131072 elements)
perf: all
	@echo "=== Performance Testing (131072 elements) ==="
	@echo "\n--- Multithreaded Sorting ---"
	@echo "1 thread:"
	./sorting_multithread 131072 1
	@echo "\n2 threads:"
	./sorting_multithread 131072 2
	@echo "\n4 threads:"
	./sorting_multithread 131072 4
	@echo "\n8 threads:"
	./sorting_multithread 131072 8
	@echo "\n\n--- Multiprocess Sorting ---"
	@echo "1 process:"
	./sorting_multiprocess 131072 1
	@echo "\n2 processes:"
	./sorting_multiprocess 131072 2
	@echo "\n4 processes:"
	./sorting_multiprocess 131072 4
	@echo "\n8 processes:"
	./sorting_multiprocess 131072 8
	@echo "\n\n--- Max Aggregation ---"
	@echo "1 thread:"
	./max_aggregation 131072 1
	@echo "\n2 threads:"
	./max_aggregation 131072 2
	@echo "\n4 threads:"
	./max_aggregation 131072 4
	@echo "\n8 threads:"
	./max_aggregation 131072 8

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build all programs (default)"
	@echo "  clean        - Remove all build artifacts"
	@echo "  test         - Run correctness tests with 32 elements"
	@echo "  perf         - Run performance tests with 131072 elements"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Individual programs:"
	@echo "  sorting_multithread  - Build multithreaded sorting"
	@echo "  sorting_multiprocess - Build multiprocess sorting"
	@echo "  max_aggregation      - Build max aggregation"

.PHONY: all clean test perf help
