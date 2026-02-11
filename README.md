# Thread Pool

A C++20 Thread Pool implementation. This project demonstrates modern C++ concurrency patterns, RAII resource management, and thread-safe design without using third-party libraries (other than GoogleTest for testing).

## Features
- **Fixed Worker Pool**: Starts `N` threads that persist until destruction.
- **Task Queue**: Thread-safe blocking queue (no busy-waiting).
- **Modern API**: Accepts lambdas, functions, and functors with arguments via `submit()`.
- **Future Support**: Returns `std::future<>` for result retrieval and exception handling.
- **Graceful Shutdown**: Completes queued tasks before exiting; rejects new tasks after shutdown.
- **Sanitizer Ready**: Built-in CMake options for AddressSanitizer (ASan) and ThreadSanitizer (TSan).

## Requirements
- CMake >= 3.20
- C++20 compliant compiler (GCC 10+, Clang 11+, MSVC 19.28+)

## Build Instructions

```bash
mkdir build && cd build
# Standard build
cmake ..
cmake --build .

# Build with ThreadSanitizer (Detects data races)
cmake .. -DENABLE_TSAN=ON
cmake --build .
