# Event Bus Architecture Inspired by LMAX Disruptor

## Reference
This document summarizes key insights from the LMAX Disruptor paper, which describes a high-performance alternative to bounded queues for exchanging data between concurrent threads. The architecture of `event_bus` is inspired by these principles.

---

## Key Points & Notes

### Why Not Traditional Queues?
- **Queues between processes** can be as slow as SSD I/O operations, adding hundreds of microseconds of latency.
- **Tightly coupled queues and logic** cause different system parts to compete for resources in multi-threaded environments, leading to contention and inefficiency.
- **Queues mix responsibilities**: producing, consuming, and storing data, which is inefficient in multi-threaded systems.

### LMAX System Highlights
- **Order matching engine**, real-time risk management, and highly available in-memory transaction processing.
- Achieved **25 million messages/second** and **latencies lower than 50 nanoseconds** (limit test).

### Concurrency Concepts
- **Mutual exclusion**: Ensures only one thread updates a resource at a time, preventing data corruption.
- **Visibility of change**: Ensures that when one thread updates a resource, other threads can see the change.
- **Mutual exclusion is costly**: Requires locking mechanisms, which are expensive, especially under contention.

### Locking and Its Costs
- **Locks**: Expensive due to OS arbitration and context switches, which can cause loss of cached data/instructions and slow down execution.
- **Context switches**: When multiple threads compete for a lock, the OS suspends waiting threads, further degrading performance.

### CAS (Compare-And-Swap) as an Alternative
- **CAS is more efficient than locks** for updating single-word memory locations.
- **How CAS works**: Atomically updates a value if it matches an expected old value; otherwise, the operation fails and must be retried.
- **Advantages**: No kernel context switch needed, significantly more efficient than locks.
- **Limitations**: Not free—requires the processor to lock its instruction pipeline and use memory barriers for visibility. Complex operations are hard to orchestrate and prove correct with CAS.

### Modern Processor Behavior
- **Out-of-order execution**: CPUs may execute instructions and memory stores out of order for performance.
- **Memory barriers**: Used to ensure memory changes appear in the correct order when threads share state.
- **Cache systems**: Modern CPUs use complex, coherent caches (like hash tables without chaining) to bridge the speed gap between CPU and memory.
- **Store buffers**: CPUs write data to a fast buffer before main memory, allowing continued execution without waiting for slower memory writes.
- **Invalidate queues**: In multi-core systems, when one core writes to memory, it must notify other cores to invalidate their cached copies (cache coherency). Invalidate queues allow quick acknowledgment of these messages for efficiency.

---

## Summary Table

| Mechanism         | Pros                                      | Cons/Limitations                              |
|-------------------|-------------------------------------------|-----------------------------------------------|
| Locks             | Simple, general-purpose                   | Expensive under contention, context switches  |
| CAS               | Efficient for simple updates, no context switch | Hard for complex logic, not free, needs retries |
| Queues            | Familiar, easy to use                     | Inefficient for high-performance concurrency  |

---

## Notes

- For high-performance, low-latency systems, avoid traditional queues and locks where possible.
- Use CAS for simple atomic updates, but be cautious with complex logic.
- Understand and leverage modern CPU features (caches, store buffers, memory barriers) for optimal concurrency.

---

**Reference:**  
LMAX Disruptor: High performance alternative to bounded queues for exchanging data between