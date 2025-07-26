#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

constexpr uint64_t NUM_ITR = 500'000'000;

void single_thread() {
    uint64_t counter = 0;
    for (uint64_t i = 0; i < NUM_ITR; ++i) {
        ++counter;
    }
}

void single_thread_with_lock() {
    
    uint64_t counter = 0;
    std::mutex mtx;
    
    for (uint64_t i = 0; i < NUM_ITR; ++i) {
        std::lock_guard<std::mutex> lock(mtx);
        ++counter;
    }

}

void two_thread_with_lock() {
    
    uint64_t counter = 0;
    std::mutex mtx;
    
    auto worker = [&]() {
        for (uint64_t i = 0; i < NUM_ITR / 2; ++i) {
            std::lock_guard<std::mutex> lock(mtx);
            ++counter;
        }
    };

    std::thread t1(worker), t2(worker);
    t1.join();
    t2.join();

}

void single_thread_with_atomic() {

    std::atomic<uint64_t> counter{0};

    for (uint64_t i = 0; i < NUM_ITR; ++i) {
        // fetch add is an atomic operation that increments the value and 
        // returns the previous value ensuring safety without locks
        // memory_order_relaxed means no special sync or ordering enforced,
        // fastest atomic mode but only safe if we don't need to coordinate 
        // with other thread
        counter.fetch_add(1, std::memory_order_relaxed);
    }

}

void two_threads_with_atomic() {

    std::atomic<uint64_t> counter{0};

    auto worker = [&] () {
        for (uint64_t i = 0; i < NUM_ITR / 2; ++i) {
            // here memory_order_relaxed still ensures correctness as the
            // increments won't be lost but it doesn't gurantee any sync or 
            // ordering between the threads
            counter.fetch_add(1, std::memory_order_relaxed);
        }
    };

    std::thread t1(worker), t2(worker);
    t1.join();
    t2.join();
}

void single_thread_with_volatile() {

    volatile uint64_t counter = 0;

    for (uint64_t i = 0; i < NUM_ITR; i++) {
        ++counter;
    }

}

template <typename Func> 
void run_and_time(const std::string & label, Func func) {

    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << label << ": " << ms << " ms"  << std::endl;

}

int main() {

    run_and_time("Single thread", single_thread);
    run_and_time("String thread with lock", single_thread_with_lock);
    run_and_time("Two thread with lock", two_thread_with_lock);
    run_and_time("Single thread with CAS (atomic)", single_thread_with_atomic);
    run_and_time("Two threads with CAS (atomic)", two_threads_with_atomic);
    run_and_time("Single thread with volatile write", single_thread_with_volatile);
    
    return 0;
}

// output :
// Single thread: 122 ms
// Single thread with lock: 5571 ms
// Two thread with lock: 24952 ms
// Single thread with CAS (atomic): 2206 ms
// Two threads with CAS (atomic): 3958 ms
// Single thread with volatile write: 136 ms